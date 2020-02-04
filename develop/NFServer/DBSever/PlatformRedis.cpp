///--------------------------------------------------------------------
/// 文件名:		PlatformRedis.cpp
/// 内  容:		平台服数据库
/// 说  明:		
/// 创建日期:	2019年10月12日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PlatformRedis.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"

bool CPlatformRedis::Init()
{
	CObjectRedis::Init();

	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CPlatformRedis::AfterInit()
{
	CObjectRedis::AfterInit();

	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_START_LOAD_PLATFORM, this, (NetMsgFun)&CPlatformRedis::OnReqiureStartLoadPlatform);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_CREATE_PLATFORM, this, (NetMsgFun)&CPlatformRedis::OnReqiureCreateBattleHelper);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_PLATFORM_DATA, this, (NetMsgFun)&CPlatformRedis::OnReqiureSavePlatform);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_PLATFORM_DATA_EXIT, this, (NetMsgFun)&CPlatformRedis::OnReqiureSavePlatform);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_DELETE_PLATFORM, this, (NetMsgFun)&CPlatformRedis::OnReqiureDeletePlatform);

	return true;
}

bool CPlatformRedis::CreateBattleHelper(OuterMsg::PubOpData& createHelper, int nServerID)
{
	if (createHelper.PubName().empty())
	{
		QLOG_WARING << __FUNCTION__ << " name is empty";
		return false;
	}

	if (createHelper.pub_id().data1() == 0 && createHelper.pub_id().data2() == 0)
	{
		QLOG_WARING << __FUNCTION__ << " uid is null";
		return false;
	}

	NFGUID id;
	PBConvert::ToNF(createHelper.pub_id(), id);
	const std::string& strName = createHelper.PubName();

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver)
	{
		if (!xNoSqlDriver->HSET(m_pCommonRedis->GetPlatformListCacheKey(nServerID), id.ToString(), strName))
		{
			QLOG_WARING << __FUNCTION__ << " HSET failed uid:" << id;
			return false;
		}
	}

	return true;
}

// 请求开始加载Platform
void CPlatformRedis::OnReqiureStartLoadPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver)
	{
		auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
		if (pServerData == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
			return;
		}

		const int& nServerID = pServerData->report.server_id();
		if (xNoSqlDriver->EXISTS(m_pCommonRedis->GetPlatformListCacheKey(nServerID)))
		{
			// 获取所有Guild信息
			string_pair_vector vValueList;
			if (!xNoSqlDriver->HGETALL(m_pCommonRedis->GetPlatformListCacheKey(nServerID), vValueList))
			{
				QLOG_ERROR << __FUNCTION__ << " HGETALL failed";
				return;
			}

			//回复加载的PLATFORM
			OuterMsg::PropertyList xPropertyList;
			StringListToPropertyList(&xPropertyList, vValueList);
			m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_START_LOAD_PLATFORM, xPropertyList, nSockIndex);

			for (int i = 0; i < vValueList.size(); ++i)
			{
				const std::string& strKey = vValueList[i].first;
				const std::string& strValue = vValueList[i].second;

				NFGUID id;
				id.FromString(strKey);

				std::string strData;
				if (!this->LoadObjectData(id, strData))
				{
					QLOG_ERROR << __FUNCTION__ << " failed class name:" << strValue
						<< " ObjectID:" << id.ToString();
				}

				m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PLATFORM_DATA, strData, nSockIndex);
			}

			// 通知加载完成
			OuterMsg::AckEventResult xMsg;
			xMsg.set_event_code(OuterMsg::EGameEventCode::EGEC_SUCCESS);
			m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PLATFORM_COMPLETE, xMsg, nSockIndex);
		}
		else
		{
			// 通知加载完成
			OuterMsg::AckEventResult xMsg;
			xMsg.set_event_code(OuterMsg::EGameEventCode::EGEC_NOEXIST_PLATFORM_DB);
			m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PLATFORM_COMPLETE, xMsg, nSockIndex);
		}
	}

	return;
}

// 请求创建BattleHelper
void CPlatformRedis::OnReqiureCreateBattleHelper(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " PubOpData ParseFromArray failed";
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

	if (CreateBattleHelper(xMsg, nServerID))
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_SUCCESS);

		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_PLATFORM, xAckResult, nSockIndex);
	}
	else
	{
		xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_CREATE_PLATFORM_DB_ERROR);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_CREATE_PLATFORM, xAckResult, nSockIndex);

		QLOG_ERROR << __FUNCTION__ << " CreateBattleHelper failed";
	}
}

// 请求保存pub
void CPlatformRedis::OnReqiureSavePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubNameData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " PubNameData ParseFromArray failed";
		return;
	}

	NFGUID nPubID;
	PBConvert::ToNF(xMsg.object_data().ident(), nPubID);
	if (!this->SaveObjectData(nPubID, xMsg.object_data().SerializeAsString()))
	{
		return;
	}
	
	if (nMsgID == OuterMsg::SS_REQ_SAVE_PLATFORM_DATA_EXIT)
	{
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_OBJECT_SAVED, nPubID.ToString(), nSockIndex);
	}
}

void CPlatformRedis::OnReqiureDeletePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	OuterMsg::PubOpData xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver)
	{
		// 删除obj数据库
		NFGUID id;
		PBConvert::ToNF(xMsg.pub_id(), id);

		if (xNoSqlDriver->EXISTS(m_pCommonRedis->GetPlatformListCacheKey(nServerID)))
		{
			// 获取所有PUB信息
			if (!xNoSqlDriver->HDEL(m_pCommonRedis->GetPlatformListCacheKey(nServerID), id.ToString()))
			{
				std::ostringstream str;
				QLOG_ERROR << __FUNCTION__ << " del failed:" << m_pCommonRedis->GetPlatformListCacheKey(nServerID)
					<< " " << xMsg.PubName();
				return;
			}
		}

		if (!this->DeleteObject(id))
		{
			QLOG_ERROR << __FUNCTION__ << " del object data failed " << xMsg.PubName()
				<< " GUID: " << id.ToString();
		}
	}
}

void CPlatformRedis::StringListToPropertyList(OuterMsg::PropertyList* pPropertyList, string_pair_vector& vValueList)
{
	for (auto &it : vValueList)
	{
		OuterMsg::PropertyString* pPropertyData = pPropertyList->add_property_string();
		if (pPropertyData == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " pPropertyData == NULL";
			return;
		}

		pPropertyData->set_name(it.second);
		pPropertyData->set_data(it.first);
	}
}