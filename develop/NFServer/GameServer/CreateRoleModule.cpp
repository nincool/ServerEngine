///--------------------------------------------------------------------
/// 文件名:		CreateRoleModule.cpp
/// 内  容:		角色创建
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "CreateRoleModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFPlayer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFUtils/StringUtil.h"

bool CCreateRoleModule::Init()
{
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pKernelModule = dynamic_cast<NFKernelModule*>(pPluginManager->FindModule<NFIKernelModule>());
	m_pGameKernel = dynamic_cast<IGameKernel*>(pPluginManager->FindModule<IGameKernel>());
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pHeartBeatModule = pPluginManager->FindModule<NFIHeartBeatModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pGameServer = pPluginManager->FindModule<CGameServer>();

	assert(m_pKernelModule != nullptr);

	return true;
}

bool CCreateRoleModule::AfterInit()
{
	m_pClassModule->AddEventCallBack(LC::Player::ThisName(), EVENT_OnCreate, METHOD_ARGS(CCreateRoleModule::OnPlayerCreate));

	//请求角色列表
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ROLE_LIST, this, (NetMsgFun)&CCreateRoleModule::OnReqiureRoleListProcess);
	//DB 发送过的角色列表。
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SC_ACK_ROLE_LIST, this, (NetMsgFun)&CCreateRoleModule::OnReposeRoleListProcess);

	// 请求创建角色
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_CREATE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnCreateRoleGameProcess);
	// 预创建角色成功
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_PRECREATE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnAckPreCreateRole);
	//DB 发送过的角色创建回复
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SC_ACK_CREATE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnAckCreateRoleProcess);

	//删除角色
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_DELETE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnDeleteRoleGameProcess);

	//请求进入游戏
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ENTER_GAME, this, (NetMsgFun)&CCreateRoleModule::OnClienEnterGameProcess);
	//DB 加载角色数据返回
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_ROLE_DATA, this, (NetMsgFun)&CCreateRoleModule::OnDBLoadRoleDataProcess);

	//客户端发来进入游戏完成事件
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ENTER_GAME_FINISH, this, (NetMsgFun)&CCreateRoleModule::OnClientEnterGameFinishProcess);

	//删除角色
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_PLAYER_REMOVE, this, (NetMsgFun)&CCreateRoleModule::OnCommandPlayerRemove);

	// 编辑玩家数据
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_ALTER_PLAYER_DATA, this, (NetMsgFun)&CCreateRoleModule::OnAlterPlayer);

	//删除角色
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_ACK_OFFLINE_NOTIFY, this, (NetMsgFun)&CCreateRoleModule::OnCommandPlayerOffline);
	return true;
}

bool CCreateRoleModule::Shut()
{
	return true;
}

bool CCreateRoleModule::Execute()
{
	int64_t dnCurTime = pPluginManager->GetNowTime();
	static int64_t dnLastTime = 0;
	if (dnCurTime < dnLastTime)
	{
		return true;
	}

	dnLastTime = dnCurTime + ONE_MIN_S;

	// 删除超时编辑数据
	if (m_mapAlterPlayer.empty())
	{
		return true;
	}

	std::list<NFGUID> list_del;
	for (auto map_it : m_mapAlterPlayer)
	{
		bool bDel = true;
		for (auto it : map_it.second)
		{
			if (dnCurTime < it.dnDeleteTime)
			{
				bDel = false;
			}
		}

		if (bDel)
		{
			list_del.push_back(map_it.first);
		}
	}

	for (auto it : list_del)
	{
		QLOG_ERROR << __FUNCTION__ << " alterinfo over time player:" << it;
		m_mapAlterPlayer.erase(it);
	}

	return true;
}

//请求角色列表
void CCreateRoleModule::OnReqiureRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, std::string(msg, nLen));
}

//DB 发送过的角色列表。
void CCreateRoleModule::OnReposeRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_PROXY, nMsgID, std::string(msg, nLen));
}

//创建角色回调
void CCreateRoleModule::OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::EGameEventCode eCode = OuterMsg::EGEC_UNKOWN_ERROR;
	OuterMsg::ReqCreateRole xData;
	do
	{
		if (!xData.ParseFromArray(msg, nLen))
		{
			QLOG_WARING << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
			break;
		}

		auto& strName = xData.noob_name();
		if (strName.empty())
		{
			QLOG_WARING << __FUNCTION__ << " name is empty";
			eCode = OuterMsg::EGEC_EXIST_SENSITIVE_WORD;
			break;
		}

		// 敏感词检测
		if (m_pKernelModule->ExistSensitiveWord(strName))
		{
			eCode = OuterMsg::EGEC_EXIST_SENSITIVE_WORD;
			break;
		}

		// 长度检测
		if (StringUtil::UTF8ToString(strName).length() > m_pGameServer->GetRoleNameMaxSize())
		{
			eCode = OuterMsg::EGEC_LENGTH_ERROR;
			break;
		}

		eCode = OuterMsg::EGEC_SUCCESS;
	} while (false);


	if (eCode == OuterMsg::EGEC_SUCCESS)
	{
		// 预创建
		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_PRECREATE_ROLE, std::string(msg, nLen));
	}
	else
	{
		OuterMsg::AckEventResult xAck;
		xAck.set_event_code(eCode);
		std::string strData;
		OuterMsg::MsgBase xMsg;
		xMsg.set_msg_data(xAck.SerializeAsString());
		*xMsg.mutable_player_id() = xData.client_id();
		m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_CREATE_ROLE, xMsg, nSockIndex);
	}
}

// 预创建成功 开始创建角色
void CCreateRoleModule::OnAckPreCreateRole(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::EGameEventCode eCode = OuterMsg::EGEC_UNKOWN_ERROR;
	OuterMsg::ReqCreateRole xData;
	do
	{
		if (!xData.ParseFromArray(msg, nLen))
		{
			QLOG_WARING << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
			break;
		}

		m_pClassModule->CloseEvent();
		NF_SHARE_PTR<NFPlayer> pPlayer =
			dynamic_pointer_cast<NFPlayer>(m_pKernelModule->CreateObject(LC::Player::ThisName(), NULL_STR, NULL_DATA_LIST));
		m_pClassModule->OpenEvent();
		if (pPlayer == nullptr)
		{
			break;
		}

		pPlayer->SetCreateTime(pPluginManager->GetNowTime());
		pPlayer->SetName(xData.noob_name());
		pPlayer->SetAccount(xData.account());
		pPlayer->OnEvent(EVENT_OnCreateRole, NULL_OBJECT, NFDataList() << xData.msg_data());

		if (!GetData(pPlayer->Self(), *xData.mutable_roleData()))
		{
			QLOG_ERROR << __FUNCTION__ << " GetData failed";
			break;
		}

		m_pClassModule->CloseEvent();
		m_pKernelModule->DestroyPlayer(pPlayer->Self());
		m_pClassModule->OpenEvent();

		eCode = OuterMsg::EGEC_SUCCESS;

	} while (false);


	if (eCode == OuterMsg::EGEC_SUCCESS)
	{
		// 正式创建, 由于已经做了预创建, 所以除了网络问题, 应该必然成功
		m_pNetClientModule->SendMsgByType(
			NF_SERVER_TYPES::NF_ST_DB, OuterMsg::CS_REQ_CREATE_ROLE, xData);
	}
	else
	{
		OuterMsg::AckEventResult xAck;
		xAck.set_event_code(eCode);
		std::string strData;
		OuterMsg::MsgBase xMsg;
		*xMsg.mutable_player_id() = xData.client_id();
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SC_ACK_CREATE_ROLE, xMsg);
	}
}

//DB 发送过的角色创建回复
void CCreateRoleModule::OnAckCreateRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID nClientID;
	OuterMsg::AckEventResult xData;
	if (!ReceivePB(nMsgID, msg, nLen, xData, nClientID))
	{
		QLOG_WARING << __FUNCTION__ << " AckEventResult ReceivePB failed";
		return;
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_PROXY, nMsgID, std::string(msg, nLen));
}

//删除角色
void CCreateRoleModule::OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, std::string(msg, nLen));
}

//请求进入游戏
void CCreateRoleModule::OnClienEnterGameProcess(
	const NFSOCK nSockIndex, 
	const int nMsgID, 
	const char* msg, 
	const uint32_t nLen)
{
	NFGUID nClientID;
	OuterMsg::ReqEnterGameServer xMsg;
	if (!ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
	{
		QLOG_WARING << __FUNCTION__ << " ReqEnterGameServer ReceivePB failed";
		return;
	}

	NFGUID nRoleID = PBToNF(xMsg.id());
	if (nRoleID.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " role is null";
		return;
	}

	//取得接入服务器信息
	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (nullptr == pServerData)
	{
		QLOG_ERROR << __FUNCTION__ << " nSockIndex not exist";
		return;
	}

	int nGateID = pServerData->report.app_id();
	if (nGateID < 0)
	{
		QLOG_ERROR << __FUNCTION__ << " nGateID < 0";
		return;
	}

	//已存在 正在取玩家数据 直接更新连接信息
	auto it = m_mapConnInfo.find(nRoleID);
	bool bLoading = it != m_mapConnInfo.end();
	auto& info = m_mapConnInfo[nRoleID];
	info.nGateID = nGateID;
	info.xClientID = nClientID;
	info.strIP = xMsg.ip();
	info.strAccount = xMsg.account();
	info.strDeviceID = xMsg.device_id();
	info.strDeviceMac = xMsg.device_mac();
	info.nOsType = xMsg.os_type();

	// 响应编辑玩家数据回调
	auto itAlert = m_mapAlterPlayer.find(nRoleID);
	if (itAlert != m_mapAlterPlayer.end())
	{
		// 已经请求编辑数据，等待编辑数据返回直接走加载角色逻辑
		return;
	}

	// 编辑玩家可用
	EnableAlterPlayer(nRoleID);

	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(nRoleID));
	if (pPlayer == nullptr)
	{
		// 不存在，加载角色
		if (!bLoading)
		{
			m_pNetClientModule->SendMsgByType(
				NF_SERVER_TYPES::NF_ST_DB,
				OuterMsg::SS_REQ_LOAD_ROLE_DATA,
				std::string(msg, nLen));
		}
		else
		{
			// 正在加载 等待
		}
	}
	else if (pPlayer->GetOnlineState() == OnlineState::ONLINE_ALTER)
	{
		// 编辑状态角色
		// 由于编辑状态的角色没有触发任何事件，所以需要重新加载玩家来触发正常的上线事件
		
		// 获取当前玩家数据
		OuterMsg::ObjectDataPack xRoleDataPack;
		if (!GetData(nRoleID, xRoleDataPack))
		{
			QLOG_ERROR << __FUNCTION__ << " GetData failed";
			return;
		}

		// 结束编辑
		CB_DestroyAlterPlayer(nRoleID, "", 0, 0);

		// 加载玩家
		LoadRole(xRoleDataPack);

	}
	else // 顶号
	{
		// 通知客户端进入游戏
		OnPlayerCreate(nRoleID, NULL_OBJECT, NULL_DATA_LIST);

		// 下发玩家数据
		m_pClassModule->RunCommandCallBack(
			LC::Player::ThisName(), 
			OuterMsg::SS_REQ_UPDATE_PLAYER_DATA, 
			nRoleID, 
			NULL_OBJECT, 
			NULL_DATA_LIST);
	}
}

//DB 加载角色数据返回
void CCreateRoleModule::OnDBLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	// 加载角色
	LoadRole(xMsg);
}

bool CCreateRoleModule::LoadRole(const OuterMsg::ObjectDataPack& xMsg)
{
	NFGUID nRoleID;
	PBConvert::ToNF(xMsg.ident(), nRoleID);
	if (m_pKernelModule->ExistObject(nRoleID))
	{
		QLOG_ERROR << __FUNCTION__ << " already exist role self:" << nRoleID;
		return false;
	}

	NF_SHARE_PTR<NFObject> pObject =
		dynamic_pointer_cast<NFObject>(
			PBConvert::ToNFObject(
				m_pKernelModule,
				NFGUID(),
				&xMsg,
				NULL_DATA_LIST,
				true, false, true));
	if (pObject == nullptr)
	{
		return false;
	}

	const NFGUID& self = pObject->Self();

	//增加定时玩家数据保存心跳
	m_pHeartBeatModule->AddHeartBeat(self, 
		"SaveDataOnTime", 
		this, 
		&CCreateRoleModule::SaveDataOnTime, 
		m_pGameServer->GetSaveDataTime(),
		-1);

	// 进入游戏事件
	pObject->OnEvent(EVENT_OnEntryGame, NFGUID(), NULL_DATA_LIST);

	// 响应玩家编辑
	auto itAlert = m_mapAlterPlayer.find(self);
	if (itAlert != m_mapAlterPlayer.end())
	{
		for (auto& it : itAlert->second)
		{
			it.fp(self, it.args);
		}
		m_mapAlterPlayer.erase(itAlert);
	}

	return true;
}

// 玩家创建
int CCreateRoleModule::OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	auto pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (nullptr == pPlayer)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player self:" << self;
		return 0;
	}

	OuterMsg::EGameEventCode eCode = OuterMsg::EGameEventCode::EGEC_SUCCESS;
	// 玩家连接
	if (!PlayerConnect(self))
	{
		eCode = OuterMsg::EGameEventCode::EGEC_UNKOWN_ERROR;
	}
	else if (pPlayer->GetOpenTime() > pPluginManager->GetNowTime())
	{
		eCode = OuterMsg::EGameEventCode::EGEC_PLAYER_CLOSURE;
	}

	//通知客户端进入游戏
	OuterMsg::AckEventResult xAskMsg;
	xAskMsg.set_event_code(eCode);
	*xAskMsg.mutable_event_client() = NFToPB(pPlayer->GetClientID());
	*xAskMsg.mutable_event_object() = NFToPB(self);
	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_ACK_ENTER_GAME, xAskMsg, self);

	if (eCode != OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		// 通知world断开连接
		OuterMsg::ClientConnectInfo xMsg;
		PBConvert::ToPB(self, *xMsg.mutable_player());
		PBConvert::ToPB(pPlayer->GetClientID(), *xMsg.mutable_client());
		m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_USER_BREAK, xMsg);
	}

	return 0;
}

void CCreateRoleModule::OnClientEnterGameFinishProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID nPlayerID;
	OuterMsg::ReqAckEnterGameSuccess xMsg;
	if (!ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqAckEnterGameSuccess ReceivePB failed";
		return;
	}

	NF_SHARE_PTR<NFPlayer> pPlayerObj 
		= dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(nPlayerID));
	if (pPlayerObj == nullptr)
	{
		return;
	}

	// 设置玩家状态
	pPlayerObj->SetState(COE_CREATE_CLIENT_FINISH);

	pPlayerObj->OnEvent(EVENT_OnReady, NFGUID(), NULL_DATA_LIST);

	//m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_ENTER_GAME_FINISH, std::string(msg, nLen), nSockIndex);
}

bool CCreateRoleModule::GetData(const NFGUID& self, OuterMsg::ObjectDataPack& xRoleDataPack)
{
	return PBConvert::ToPBObject(
		m_pKernelModule,
		self,
		&xRoleDataPack,
		[](NFIProperty& prop)
		{
			return !prop.IsNull() && prop.GetSave();
		},
		[](NFIRecord& record)
		{
			return !record.IsEmpty() && record.GetSave();
		});
}

void CCreateRoleModule::SaveData(const NFGUID& self, bool bExit/*= false*/)
{
	OuterMsg::ObjectDataPack xRoleDataPack;
	if (!GetData(self, xRoleDataPack))
	{
		QLOG_ERROR << __FUNCTION__ << " GetData failed";
		return;
	}
	
	int nMsgID = bExit ? (int)OuterMsg::SS_REQ_SAVE_ROLE_DATA_EXIT : (int)OuterMsg::SS_REQ_SAVE_ROLE_DATA;

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, xRoleDataPack);
}

int CCreateRoleModule::SaveDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// 数据存库事件
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find obj:" << self;
		return 0;
	}

	// 存库事件
	pObject->OnEvent(EVENT_OnStore, NFGUID(), NFDataList() << STORE_TIMING);

	SaveData(self);
	return 0;
}

void CCreateRoleModule::SendOnline(const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer>& pPlayer = 
		dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find obj:" << self;
		return;
	}

	OuterMsg::RoleOnlineNotify xMsg;
	*xMsg.mutable_self() = NFToPB(self);
	xMsg.set_game(pPluginManager->GetAppID());
	xMsg.set_proxy(pPlayer->GetGateID());
	*xMsg.mutable_client() = NFToPB(pPlayer->GetClientID());

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_ACK_ONLINE_NOTIFY, xMsg);
}

bool CCreateRoleModule::PlayerConnect(const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = 
		dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	auto it = m_mapConnInfo.find(pPlayer->Self());
	if (it == m_mapConnInfo.end())
	{
		QLOG_ERROR << __FUNCTION__ << " not find conninfo self:" << pPlayer->Self();
		return false;
	}
	auto& info = it->second;

	// 设置连接数据
	pPlayer->SetGameID(pPluginManager->GetAppID());
	pPlayer->SetGateID(info.nGateID);
	pPlayer->SetOnlineTime(pPluginManager->GetNowTime());
	pPlayer->SetOfflineTime(0);
	pPlayer->SetClientID(info.xClientID);
	pPlayer->SetAccount(info.strAccount);
	pPlayer->SetDeviceID(info.strDeviceID);
	pPlayer->SetDeviceMAC(info.strDeviceMac);
	pPlayer->SetIP(info.strIP);
	pPlayer->SetOsType(info.nOsType);
	pPlayer->SetOnlineState(OnlineState::ONLINE_TRUE);

	// 删除临时数据
	m_mapConnInfo.erase(it);

	//设置玩家上线
	SendOnline(self);

	return true;
}

bool CCreateRoleModule::PlayerDisconnect(const NFGUID& self)
{
	m_mapConnInfo.erase(self);

	NF_SHARE_PTR<NFPlayer> pPlayer =
		dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	// 是否在线
	if (pPlayer->GetOnlineState() != OnlineState::ONLINE_TRUE)
	{
		// 不在线
		return true;
	}

	// 断线事件
	pPlayer->OnEvent(EVENT_OnDisconnect, NULL_OBJECT, NULL_DATA_LIST);

	// 设置连接数据
	auto now = pPluginManager->GetNowTime();
	auto onlineTime = pPlayer->GetOnlineTime();
	pPlayer->SetGateID(0);
	pPlayer->SetOfflineTime(now);
	pPlayer->SetOnlineTime(0);
	pPlayer->SetClientID(NULL_OBJECT);
	if (onlineTime > 0 && now > onlineTime)
	{
		pPlayer->SetTotalSec(pPlayer->GetTotalSec() + now - onlineTime);
	}

	pPlayer->SetOnlineState(OnlineState::ONLINE_FALSE);

	return true;
}

const PlayerConnInfo* CCreateRoleModule::GetConnInfo(const NFGUID& player) const
{
	auto it = m_mapConnInfo.find(player);
	if (it == m_mapConnInfo.end())
	{
		return nullptr;
	}

	return &(it->second);
}

const std::map<NFGUID, PlayerConnInfo>& CCreateRoleModule::GetConnInfoMap() const
{
	return m_mapConnInfo;
}

bool CCreateRoleModule::RemoveConnInfo(const NFGUID& nRoleID)
{
	auto it = m_mapConnInfo.find(nRoleID);
	if (it == m_mapConnInfo.end())
	{
		return false;
	}

	m_mapConnInfo.erase(it);

	return true;
}

void CCreateRoleModule::OnCommandPlayerRemove(
	const NFSOCK nSockIndex, 
	const int nMsgID, 
	const char* msg, 
	const uint32_t nLen)
{
	NFGUID self;
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	PBConvert::ToNF(xMsg, self);

	// 数据存库事件
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return;
	}

	if (pPlayer->GetOnlineState() == OnlineState::ONLINE_TRUE)
	{
		// 玩家在线 中断移除玩家流程
		QLOG_WARING << __FUNCTION__ << " player online cant remove :" << self;
		return;
	}

	// 存库事件
	pPlayer->OnEvent(EVENT_OnStore, NULL_OBJECT, NFDataList() << STORE_EXIT);

	// 尝试执行下线
	PlayerDisconnect(self);

	//销毁玩家-时保存数据到数据库
	SaveData(self, true);

	// 删除角色
	m_pKernelModule->DestroyPlayer(self);

	// 删除连接信息
	RemoveConnInfo(self);
}

void CCreateRoleModule::OnCommandPlayerOffline(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	NFGUID self;
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	PBConvert::ToNF(xMsg, self);

	PlayerDisconnect(self);
}

bool CCreateRoleModule::AlterPlayer(const NFGUID& player, 
	const NFDataList& args, 
	const std::function<void(const NFGUID & self, const NFDataList & args)>& fp)
{
	if (player.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player is null";
		return false;
	}

	if (m_pGameKernel->SeekRoleName(player).empty())
	{
		QLOG_ERROR << __FUNCTION__ << " not player:" << player;
		return false;
	}

	// 使得编辑状态玩家可用
	EnableAlterPlayer(player);

	// 角色已经存在
	NF_SHARE_PTR<NFPlayer> pPlayerObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(player));
	if (pPlayerObj != nullptr)
	{
		// 直接调用回调消息
		fp(player, args);

		// 编辑状态则重置删除心跳
		if (pPlayerObj->GetOnlineState() == OnlineState::ONLINE_ALTER)
		{
			// 删除心跳
			m_pHeartBeatModule->RemoveHeartBeat(player, "CB_DestroyAlterPlayer");
			// 定时删除心跳
			m_pHeartBeatModule->AddHeartBeat(player,
				"CB_DestroyAlterPlayer",
				this,
				&CCreateRoleModule::CB_DestroyAlterPlayer,
				m_pGameServer->GetAlterPlayerLifeTime(),
				1);

			// 使得编辑状态玩家不可用
			DisableAlterPlayer(player);
		}

		return true;
	}

	// 缓存数据
	AlterPlayerInfo info;
	info.fp = fp;
	info.args = args;
	info.dnDeleteTime = pPluginManager->GetNowTime() + ONE_MIN_S;
	m_mapAlterPlayer[player].push_back(info);

	// 角色已经开始加载
	auto* pConnInfo = GetConnInfo(player);
	if (pConnInfo != nullptr)
	{
		return true;
	}

	// 加载玩家数据
	OuterMsg::GUID xMsg;
	PBConvert::ToPB(player, xMsg);

	return m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_ALTER_PLAYER_DATA, xMsg);
}

void CCreateRoleModule::OnAlterPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	NFGUID player;
	PBConvert::ToNF(xMsg.ident(), player);
	if (m_pKernelModule->ExistObject(player))
	{
		// 玩家已经存在
		QLOG_ERROR << __FUNCTION__ << " role already exist:" << player;
		return;
	}

	auto itAlter = m_mapAlterPlayer.find(player);
	if (itAlter == m_mapAlterPlayer.end())
	{
		// 没有此玩家数据
		QLOG_ERROR << __FUNCTION__ << " not find player:" << player;
		return;
	}

	bool bRet = false;
	do
	{
		// 玩家等待登录
		auto* pConnInfo = GetConnInfo(player);
		if (pConnInfo != nullptr)
		{
			// 玩家登录
			if (LoadRole(xMsg))
			{
				return;
			}
			break;
		}

		// 关闭事件
		m_pClassModule->CloseEvent();
		// 创建角色
		NF_SHARE_PTR<NFPlayer> pPlayer =
			dynamic_pointer_cast<NFPlayer>(
				PBConvert::ToNFObject(
					m_pKernelModule,
					NFGUID(),
					&xMsg,
					NULL_DATA_LIST,
					true, false, true));
		// 开启事件
		m_pClassModule->OpenEvent();
		if (pPlayer == nullptr)
		{
			// 玩家创建失败
			QLOG_ERROR << __FUNCTION__ << " create player failed";
			break;
		}

		// 定时保存
		m_pHeartBeatModule->AddHeartBeat(player,
			"CB_AlterPlayerSaveData",
			this,
			&CCreateRoleModule::CB_AlterPlayerSaveData,
			m_pGameServer->GetSaveDataTime(),
			-1);

		// 定时删除心跳
		m_pHeartBeatModule->AddHeartBeat(player, 
			"CB_DestroyAlterPlayer", 
			this, 
			&CCreateRoleModule::CB_DestroyAlterPlayer, 
			m_pGameServer->GetAlterPlayerLifeTime(),
			1);

		// 玩家编辑状态
		pPlayer->SetOnlineState(OnlineState::ONLINE_ALTER);
		
		bRet = true;

	} while (false);

	if (bRet)
	{
		// 响应回调
		for (auto& it : itAlter->second)
		{
			it.fp(player, it.args);
		}

		// 关闭事件
		m_pClassModule->CloseEvent();
		// 保存数据
		SaveData(player);
		// 开启事件
		m_pClassModule->OpenEvent();
		// 使得编辑状态玩家不可用
		DisableAlterPlayer(player);
	}
	else
	{
		// 关闭事件
		m_pClassModule->CloseEvent();
		// 删除对象
		m_pKernelModule->DestroyObjectAny(player);
		// 开启事件
		m_pClassModule->OpenEvent();
	}
	
	// 清理回调
	m_mapAlterPlayer.erase(itAlter);
}

int CCreateRoleModule::CB_AlterPlayerSaveData(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// 编辑玩家可用
	EnableAlterPlayer(self);

	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return 0;
	}

	if (pPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		QLOG_ERROR << __FUNCTION__ << " OnlineState:" << pPlayer->GetOnlineState() << " not OnlineState::ONLINE_ALTER";
		return 0;
	}

	SaveData(self);

	// 编辑玩家不可用
	DisableAlterPlayer(self);

	return 0;
}

int CCreateRoleModule::CB_DestroyAlterPlayer(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// 编辑玩家可用
	EnableAlterPlayer(self);

	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return 0;
	}

	if (pPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		return 0;
	}

	// 关闭事件
	m_pClassModule->CloseEvent();
	// 保存数据
	SaveData(self);
	// 删除对象
	if (!m_pKernelModule->DestroyPlayer(self))
	{
		QLOG_ERROR << __FUNCTION__ << " destroy player failed";
	}
	// 开启事件
	m_pClassModule->OpenEvent();

	return 0;
}

bool CCreateRoleModule::EnableAlterPlayer(const NFGUID& player)
{
	// 使得编辑状态玩家可用
	NF_SHARE_PTR<NFPlayer> pAlterPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetDisableObject(player));
	if (pAlterPlayer == nullptr)
	{
		return false;
	}

	if (pAlterPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		QLOG_ERROR << __FUNCTION__ << " OnlineState:" << pAlterPlayer->GetOnlineState() << " not OnlineState::ONLINE_ALTER";
	}

	return m_pKernelModule->EnableObject(player);
}

bool CCreateRoleModule::DisableAlterPlayer(const NFGUID& player)
{
	// 使得编辑状态玩家可用
	NF_SHARE_PTR<NFPlayer> pAlterPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(player));
	if (pAlterPlayer == nullptr)
	{
		return false;
	}

	if (pAlterPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		QLOG_ERROR << __FUNCTION__ << " OnlineState:" << pAlterPlayer->GetOnlineState() << " not OnlineState::ONLINE_ALTER";
		return false;
	}

	return m_pKernelModule->DisableObject(player);
}