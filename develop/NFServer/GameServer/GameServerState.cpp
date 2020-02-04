///--------------------------------------------------------------------
/// 文件名:		CGameServerState.cpp
/// 内  容:		游戏状态机
/// 说  明:		
/// 创建日期:	2019年9月26日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "GameServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "GameServer.h"
#include "NFComm/NFUtils/QuickLog.h"

bool CGameServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pGameServer = pPluginManager->FindModule<CGameServer>();
	return true;
}

bool CGameServerState::AfterInit()
{
	return true;
}

bool CGameServerState::Execute()
{
	// 定时器1秒刷新
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		GameStateMachine(GAMEINPUT_TIMER);
	}

	return true;
}

const char* CGameServerState::GetState() const
{
	switch (m_nGameState)
	{
	case GAMESTATE_UNKNOWN:
		return "Unknown";
	case GAMESTATE_WAITINGOPEND:
		return "WaitingOpen";
	case GAMESTATE_LOADING:
		return "Loading";
	case GAMESTATE_OPENED:
		return "Opened";
	case GAMESTATE_CLOSING:
		return "Closing";
	case GAMESTATE_CLOSED:
		return "Closed";
	default:
		Assert(0);
		break;
	}

	return "";
}

int CGameServerState::GameStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nGameState)
	{
	case GAMESTATE_UNKNOWN:
		return GameStateUnknown(input, msg, nLen);
	case GAMESTATE_WAITINGOPEND:
		return GameStateWaitingOpen(input, msg, nLen);
	case GAMESTATE_LOADING:
		return GameStateLoading(input, msg, nLen);
	case GAMESTATE_OPENED:
		return GameStateOpened(input, msg, nLen);
	case GAMESTATE_CLOSING:
		return GameStateClosing(input, msg, nLen);
	case GAMESTATE_CLOSED:
		return GameStateClosed(input, msg, nLen);
	default:
		Assert(0);
		break;
	}

	return 0;
}

void CGameServerState::SetState(int nState)
{
	m_nGameState = nState;

	m_pGameServer->UpReport();

	QLOG_SYSTEM_S << "GameServerState:" << GetState();

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", GetState()} });
	second_gauge.Increment();
	CATCH_END
}

int CGameServerState::GameStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case GAMEINPUT_WAITINGOPEN:
		SetState(GAMESTATE_WAITINGOPEND);
		break;
	default:
		break;
	}

	return 1;
}

int CGameServerState::GameStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case GAMEINPUT_LOADING:
	{
		SetState(GAMESTATE_LOADING);

		NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
		if (pServerClass != nullptr)
		{
			pServerClass->RunEventCallBack(EVENT_OnAllNetComplete, NFGUID(), NFGUID(), NFDataList());
		}
	}
	break;
	default:
		break;
	}

	return 1;
}

int CGameServerState::GameStateLoading(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case GAMEINPUT_PUBLOAD:
	{
		bCompletePub = true;
		StateCheckForOpen();
	}
	break;
	case GAMEINPUT_GUILDLOAD:
	{
		bCompleteGuild = true;
		StateCheckForOpen();
	}
	break;
	case GAMEINPUT_GUIDNAMELOAD:
	{
		bCompleteGuidName = true;
		StateCheckForOpen();
	}
	break;
	default:
		break;
	}

	return 1;
}

int CGameServerState::GameStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case GAMEINPUT_CLOSE:
		SetState(GAMESTATE_CLOSING);
		break;
	default:
		break;
	}

	return 1;
}

int CGameServerState::GameStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case GAMEINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CGameServerState::GameStateClosing(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case GAMEINPUT_TIMER:
	{
		// 发送回应
		if (m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_CLOSE_GAMESERVER, string()))
		{
			SetState(GAMESTATE_CLOSED);
		}
		else
		{
			//日志添加  数据库关服消息发送失败
			QLOG_ERROR << "game close send world failed";
		}
	}
	break;
	default:
		break;
	}

	return 1;
}

int CGameServerState::StateCheckForOpen()
{
	if (bCompleteGuild && bCompletePub && bCompleteGuidName)
	{
		NF_SHARE_PTR<NFIClass> pPubClass = m_pClassModule->GetElement(LC::Pub::ThisName());
		if (pPubClass != nullptr)
		{
			pPubClass->RunEventCallBack(EVENT_OnPubComplete, NFGUID(), NFGUID(), NFDataList());
		}

		NF_SHARE_PTR<NFIClass> pGuildClass = m_pClassModule->GetElement(LC::Guild::ThisName());
		if (pGuildClass != nullptr)
		{
			pGuildClass->RunEventCallBack(EVENT_OnGuildComplete, NFGUID(), NFGUID(), NFDataList());
		}

		// 回应world已经open
		if (m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_OPEN_GAMESERVER, std::string()))
		{
			// 进入open状态
			SetState(GAMESTATE_OPENED);

			// 更新report信息
			m_pGameServer->UpReport();

			QuickLog::GetRef().OpenNextSystemEchoGreen();
			QLOG_SYSTEM_S << "Game Opened";
		}
		else
		{
			QLOG_ERROR << __FUNCTION__ << "game close send world failed";
		}
	}

	return 0;
}