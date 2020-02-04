///--------------------------------------------------------------------
/// 文件名:		WorldServerState.cpp
/// 内  容:		世界服状态机
/// 说  明:		
/// 创建日期:	2019年9月29日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "WorldServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "../ProxyServer/ProxyServerState.h"
#include "WorldServer.h"
#include "NFComm/NFUtils/QuickLog.h"

bool CWorldServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pClientNet = pPluginManager->FindModule<NetClientModule>();
	m_pWorldPlayers = pPluginManager->FindModule<CWorldPlayers>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pWorldServer = pPluginManager->FindModule<CWorldServer>();

	return true;
}

bool CWorldServerState::AfterInit()
{
	return true;
}

bool CWorldServerState::Execute()
{
	// 定时器1秒刷新
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		WorldStateMachine(WORLDINPUT_TIMER);
	}

	return true;
}

const char* CWorldServerState::GetState() const
{
	switch (m_nWorldState)
	{
	case WORLDSTATE_UNKNOWN:
		return "Unknown";
	case WORLDSTATE_WAITINGALLNET:
		return "WaitingAllNet";
	case WORLDSTATE_WAITINGOPEN:
		return "WaitingOpen";
	case WORLDSTATE_OPENED:
		return "Opened";
	case WORLDSTATE_CLOSEPROXY:
		return "CloseProxy";
	case WORLDSTATE_CLOSEGAME:
		return "CloseGame";
	case WORLDSTATE_CLOSEPUB:
		return "ClosePub";
	case WORLDSTATE_CLOSEDB:
		return "CloseDB";
	case WORLDSTATE_CLOSED:
		return "Closed";
	case WORLDSTATE_CLEAR_PLAYER:
		return "ClearPlayer";
	default:
		Assert(0);
		break;
	}

	return "";
}

int CWorldServerState::WorldStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nWorldState)
	{
	case WORLDSTATE_UNKNOWN:
		return WorldStateUnknown(input, msg, nLen);
	case WORLDSTATE_WAITINGALLNET:
		return WorldStateWaitingAllNet(input, msg, nLen);
	case WORLDSTATE_WAITINGOPEN:
		return WorldStateWaitingOpen(input, msg, nLen);
	case WORLDSTATE_OPENED:
		return WorldStateOpened(input, msg, nLen);
	case WORLDSTATE_CLOSEPROXY:
		return WorldStateCloseProxy(input, msg, nLen);
	case WORLDSTATE_CLOSEGAME:
		return WorldStateCloseGame(input, msg, nLen);
	case WORLDSTATE_CLOSEPUB:
		return WorldStateClosePub(input, msg, nLen);
	case WORLDSTATE_CLOSEDB:
		return WorldStateCloseDB(input, msg, nLen);
	case WORLDSTATE_CLOSED:
		return WorldStateClosed(input, msg, nLen);
	case WORLDSTATE_CLEAR_PLAYER:
		return WorldStateClearPlayer(input, msg, nLen);
	default:
		Assert(0);
		break;
	}
	
	return 0;
}

void CWorldServerState::SetState(int nState)
{
	m_nWorldState = nState;

	m_pWorldServer->UpReport();

	QLOG_SYSTEM_S << "WorldServerState: " << GetState();

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", GetState()} });
	second_gauge.Increment();
	CATCH_END
}

int CWorldServerState::WorldStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_WAITINGALLNET:
		SetState(WORLDSTATE_WAITINGALLNET);
		break;
	default:
		break;
	}

	return 1;
}

int CWorldServerState::WorldStateWaitingAllNet(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_DBNET:
		bDBNetComplete = true;
		CheckForAllNetComplete();
		break;
	case WORLDINPUT_GAMENET:
		bGameNetComplete = true;
		CheckForAllNetComplete();
		break;
	case WORLDINPUT_PROXYNET:
		bProxyNetComplete = true;
		CheckForAllNetComplete();
		break;
	case WORLDINPUT_PUBNET:
		bPubNetComplete = true;
		CheckForAllNetComplete();
		break;
	default:
		break;
	}

	return 1;
}

int CWorldServerState::WorldStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	// 其他进程关闭重连
	case WORLDINPUT_DBNET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_DB, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_GAMENET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_PUBNET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PUB, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_PROXYNET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;

	case WORLDINPUT_DBOPEN:
		bDBServerOpen = true;
		CheckForAllServerOpen();
		break;
	case WORLDINPUT_GAMEOPEN:
		bGameServerOpen = true;
		CheckForAllServerOpen();
		break;
	case WORLDINPUT_PROXYOPEN:
		bProxyServerOpen = true;
		CheckForAllServerOpen();
		break;
	case WORLDINPUT_PUBOPEN:
		bPubServerOpen = true;
		CheckForAllServerOpen();
		break;
	default:
		break;
	}

	return 1;
}

int CWorldServerState::WorldStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	// 其他进程关闭重连
	case WORLDINPUT_DBNET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_DB, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_GAMENET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_PUBNET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PUB, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_PROXYNET:
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		break;
	case WORLDINPUT_PROXYOPEN:
		// 通知Proxy可以开启
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SS_REQ_SYN_INFO, lexical_cast<string>(CProxyServerState::CAN_OPEN));
		break;

	case WORLDINPUT_CLOSE:
	{
		// 更新report信息
		OuterMsg::ServerInfoReport xServerInfoReport;
		xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
		xServerInfoReport.set_app_id(pPluginManager->GetAppID());
		xServerInfoReport.set_server_id(pPluginManager->GetServerID());
		*(xServerInfoReport.mutable_server_name()) = m_pWorldServer->GetServerName();
		xServerInfoReport.set_server_state(OuterMsg::EST_MAINTEN);
		xServerInfoReport.set_server_type(NF_ST_WORLD);
		xServerInfoReport.set_server_ip(m_pWorldServer->GetTypeIP(NET_TYPE_INNER));
		xServerInfoReport.set_server_port(m_pWorldServer->GetTypePort(NET_TYPE_INNER));
		xServerInfoReport.set_maintain_ip(m_pWorldServer->GetTypeIP(NET_TYPE_MAINTAIN));
		xServerInfoReport.set_maintain_port(m_pWorldServer->GetTypePort(NET_TYPE_MAINTAIN));
		xServerInfoReport.set_server_max_online(m_pWorldServer->GetPlayersMax());
		xServerInfoReport.set_server_cur_count(m_pWorldPlayers->GetPlayerCount());

		std::string strMsg;
		xServerInfoReport.SerializeToString(&strMsg);
		m_pClientNet->SetReport(strMsg);
		m_pClientNet->UpReport(true);

		// 通知Proxy 关闭
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SS_REQ_CLOSE_PROXYSERVER, std::string());
		SetState(WORLDSTATE_CLOSEPROXY);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CWorldServerState::WorldStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CWorldServerState::WorldStateCloseProxy(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_CLOSEPROXY:
	{
		m_pWorldPlayers->ClearPlayers();
		SetState(WORLDSTATE_CLEAR_PLAYER);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CWorldServerState::WorldStateClearPlayer(int input, const char* msg, const uint32_t nLen)
{
	if (input == WORLDINPUT_TIMER)
	{
		if (m_pWorldPlayers->IsFinishClearPlayer())
		{
			// 通知GAMESERVER关闭
			m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_REQ_CLOSE_GAMESERVER, std::string());
			SetState(WORLDSTATE_CLOSEGAME);
		}
	}

	return 1;
}

int CWorldServerState::WorldStateCloseGame(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_CLOSEGAME:
	{
		// 通知PUBSERVER 关闭
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PUB, OuterMsg::SS_REQ_CLOSE_PUBSERVER, std::string());
		SetState(WORLDSTATE_CLOSEPUB);
	}
	break;
	default:
		break;
	}

	return 0;
}

int CWorldServerState::WorldStateClosePub(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_CLOSEPUB:
	{
		// 通知DBSERVER 关闭
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_DB, OuterMsg::SS_REQ_CLOSE_DBSERVER, std::string());
		SetState(WORLDSTATE_CLOSEDB);
	}
	break;
	default:
		break;
	}

	return 0;
}

int CWorldServerState::WorldStateCloseDB(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case WORLDINPUT_CLOSEDB:
	{
		// 所有服已关闭
		SetState(WORLDSTATE_CLOSED);
	}
	break;
	default:
		break;
	}

	return 0;
}

int CWorldServerState::CheckForAllNetComplete()
{
	// 如果所有服的网络完成
	if (bDBNetComplete && bGameNetComplete && bPubNetComplete && bProxyNetComplete)
	{
		// 切换到等待状态
		SetState(CWorldServerState::WORLDSTATE_WAITINGOPEN);

		// 通知所有服务
		for (auto &it : vServerList)
		{
			m_pNetServerInsideModule->SendMsgToTypeServer((NF_SERVER_TYPES)it, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());
		}
	}

	return 0;
}

int CWorldServerState::CheckForAllServerOpen()
{
	// 如果所有服Open
	if (bDBServerOpen && bGameServerOpen && bPubServerOpen && bProxyServerOpen)
	{
		// 切换到等待状态
		SetState(CWorldServerState::WORLDSTATE_OPENED);

		//通知其他服务完成所有服务
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_DB, OuterMsg::SS_OPEN_ALLSERVER, std::string());
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_OPEN_ALLSERVER, std::string());
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PUB, OuterMsg::SS_OPEN_ALLSERVER, std::string());
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SS_OPEN_ALLSERVER, std::string());

		// 更新report信息
		m_pWorldServer->UpReport();

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "WorldServer Opened";

		// 通知Proxy可以开启
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SS_REQ_SYN_INFO, lexical_cast<string>(CProxyServerState::CAN_OPEN));
	}

	return 0;
}