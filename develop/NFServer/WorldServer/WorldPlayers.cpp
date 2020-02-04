///--------------------------------------------------------------------
/// 文件名:		WorldPlayers.cpp
/// 内  容:		世界服务器玩管理器
/// 说  明:		
/// 创建日期:	2019年10月12日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "WorldPlayers.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/NFTool.hpp"

bool CWorldPlayers::Init()
{
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pWorldServer = pPluginManager->FindModule<CWorldServer>();

	return true;
}

bool CWorldPlayers::AfterInit()
{
	//监听玩家角色上线消息
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_ONLINE_NOTIFY, this, (NetMsgFun)&CWorldPlayers::OnOnlineProcess);
	//监听玩家角色下线消息
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_PLAYER_LEAVE_GAME, this, (NetMsgFun)&CWorldPlayers::OnLeaveGame);

	// 玩家数据保存完毕
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_PLAYER_SAVED, this, (NetMsgFun)&CWorldPlayers::OnPlayerSaved);
	// 客户端断线
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_USER_BREAK, this, (NetMsgFun)&CWorldPlayers::OnUserBreak);

	return true;
}

bool CWorldPlayers::Execute()
{
	// 定时器1秒钟刷新
	int64_t dnNow = pPluginManager->GetNowTime();
	if (m_dnLastTime == dnNow)
	{
		return true;
	}
	m_dnLastTime = dnNow;

	for (auto& it : m_mapPlayers)
	{
		PlayerStateMachine(it.first, PLAYERINPUT_TIMER, nullptr, 0);
	}

	// 删除数据
	for (auto& it : m_listRemovePlayers)
	{
		m_mapPlayers.erase(it);
	}
	m_listRemovePlayers.clear();

	return true;
}

PlayerData& CWorldPlayers::AddPlayer(const NFGUID& self, const NFGUID& client, int nGameID, int nGateID)
{
	auto& player = m_mapPlayers[self];

	// 已经存在，连接改变，需要断开以前的连接
	if (!player.client.IsNull() && player.client != client)
	{
		BreakClient(player);
	}

	player.self = self;
	player.client = client;
	player.nGameID = nGameID;
	player.nGateID = nGateID;

	return player;
}

bool CWorldPlayers::RemovePlayer(const NFGUID& self)
{
	m_listRemovePlayers.push_back(self);

	return true;
}

void CWorldPlayers::BreakClient(const PlayerData& player)
{
	auto pServerData = m_pNetServerInsideModule->GetSameGroupServer(player.nGateID, NF_ST_PROXY);
	if (pServerData == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find proxy:" << player.nGateID;
		return;
	}

	OuterMsg::GUID xMsg;
	PBConvert::ToPB(player.client, xMsg);
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_REQ_BREAK_CLIENT, xMsg, pServerData->nSockIndex);
}

const PlayerData* CWorldPlayers::GetPlayer(const NFGUID& self)
{
	auto it = m_mapPlayers.find(self);
	if (it == m_mapPlayers.end())
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << self;
		return nullptr;
	}

	return &it->second;
}

void CWorldPlayers::ClearPlayers()
{
	for (auto& it : m_mapPlayers)
	{
		PlayerStateMachine(it.first, PLAYERINPUT_SERVER_CLOSE, nullptr, 0);
	}
}

bool CWorldPlayers::IsFinishClearPlayer()
{
	for (auto& it : m_mapPlayers)
	{
		if (it.second.nState == PLAYERSTATE_SAVING)
		{
			return false;
		}
	}

	return true;
}

void CWorldPlayers::OnOnlineProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::RoleOnlineNotify xMsg;
	xMsg.ParseFromArray(msg, nLen);

	NFGUID self = PBToNF(xMsg.self());
	NFGUID client = PBToNF(xMsg.client());

	auto& player = AddPlayer(self, client, xMsg.game(), xMsg.proxy());
	SetPlayerState(player, PLAYERSTATE_ONLINE);

	m_pWorldServer->UpReport();
}

// 角色离开游戏
void CWorldPlayers::CommandPlayerLeaveGame(const PlayerData& player)
{
	auto pServerData = m_pNetServerInsideModule->GetSameGroupServer(player.nGameID, NF_ST_GAME);
	if (pServerData == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find game:" << player.nGameID;
		return;
	}

	OuterMsg::GUID xMsg;
	PBConvert::ToPB(player.self, xMsg);
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_REQ_PLAYER_REMOVE, xMsg, pServerData->nSockIndex);
}

void CWorldPlayers::CommandPlayerOffline(const PlayerData& player)
{
	auto pServerData = m_pNetServerInsideModule->GetSameGroupServer(player.nGameID, NF_ST_GAME);
	if (pServerData == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find game:" << player.nGameID;
		return;
	}

	OuterMsg::GUID xMsg;
	PBConvert::ToPB(player.self, xMsg);
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_OFFLINE_NOTIFY, xMsg, pServerData->nSockIndex);
}

void CWorldPlayers::OnPlayerSaved(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	NFGUID self;
	PBConvert::ToNF(xMsg, self);

	PlayerStateMachine(self, PLAYERINPUT_STORE_SAVED, msg, nLen);
}

void CWorldPlayers::OnUserBreak(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ClientConnectInfo xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ClientConnectInfo ParseFromArray failed";
		return;
	}

	NFGUID client;
	PBConvert::ToNF(xMsg.client(), client);
	NFGUID player;
	PBConvert::ToNF(xMsg.player(), player);
	
	auto* pPlayer = GetPlayer(player);
	if (pPlayer == nullptr)
	{
		return;
	}

	// 不是同一个连接信息，不处理
	if (pPlayer->client != client)
	{
		return;
	}

	PlayerStateMachine(player, PLAYERINPUT_USER_BREAK, msg, nLen);
}

void CWorldPlayers::OnLeaveGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	NFGUID self;
	PBConvert::ToNF(xMsg, self);

	PlayerStateMachine(self, PLAYERINPUT_LEAVE_GAME, msg, nLen);

	m_pWorldServer->UpReport();
}

//当前在线人数
size_t CWorldPlayers::GetPlayerCount()
{
	return m_mapPlayers.size();
}