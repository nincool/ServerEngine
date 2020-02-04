///--------------------------------------------------------------------
/// 文件名:		PubRedis.cpp
/// 内  容:		公共数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PubRedis.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"

bool CPubRedis::Init()
{
	CObjectRedis::Init();

	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CPubRedis::AfterInit()
{
	CObjectRedis::AfterInit();

	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_START_LOAD_PUB, this, (NetMsgFun)&CPubRedis::OnReqiureStartLoadPub);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_START_LOAD_GUILD, this, (NetMsgFun)&CPubRedis::OnReqiureStartLoadGuild);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_CREATE_PUB, this, (NetMsgFun)&CPubRedis::OnReqiureCreatePub);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_CREATE_GUILD, this, (NetMsgFun)&CPubRedis::OnReqiureCreateGuild);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_PUB_DATA, this, (NetMsgFun)&CPubRedis::OnReqiureSavePub);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_GUILD_DATA, this, (NetMsgFun)&CPubRedis::OnReqiureSaveGuild);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_PUB_DATA_EXIT, this, (NetMsgFun)&CPubRedis::OnReqiureSavePub);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_GUILD_DATA_EXIT, this, (NetMsgFun)&CPubRedis::OnReqiureSaveGuild);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_DELETE_GUILD, this, (NetMsgFun)&CPubRedis::OnReqiureDeleteGuild);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_DELETE_PUB, this, (NetMsgFun)&CPubRedis::OnReqiureDeletePub);

	return true;
}

bool CPubRedis::CreateGuild(OuterMsg::PubOpData& createGuild, int nServerID)
{
	if (createGuild.PubName().empty())
	{
		QLOG_WARING << __FUNCTION__ << " name is empty";
		return false;
	}

	if (createGuild.pub_id().data1() == 0 && createGuild.pub_id().data2() == 0)
	{
		QLOG_WARING << __FUNCTION__ << " guid is null";
		return false;
	}

	NFGUID id;
	PBConvert::ToNF(createGuild.pub_id(), id);
	const std::string& strName = createGuild.PubName();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		//名字和UID保存在一个节点上，用于重名检查
		if (!xNoSqlDriver->HEXISTS(m_pCommonRedis->GetGuildListCacheKey(nServerID), strName))
		{
			//the name ref to the guid
			xNoSqlDriver->HSET(m_pCommonRedis->GetGuildListCacheKey(nServerID), strName, id.ToString());
		}
	}

	return true;
}

bool CPubRedis::CreatePub(OuterMsg::PubOpData& createPub, int nServerID)
{
	if (createPub.PubName().empty())
	{
		QLOG_WARING << __FUNCTION__ << " name is empty";
		return false;
	}

	if (createPub.pub_id().data1() == 0 && createPub.pub_id().data2() == 0)
	{
		QLOG_WARING << __FUNCTION__ << " guid is null";
		return false;
	}

	NFGUID id;
	PBConvert::ToNF(createPub.pub_id(), id);
	const std::string& strName = createPub.PubName();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		//名字和UID保存在一个节点上，用于重名检查
		if (!xNoSqlDriver->HEXISTS(m_pCommonRedis->GetPubListCacheKey(nServerID), strName))
		{
			//the name ref to the guid
			xNoSqlDriver->HSET(m_pCommonRedis->GetPubListCacheKey(nServerID), strName, id.ToString());
		}
	}

	return true;
}

bool CPubRedis::ExistPubName(const std::string& strPubName, int nServerID)
{
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		return xNoSqlDriver->HEXISTS(m_pCommonRedis->GetPubListCacheKey(nServerID), strPubName);
	}

	return false;
}

bool CPubRedis::ExistGuildName(const std::string& strGuildName, int nServerID)
{
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		return xNoSqlDriver->HEXISTS(m_pCommonRedis->GetGuildListCacheKey(nServerID), strGuildName);
	}

	return false;
}

// 请求开始加载Guild
void CPubRedis::OnReqiureStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		if (xNoSqlDriver->EXISTS(m_pCommonRedis->GetGuildListCacheKey(nServerID)))
		{
			// 获取所有Guild信息
			string_pair_vector vValueList;
			if (!xNoSqlDriver->HGETALL(m_pCommonRedis->GetGuildListCacheKey(nServerID), vValueList))
			{
				QLOG_WARING << __FUNCTION__ << " HGETALL failed serverid:" << nServerID;
				return;
			}

			//回复加载的Guild
			OuterMsg::PropertyList xPropertyList;
			StringListToPropertyList(&xPropertyList, vValueList);
			m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_START_LOAD_GUILD, xPropertyList, nSockIndex);

			for (int i = 0; i < vValueList.size(); ++i)
			{
				const std::string& strKey = vValueList[i].first;
				const std::string& strValue = vValueList[i].second;

				NFGUID id;
				id.FromString(strValue);

				std::string strData;
				if (!this->LoadObjectData(id, strData))
				{
					QLOG_ERROR << __FUNCTION__ << " LoadObjectData failed, class name :" << LC::Guild::ThisName()
						<< " ObjectID:" << id.ToString();
				}

				m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_GUILD_DATA, strData, nSockIndex);
			}
		}

		// 通知加载完成
		OuterMsg::MsgEmpty xMsg;
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_GUILD_COMPLETE, xMsg, nSockIndex);
	}

	return;
}

// 请求开始加载pub
void CPubRedis::OnReqiureStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		if (xNoSqlDriver->EXISTS(m_pCommonRedis->GetPubListCacheKey(nServerID)))
		{
			// 获取所有PUB信息
			string_pair_vector vValueList;
			if (!xNoSqlDriver->HGETALL(m_pCommonRedis->GetPubListCacheKey(nServerID), vValueList))
			{
				QLOG_WARING << __FUNCTION__ << " HGETALL failed serverid:" << nServerID;
				return;
			}
			
			//回复加载的PUB
			OuterMsg::PropertyList xPropertyList;
			StringListToPropertyList(&xPropertyList, vValueList);
			m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_START_LOAD_PUB, xPropertyList, nSockIndex);

			for (int i = 0; i < vValueList.size(); ++i)
			{
				const std::string& strValue = vValueList[i].second;

				NFGUID id;
				id.FromString(strValue);
				std::string strData;
				if (!this->LoadObjectData(id, strData))
				{
					QLOG_ERROR << __FUNCTION__ << " LoadObjectData failed, class name :" << vValueList[i].first
						<< " ObjectID:" << id.ToString();
				}

				m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PUB_DATA, strData, nSockIndex);
			}
		}

		// 通知加载完成
		OuterMsg::MsgEmpty xMsg;
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PUB_COMPLETE, xMsg, nSockIndex);
	}

	return;
}

// 请求创建pub
void CPubRedis::OnReqiureCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	const std::string& strName = xMsg.PubName();

	//重名检查
	NFGUID id;
	PBConvert::ToNF(xMsg.pub_id(), id);

	OuterMsg::AckResult xAckResult;
	xAckResult.set_name(strName);
	PBConvert::ToPB(id, *xAckResult.mutable_obj_id());

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	if (ExistPubName(strName, nServerID))
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_CREATE_PUB_NAME_REPEAT);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_PUB, xAckResult, nSockIndex);

		QLOG_WARING << __FUNCTION__ << " already Exist PubName:" << strName
			<< " serverid:" << nServerID;

		return;
	}

	if (CreatePub(xMsg, nServerID))
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_SUCCESS);

		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_PUB, xAckResult, nSockIndex);
	}
	else
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_CREATE_PUB_DB_ERROR);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_PUB, xAckResult, nSockIndex);

		QLOG_ERROR << __FUNCTION__ << " CreatePub failed";
	}
}

// 请求创建guild
void CPubRedis::OnReqiureCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	const std::string& strName = xMsg.PubName();

	//重名检查
	NFGUID id;
	PBConvert::ToNF(xMsg.pub_id(), id);

	OuterMsg::AckResult xAckResult;
	xAckResult.set_name(strName);
	PBConvert::ToPB(id, *xAckResult.mutable_obj_id());

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	if (ExistGuildName(strName, nServerID))
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_CREATE_GUILD_NAME_REPEAT);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_GUILD, xAckResult, nSockIndex);
		
		QLOG_WARING << __FUNCTION__ << " already Exist GuildName:" << strName
			<< " serverid:" << nServerID;

		return;
	}
	
	if (CreateGuild(xMsg, nServerID))
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_SUCCESS);

		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_GUILD, xAckResult, nSockIndex);
	}
	else
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_CREATE_GUILD_DB_ERROR);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_GUILD, xAckResult, nSockIndex);

		QLOG_ERROR << __FUNCTION__ << " CreateGuild failed";
	}
}

// 请求保存pub
void CPubRedis::OnReqiureSavePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubNameData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubNameData ParseFromArray failed";
		return;
	}

	NFGUID nPubID;
	PBConvert::ToNF(xMsg.object_data().ident(), nPubID);

	if (!this->SaveObjectData(nPubID, xMsg.object_data().SerializeAsString()))
	{
		return;
	}
	
	if (nMsgID == OuterMsg::SS_REQ_SAVE_PUB_DATA_EXIT)
	{
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_OBJECT_SAVED, nPubID.ToString(), nSockIndex);
	}
}

// 请求保存guild
void CPubRedis::OnReqiureSaveGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	NFGUID nGuildID;
	PBConvert::ToNF(xMsg.ident(), nGuildID);

	if (!this->SaveObjectData(nGuildID, std::string(msg, nLen)))
	{
		return;
	}

	if (nMsgID == OuterMsg::SS_REQ_SAVE_GUILD_DATA_EXIT)
	{
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_OBJECT_SAVED, nGuildID.ToString(), nSockIndex);
	}
}

void CPubRedis::OnReqiureDeleteGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		if (xNoSqlDriver->EXISTS(m_pCommonRedis->GetGuildListCacheKey(nServerID)))
		{
			// 获取所有PUB信息
			if (!xNoSqlDriver->HDEL(m_pCommonRedis->GetGuildListCacheKey(nServerID), xMsg.PubName()))
			{
				QLOG_ERROR << __FUNCTION__ << " del failed " << m_pCommonRedis->GetGuildListCacheKey(nServerID)
					<< " " << xMsg.PubName();
				return;
			}
		}

		// 删除obj数据库
		NFGUID id;
		PBConvert::ToNF(xMsg.pub_id(), id);
		if (!this->DeleteObject(id))
		{
			QLOG_ERROR << __FUNCTION__ << " del object data failed name:" << xMsg.PubName();
		}
	}
}

void CPubRedis::OnReqiureDeletePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver)
	{
		if (xNoSqlDriver->EXISTS(m_pCommonRedis->GetPubListCacheKey(nServerID)))
		{
			// 获取所有PUB信息
			if (!xNoSqlDriver->HDEL(m_pCommonRedis->GetPubListCacheKey(nServerID), xMsg.PubName()))
			{
				QLOG_ERROR << __FUNCTION__ << " del failed " << m_pCommonRedis->GetPubListCacheKey(nServerID)
					<< " " << xMsg.PubName();
				return;
			}
		}

		// 删除obj数据库
		NFGUID id;
		PBConvert::ToNF(xMsg.pub_id(), id);
		if (!this->DeleteObject(id))
		{
			QLOG_ERROR << __FUNCTION__ << " del object data failed name:" << xMsg.PubName();
		}
	}
}

void CPubRedis::StringListToPropertyList(OuterMsg::PropertyList* pPropertyList, string_pair_vector& vValueList)
{
	for (auto &it : vValueList)
	{
		OuterMsg::PropertyString* pPropertyData = pPropertyList->add_property_string();
		if (pPropertyData == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pPropertyData == NULL";
			return;
		}

		pPropertyData->set_name(it.first);
		pPropertyData->set_data(it.second);
	}
}