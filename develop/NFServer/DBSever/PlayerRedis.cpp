///--------------------------------------------------------------------
/// 文件名:		PlayerRedis.cpp
/// 内  容:		玩家数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PlayerRedis.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/NFTool.hpp"

// 每次发送最大条数
constexpr int GUID_NAME_COUNT = 10000;

// 明文显示属性定义
const std::set<std::string> FRANK_PROP_INT =
{
	LC::Player::TotalSec(),
	LC::Player::CreateTime(),
	LC::Player::OfflineTime(),
	LC::Player::OpenTime(),
};
const std::set<std::string> FRANK_PROP_STR =
{
	LC::Player::LiteData(),
	LC::Player::Account(),
	LC::Player::Name(),
};
const std::set<std::string> FRANK_PROP_DOUBLE = {};
const std::set<std::string> FRANK_PROP_OBJECT = {};

bool CPlayerRedis::Init()
{
	CObjectRedis::Init();

	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetClient = pPluginManager->FindModule<NetClientModule>();
	m_pDBServer = pPluginManager->FindModule<CDBServer>();

	return true;
}

bool CPlayerRedis::AfterInit()
{
	CObjectRedis::AfterInit();

	//请求角色列表
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ROLE_LIST, this, (NetMsgFun)&CPlayerRedis::OnRequireRoleListProcess);
	//创建角色回调
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_PRECREATE_ROLE, this, (NetMsgFun)&CPlayerRedis::OnPreCreateRoleGameProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_CREATE_ROLE, this, (NetMsgFun)&CPlayerRedis::OnCreateRoleGameProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_DELETE_ROLE, this, (NetMsgFun)&CPlayerRedis::OnDeleteRoleGameProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_LOAD_ROLE_DATA, this, (NetMsgFun)&CPlayerRedis::OnLoadRoleDataProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_ROLE_DATA, this, (NetMsgFun)&CPlayerRedis::OnSaveRoleDataProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SAVE_ROLE_DATA_EXIT, this, (NetMsgFun)&CPlayerRedis::OnSaveRoleDataProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_PLAYER_RENAME, this, (NetMsgFun)&CPlayerRedis::OnRename);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_ALTER_PLAYER_DATA, this, (NetMsgFun)&CPlayerRedis::OnGetPlayerData);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_LOAD_PLAYER_GUID_NAME, this, (NetMsgFun)&CPlayerRedis::OnGetPlayerGuidName);

	return true;
}

bool CPlayerRedis::ExistPlayerName(const std::string& strPlayerName, int nServerID)
{
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " xNoSqlDriver == NULL";
		return false;
	}

	if (xNoSqlDriver->HEXISTS(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strPlayerName))
	{
		return true;
	}

	std::string strPlayerNameTemp = m_pCommonRedis->GetPlayerNameCacheKey(strPlayerName);
	if (xNoSqlDriver->EXISTS(strPlayerNameTemp))
	{
		return true;
	}

	return false;
}

//请求角色列表
void CPlayerRedis::OnRequireRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NF_SHARE_PTR<IRedisDriver> xDataNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_ACCOUNT);
	if (xNoSqlDriver == nullptr || xDataNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " xNoSqlDriver/xDataNoSqlDriver == NULL";
		return;
	}

	NFGUID nClientID;
	OuterMsg::ReqRoleList xMsg;
	if (!ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqRoleList ReceivePB failed";
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	std::string strAccountKey = m_pCommonRedis->GetAccountCacheKey(xMsg.account(), nServerID);
	string_vector vec_guid;
	if (!xNoSqlDriver->SMEMBERS(strAccountKey, vec_guid))
	{
		OuterMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
		xAckRoleLiteInfoList.set_account(xMsg.account());
		std::string strData;
		AddNetPB(xAckRoleLiteInfoList, nClientID, strData);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_ROLE_LIST, strData, nSockIndex);

		return;
	}

	OuterMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
	xAckRoleLiteInfoList.set_account(xMsg.account());

	for (int i = 0; i < vec_guid.size(); ++i)
	{
		std::vector<std::string> keys = { LC::Player::LiteData(), LC::Player::OpenTime() };
		std::vector<std::string> values;
		std::string strObjectKey = m_pCommonRedis->GetObjectCacheKey(vec_guid[i]);
		if (!xDataNoSqlDriver->HMGET(strObjectKey, keys, values))
		{
			QLOG_ERROR << __FUNCTION__ << " HMGET failed. key:" << strObjectKey;
			continue;
		}

		if (keys.size() != values.size())
		{
			QLOG_ERROR << __FUNCTION__ << " HMGET failed. key:" << strObjectKey;
			continue;
		}

		auto* data = xAckRoleLiteInfoList.add_char_data();
		if (data == nullptr)
		{
			continue;
		}

		PBConvert::ToPB(NFGUID(vec_guid[i]), *data->mutable_id());
		data->set_info(std::move(values[0]));
		data->set_open_time(::lexical_cast<int64_t>(values[1]));
	}

	std::string strData;
	AddNetPB(xAckRoleLiteInfoList, nClientID, strData);
	m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_ROLE_LIST, strData, nSockIndex);

}

//预创建角色回调
void CPlayerRedis::OnPreCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqCreateRole xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
		return;
	}

	const std::string& strAccount = xMsg.account();
	const std::string& strName = xMsg.noob_name();
	NFGUID nClientID;
	PBConvert::ToNF(xMsg.client_id(), nClientID);

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	OuterMsg::EGameEventCode eCode = OuterMsg::EGEC_UNKOWN_ERROR;
	do 
	{
		NF_SHARE_PTR<IRedisDriver> xRoleNameNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
		NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_ACCOUNT);
		if (xNoSqlDriver == nullptr || xRoleNameNoSqlDriver == nullptr)
		{
			eCode = OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
			break;
		}

		std::string strValue;
		if (xRoleNameNoSqlDriver->HGET(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strName, strValue))
		{
			if (!strValue.empty())
			{
				eCode = OuterMsg::EGEC_CREATE_ROLE_NAME_REPEAT;
			}
		}

		std::string strPlayerNameTemp = m_pCommonRedis->GetPlayerNameCacheKey(strName);
		if (xNoSqlDriver->EXISTS(strPlayerNameTemp))
		{
			eCode = OuterMsg::EGEC_CREATE_ROLE_NAME_REPEAT;
			break;
		}

		const std::string strAccountKey = m_pCommonRedis->GetAccountCacheKey(strAccount, nServerID);
		string_vector vec_members;
		if (xNoSqlDriver->SMEMBERS(strAccount, vec_members))
		{
			if (vec_members.size() >= m_pDBServer->GetCreateRoleAmount())
			{
				eCode = OuterMsg::EGEC_CREATE_ROLE_EXIT;
				break;
			}
		}

		// 暂时占用此名称
		if (!xNoSqlDriver->SET(strPlayerNameTemp, strAccount))
		{
			eCode = OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
			break;
		}
		if (!xNoSqlDriver->EXPIRE(strPlayerNameTemp, ONE_MIN_S))
		{
			eCode = OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
			break;
		}

		eCode = OuterMsg::EGEC_SUCCESS;

	} while (false);

	if (eCode == OuterMsg::EGEC_SUCCESS)
	{
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_PRECREATE_ROLE, std::string(msg, nLen), nSockIndex);
	}
	else
	{
		OuterMsg::AckEventResult xAck;
		xAck.set_event_code(eCode);
		std::string strData;
		AddNetPB(xAck, nClientID, strData);
		m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_CREATE_ROLE, strData, nSockIndex);
	}
}

//创建角色回调
void CPlayerRedis::OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqCreateRole xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
		return;
	}

	const std::string& strAccount = xMsg.account();
	const std::string& strName = xMsg.noob_name();
	NFGUID nClientID;
	PBConvert::ToNF(xMsg.client_id(), nClientID);

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	OuterMsg::EGameEventCode eCode = CreateRole(xMsg, nServerID);

	//创建角色成功 回复客户端
	OuterMsg::AckEventResult xAck;
	xAck.set_event_code(eCode);
	*xAck.mutable_event_object() = xMsg.mutable_roleData()->ident();
	std::string strData;
	AddNetPB(xAck, nClientID, strData);
	m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_CREATE_ROLE, strData, nSockIndex);
}

//在数据库创建角色
OuterMsg::EGameEventCode CPlayerRedis::CreateRole(OuterMsg::ReqCreateRole& createRoleData, int nServerID)
{
	NF_SHARE_PTR<IRedisDriver> xRoleNameNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_ACCOUNT);
	if (xNoSqlDriver == nullptr || xRoleNameNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " xNoSqlDriver/xRoleNameNoSqlDriver == NULL";
		return OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
	}

	if (createRoleData.account().empty() || createRoleData.noob_name().empty())
	{
		QLOG_ERROR << __FUNCTION__ << " account/name is empty";
		return OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
	}

	if (createRoleData.roleData().ident().data1() == 0 && createRoleData.roleData().ident().data2() == 0)
	{
		QLOG_ERROR << __FUNCTION__ << " uid is null";
		return OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
	}

	NFGUID id;
	PBConvert::ToNF(createRoleData.roleData().ident(), id);
	const std::string& strAccount = createRoleData.account();
	const std::string& strName = createRoleData.noob_name();
	const std::string strID = id.ToString();

	std::string strValue;
	if (xRoleNameNoSqlDriver->HGET(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strName, strValue))
	{
		if (strID != strValue)
		{
			QLOG_INFO << "create role failed. name exist account:" << strAccount 
				<< " name:" << strName 
				<< " ID:" << strID 
				<< " value:" << strValue;
			return OuterMsg::EGEC_CREATE_ROLE_NAME_REPEAT;
		}
	}
	else
	{
		strValue.clear();
		std::string strPlayerNameTemp = m_pCommonRedis->GetPlayerNameCacheKey(strName);
		if (xNoSqlDriver->GET(strPlayerNameTemp, strValue))
		{
			if (strValue != strAccount)
			{
				QLOG_INFO << "create role failed. name exist account:" << strAccount
					<< " name:" << strName 
					<< " ID:" << strID 
					<< " value:" << strValue;
				return OuterMsg::EGEC_CREATE_ROLE_NAME_REPEAT;
			}
		}

		if (!xRoleNameNoSqlDriver->HSET(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strName, strID))
		{
			QLOG_ERROR << __FUNCTION__ << " create role failed. HSET false account:" << strAccount
				<< " name:" << strName
				<< " ID:" << strID;
			return OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
		}
	}

	const std::string strAccountKey = m_pCommonRedis->GetAccountCacheKey(strAccount, nServerID);
	string_vector vec_members;
	if (xNoSqlDriver->SMEMBERS(strAccount, vec_members))
	{
		if (vec_members.size() >= m_pDBServer->GetCreateRoleAmount())
		{
			QLOG_ERROR << __FUNCTION__ << " create role failed. role number:" << vec_members.size()
				<< " over " << m_pDBServer->GetCreateRoleAmount();

			xRoleNameNoSqlDriver->HDEL(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strName);
			return OuterMsg::EGEC_CREATE_ROLE_EXIT;
		}
	}

	if (xNoSqlDriver->SADD(strAccountKey, strID) <= 0)
	{
		QLOG_ERROR << __FUNCTION__ << " account:" << strAccount
			<< " add object failed obj:" << id 
			<< " name:" << strName;

		xRoleNameNoSqlDriver->HDEL(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strName);
		return OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
	}

	auto& xPack = createRoleData.roleData();

	// 获取明文保存的属性
	string_vector vec_key;
	string_vector vec_value;
	GetFrankData(xPack, vec_key, vec_value);

	// 保存全部数据
	vec_key.push_back(m_pCommonRedis->GetObjectDataCacheKey());
	vec_value.emplace_back(std::move(xPack.SerializeAsString()));

	if (!SaveData(id, vec_key, vec_value))
	{
		xRoleNameNoSqlDriver->HDEL(m_pCommonRedis->GetPlayerListCacheKey(nServerID), strName);
		xNoSqlDriver->SREM(strAccountKey, string_vector({ strID }));
		return OuterMsg::EGEC_CREATE_ROLE_DB_ERROR;
	}

	// 同步GUID与名称的映射表
	OuterMsg::GuidNameList list;
	auto* pData = list.add_list();
	if (pData != nullptr)
	{
		PBConvert::ToPB(id, *pData->mutable_id());
		pData->set_name(strName);

		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_ACK_LOAD_PLAYER_GUID_NAME, list);
	}

	return OuterMsg::EGEC_SUCCESS;
}

void CPlayerRedis::OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	//NFGUID nClientID;
	//OuterMsg::ReqDeleteRole xMsg;
	//if (!ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
	//{
	//	return;
	//}

	//OuterMsg::AckRoleLiteInfoList xAckRoleLiteInfoList;
	//xAckRoleLiteInfoList.set_account(xMsg.account());
	//std::string strData;
	//AddNetPB(xAckRoleLiteInfoList, nClientID, strData);
	//m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_ROLE_LIST, strData, nSockIndex);
}

void CPlayerRedis::OnLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID nClientID;
	OuterMsg::ReqEnterGameServer xMsg;
	if (!ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqEnterGameServer ReceivePB failed";
		return;
	}

	NFGUID nRoleID = PBToNF(xMsg.id());

	std::string strMsg;
	if (!this->LoadObjectData(nRoleID, strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " LoadObjectData failed, account:" << xMsg.account()
			<< " RoleID:" << nRoleID;
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_ROLE_DATA, strMsg, nSockIndex);
}

void CPlayerRedis::OnSaveRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}
	
	NFGUID nRoleID;
	PBConvert::ToNF(xMsg.ident(), nRoleID);

	// 获取明文保存的属性
	string_vector vec_key;
	string_vector vec_value;
	GetFrankData(xMsg, vec_key, vec_value);

	// 保存全部数据
	vec_key.push_back(m_pCommonRedis->GetObjectDataCacheKey());
	vec_value.emplace_back(std::move(std::string(msg, nLen)));

	if (!SaveData(nRoleID, vec_key, vec_value))
	{
		return;
	}

	if (nMsgID == OuterMsg::SS_REQ_SAVE_ROLE_DATA_EXIT)
	{
		// 向world反馈保存成功
		std::string strGUID;
		if (!xMsg.ident().SerializeToString(&strGUID))
		{
			QLOG_ERROR << __FUNCTION__ << " serialize failed player:" << nRoleID;
			return;
		}

		m_pNetClient->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_PLAYER_SAVED, strGUID);
	}
}

//改名
void CPlayerRedis::OnRename(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PlayerRename rename;
	rename.ParseFromArray(msg, nLen);

	NFGUID player_id;
	PBConvert::ToNF(rename.player_id(), player_id);

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	bool bRename = Rename(rename.account(), player_id, rename.player_name_old(), rename.player_name_new(), nServerID);
	if (!bRename)
	{
		//改名失败
		*rename.mutable_player_name_new() = rename.player_name_old();
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_PLAYER_RENAME, rename, nSockIndex);
}

//改名
bool CPlayerRedis::Rename(const std::string& strAccount, const NFGUID& self, const std::string& oldName, const std::string& newName, int nServerID)
{
	if (ExistPlayerName(newName, nServerID))
	{
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> xRoleNameNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	NF_SHARE_PTR<IRedisDriver> pSelfDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (nullptr == xRoleNameNoSqlDriver || nullptr == pSelfDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " xRoleNameNoSqlDriver/pSelfDriver == NULL";
		return false;
	}

	//改名字库
	bool bRoleNameDriver_D = xRoleNameNoSqlDriver->HDEL(m_pCommonRedis->GetPlayerListCacheKey(nServerID), oldName);
	bool bRoleNameDriver_S = xRoleNameNoSqlDriver->HSET(m_pCommonRedis->GetPlayerListCacheKey(nServerID), newName, self.ToString());
	
	if (!bRoleNameDriver_D
		|| !bRoleNameDriver_S
		)
	{
		//还原
		xRoleNameNoSqlDriver->HDEL(m_pCommonRedis->GetPlayerListCacheKey(nServerID), newName);
		xRoleNameNoSqlDriver->HSET(m_pCommonRedis->GetPlayerListCacheKey(nServerID), oldName, self.ToString());

		return false;
	}

	// 同步GUID与名称的映射表
	OuterMsg::GuidNameList list;
	auto* pData = list.add_list();
	if (pData != nullptr)
	{
		PBConvert::ToPB(self, *pData->mutable_id());
		pData->set_name(newName);

		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_ACK_LOAD_PLAYER_GUID_NAME, list);
	}
	
	return true;
}

void CPlayerRedis::OnGetPlayerData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID player;
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	PBConvert::ToNF(xMsg, player);

	std::string strMsg;
	if (!this->LoadObjectData(player, strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " LoadObjectData failed uid;" << player;
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_ALTER_PLAYER_DATA, strMsg, nSockIndex);
}

void CPlayerRedis::OnGetPlayerGuidName(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NF_SHARE_PTR<IRedisDriver> xRoleNameNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_NAME);
	if (xRoleNameNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " xRoleNameNoSqlDriver == NULL";
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	string_pair_vector vec;
	if (!xRoleNameNoSqlDriver->HGETALL(m_pCommonRedis->GetPlayerListCacheKey(nServerID), vec))
	{
		QLOG_ERROR << __FUNCTION__ << " HGETALL failed";
		return;
	}

	OuterMsg::GuidNameList list;
	NFGUID player;
	for (int i = 0; i < vec.size(); ++i)
	{
		auto* pData = list.add_list();
		if (pData == nullptr)
		{
			continue;
		}
		auto& pair = vec[i];
		player.FromString(pair.second);
		PBConvert::ToPB(player, *pData->mutable_id());
		pData->set_name(pair.first);

		if (list.list_size() > GUID_NAME_COUNT)
		{
			list.set_state(OuterMsg::GuidNameList::INIT_ADD);
			m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PLAYER_GUID_NAME, list, nSockIndex);
			list.clear_list();
		}
	}

	list.set_state(OuterMsg::GuidNameList::INIT_FINISH);
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOAD_PLAYER_GUID_NAME, list, nSockIndex);
}

bool CPlayerRedis::GetFrankData(const OuterMsg::ObjectDataPack& xMsg, string_vector& vec_key, string_vector& vec_value)
{
	if (!FRANK_PROP_STR.empty())
	{
		auto setFrank = FRANK_PROP_STR;
		auto& prop_list = xMsg.property_list().property_string();
		for (int i = 0; i < prop_list.size(); ++i)
		{
			auto& name = prop_list.at(i).name();
			auto it = setFrank.find(name);
			if (it != setFrank.end())
			{
				vec_key.push_back(name);
				vec_value.push_back(prop_list.at(i).data());
				setFrank.erase(it);
			}

			if (setFrank.empty())
			{
				break;
			}
		}
	}
	
	if (!FRANK_PROP_INT.empty())
	{
		auto setFrank = FRANK_PROP_INT;
		auto& prop_list = xMsg.property_list().property_int();
		for (int i = 0; i < prop_list.size(); ++i)
		{
			auto& name = prop_list.at(i).name();
			auto it = setFrank.find(name);
			if (it != setFrank.end())
			{
				vec_key.push_back(name);
				vec_value.push_back(to_string(prop_list.at(i).data()));
				setFrank.erase(it);
			}

			if (setFrank.empty())
			{
				break;
			}
		}
	}

	if (!FRANK_PROP_DOUBLE.empty())
	{
		auto setFrank = FRANK_PROP_DOUBLE;
		auto& prop_list = xMsg.property_list().property_float();
		for (int i = 0; i < prop_list.size(); ++i)
		{
			auto& name = prop_list.at(i).name();
			auto it = setFrank.find(name);
			if (it != setFrank.end())
			{
				vec_key.push_back(name);
				vec_value.push_back(to_string(prop_list.at(i).data()));
				setFrank.erase(it);
			}

			if (setFrank.empty())
			{
				break;
			}
		}
	}

	if (!FRANK_PROP_OBJECT.empty())
	{
		auto setFrank = FRANK_PROP_OBJECT;
		auto& prop_list = xMsg.property_list().property_object();
		for (int i = 0; i < prop_list.size(); ++i)
		{
			auto& name = prop_list.at(i).name();
			auto it = setFrank.find(name);
			if (it != setFrank.end())
			{
				vec_key.push_back(name);
				vec_value.push_back(PBToNF(prop_list.at(i).data()).ToString());
				setFrank.erase(it);
			}

			if (setFrank.empty())
			{
				break;
			}
		}
	}

	return true;
}