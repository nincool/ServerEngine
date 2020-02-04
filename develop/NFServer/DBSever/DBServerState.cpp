///--------------------------------------------------------------------
/// 文件名:		DBServerState.cpp
/// 内  容:		数据库状态机
/// 说  明:		
/// 创建日期:	2019年9月26日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "DBServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "DBServer.h"
#include "NFComm/NFUtils/QuickLog.h"

bool CDBServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pDBServer = pPluginManager->FindModule<CDBServer>();
	return true;
}

bool CDBServerState::AfterInit()
{
	return true;
}

bool CDBServerState::Execute()
{
	// 定时器1秒刷新
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		DBStateMachine(DBINPUT_TIMER);
	}

	return true;
}

const char* CDBServerState::GetState() const
{
	switch (m_nDBState)
	{
	case DBSTATE_UNKNOWN:
		return "Unknown";
	case DBSTATE_WAITINGOPEND:
		return "WaitingOpen";
	case DBSTATE_OPENED:
		return "Opened";
	case DBSTATE_CLOSING:
		return "Closing";
	case DBSTATE_CLOSED:
		return "Closed";
	default:
		Assert(0);
		break;
	}

	return "";
}

const int CDBServerState::GetStateEnum() const
{
	return m_nDBState;
}

int CDBServerState::DBStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nDBState)
	{
	case DBSTATE_UNKNOWN:
		return DBStateUnknown(input, msg, nLen);
	case DBSTATE_WAITINGOPEND:
		return DBStateWaitingOpen(input, msg, nLen);
	case DBSTATE_OPENED:
		return DBStateOpened(input, msg, nLen);
	case DBSTATE_CLOSING:
		return DBStateClosing(input, msg, nLen);
	case DBSTATE_CLOSED:
		return DBStateClosed(input, msg, nLen);
	default:
		Assert(0);
		break;
	}
	
	return 0;
}

void CDBServerState::SetState(int nState)
{
	m_nDBState = nState;

	m_pDBServer->UpReport();

	QLOG_SYSTEM_S << "DBServerState:" << GetState();

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", GetState()} });
	second_gauge.Increment();
	CATCH_END
}

int CDBServerState::DBStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case DBINPUT_WAITINGOPEN:
		SetState(DBSTATE_WAITINGOPEND);
		break;
	default:
		break;
	}

	return 1;
}

int CDBServerState::DBStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case DBINPUT_OPEN:
	{
		SetState(DBSTATE_OPENED);

		// 更新report信息
		m_pDBServer->UpReport();

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "DBServer Opened";

		// 通知world已经OPEN状态
		m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_OPEN_DBSERVER, std::string());
	}
	break;
	default:
		break;
	}

	return 1;
}

int CDBServerState::DBStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case DBINPUT_CLOSE:
		SetState(DBSTATE_CLOSING);
		break;
	default:
		break;
	}

	return 1;
}

int CDBServerState::DBStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case DBINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}
	return 1;
}

int CDBServerState::DBStateClosing(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case DBINPUT_TIMER:
	{
		// 发送回应
		if (m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_CLOSE_DBSERVER, string()))
		{
			SetState(DBSTATE_CLOSED);
		}
		else
		{
			//日志添加  数据库关服消息发送失败
			QLOG_ERROR << __FUNCTION__ << " DBServer close ack send failed";
		}
	}
	break;
	default:
		break;
	}

	return 1;
}
