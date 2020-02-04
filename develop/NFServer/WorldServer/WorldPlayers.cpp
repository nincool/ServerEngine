///--------------------------------------------------------------------
/// �ļ���:		WorldPlayers.cpp
/// ��  ��:		����������������
/// ˵  ��:		
/// ��������:	2019��10��12��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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
	//������ҽ�ɫ������Ϣ
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_ONLINE_NOTIFY, this, (NetMsgFun)&CWorldPlayers::OnOnlineProcess);
	//������ҽ�ɫ������Ϣ
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_PLAYER_LEAVE_GAME, this, (NetMsgFun)&CWorldPlayers::OnLeaveGame);

	// ������ݱ������
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_PLAYER_SAVED, this, (NetMsgFun)&CWorldPlayers::OnPlayerSaved);
	// �ͻ��˶���
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_USER_BREAK, this, (NetMsgFun)&CWorldPlayers::OnUserBreak);

	return true;
}

bool CWorldPlayers::Execute()
{
	// ��ʱ��1����ˢ��
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

	// ɾ������
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

	// �Ѿ����ڣ����Ӹı䣬��Ҫ�Ͽ���ǰ������
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

// ��ɫ�뿪��Ϸ
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

	// ����ͬһ��������Ϣ��������
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

//��ǰ��������
size_t CWorldPlayers::GetPlayerCount()
{
	return m_mapPlayers.size();
}