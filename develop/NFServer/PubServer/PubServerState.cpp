///--------------------------------------------------------------------
/// 文件名:		CPubServerState.cpp
/// 内  容:		Pub状态机
/// 说  明:		
/// 创建日期:	2019年9月29日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PubServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "PubKernel.h"
#include "PubServer.h"
#include "NFComm/NFUtils/QuickLog.h"

bool CPubServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pPubKernel = pPluginManager->FindModule<IPubKernel>();
	m_pPubServer = pPluginManager->FindModule<CPubServer>();
	return true;
}

bool CPubServerState::AfterInit()
{
	return true;
}

bool CPubServerState::Execute()
{
	// 定时器1秒刷新
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		PubStateMachine(PUBINPUT_TIMER);
	}

	return true;
}

const char* CPubServerState::GetState() const
{
	switch (m_nPubState)
	{
	case PUBSTATE_UNKNOWN:
		return "Unknown";
	case PUBSTATE_WAITINGOPEND:
		return "WaitingOpen";
	case PUBSTATE_LOADING:
		return "Loading";
	case PUBSTATE_OPENED:
		return "Opened";
	case PUBSTATE_CLOSING:
		return "Closing";
	case PUBSTATE_CLOSED:
		return "Closed";
	default:
		Assert(0);
		break;
	}

	return "";
}

int CPubServerState::PubStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nPubState)
	{
	case PUBSTATE_UNKNOWN:
		return PubStateUnknown(input, msg, nLen);
	case PUBSTATE_WAITINGOPEND:
		return PubStateWaitingOpen(input, msg, nLen);
	case PUBSTATE_LOADING:
		return PubStateLoading(input, msg, nLen);
	case PUBSTATE_OPENED:
		return PubStateOpened(input, msg, nLen);
	case PUBSTATE_CLOSING:
		return PubStateClosing(input, msg, nLen);
	case PUBSTATE_CLOSED:
		return PubStateClosed(input, msg, nLen);
	default:
		Assert(0);
		break;
	}

	return 0;
}

void CPubServerState::SetState(int nState)
{
	m_nPubState = nState;

	m_pPubServer->UpReport();

	QLOG_SYSTEM_S << "PubServerState:" << GetState();

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", GetState()} });
	second_gauge.Increment();
	CATCH_END
}

int CPubServerState::PubStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PUBINPUT_WAITINGOPEN:
		SetState(PUBSTATE_WAITINGOPEND);
		break;
	default:
		break;
	}

	return 1;
}

int CPubServerState::PubStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PUBINPUT_LOADING:
	{
		SetState(PUBSTATE_LOADING);

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

int CPubServerState::PubStateLoading(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PUBINPUT_PUBLOAD:
	{
		bCompletePub = true;
		StateCheckForOpen();
	}
	break;
	case PUBINPUT_GUILDLOAD:
	{
		bCompleteGuild = true;
		StateCheckForOpen();
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPubServerState::PubStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PUBINPUT_CLOSESAVE:
		// 存档Pub数据 后续优化添加存档回应，确保数据存档完成
		dynamic_cast<CPubKernel*>(m_pPubKernel)->SaveAllPubData();

		SetState(PUBSTATE_CLOSING);
		break;
	default:
		break;
	}

	return 1;
}

int CPubServerState::PubStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PUBINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPubServerState::PubStateClosing(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PUBINPUT_TIMER:
	{
		// 检测是否全部存档完成
		if (dynamic_cast<CPubKernel*>(m_pPubKernel)->GetSavePubCount() == 0)
		{
			// 发送回应
			if (m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_CLOSE_PUBSERVER, string()))
			{
				SetState(PUBSTATE_CLOSED);
			}
			else
			{
				//日志添加  数据库关服消息发送失败
				QLOG_ERROR << __FUNCTION__ << " pub close send world failed";
			}
		}
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPubServerState::StateCheckForOpen()
{
	if (bCompleteGuild && bCompletePub)
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

		//通知给Game
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_ACK_LOAD_PUB_COMPLETE, std::string());
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_ACK_LOAD_GUILD_COMPLETE, std::string());

		// 回应world已经open
		if (m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_OPEN_PUBSERVER, std::string()))
		{
			// 进入open状态
			SetState(PUBSTATE_OPENED);

			// 更新report信息
			m_pPubServer->UpReport();

			QuickLog::GetRef().OpenNextSystemEchoGreen();
			QLOG_SYSTEM_S << "Pub Opened";
		}
		else
		{
			QLOG_ERROR << __FUNCTION__ << "pub open send world failed";
		}
	}

	return 0;
}