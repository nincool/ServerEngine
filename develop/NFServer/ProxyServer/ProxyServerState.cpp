///--------------------------------------------------------------------
/// �ļ���:		ProxyServerState.cpp
/// ��  ��:		�����״̬��
/// ˵  ��:		
/// ��������:	2019��9��29��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "ProxyServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "ProxyServerLogicModule.h"
#include "ProxyServer.h"
#include "NFComm/NFUtils/QuickLog.h"

bool CProxyServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pProxyServerLogicModule = pPluginManager->FindModule<CProxyServerLogicModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pProxyServer = pPluginManager->FindModule<CProxyServer>();
	return true;
}

bool CProxyServerState::AfterInit()
{
	return true;
}

bool CProxyServerState::Execute()
{
	// ��ʱ��1��ˢ��
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		ProxyStateMachine(PROXYINPUT_TIMER);
	}

	return true;
}

const char* CProxyServerState::GetState() const
{
	switch (m_nProxyState)
	{
	case PROXYSTATE_UNKNOWN:
		return "Unknown";
	case PROXYSTATE_WAITINGOPEND:
		return "WaitingOpen";
	case PROXYSTATE_PAUSED:
		return "Paused";
	case PROXYSTATE_OPENED:
		return "Opened";
	case PROXYSTATE_CLOSING:
		return "Closing";
	case PROXYSTATE_CLOSED:
		return "Closed";
	default:
		Assert(0);
		break;
	}

	return "";
}

int CProxyServerState::ProxyStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nProxyState)
	{
	case PROXYSTATE_UNKNOWN:
		return ProxyStateUnknown(input, msg, nLen);
	case PROXYSTATE_WAITINGOPEND:
		return ProxyStateWaitingOpen(input, msg, nLen);
	case PROXYSTATE_PAUSED:
		return ProxyStatePaused(input, msg, nLen);
	case PROXYSTATE_OPENED:
		return ProxyStateOpened(input, msg, nLen);
	case PROXYSTATE_CLOSING:
		return ProxyStateClosing(input, msg, nLen);
	case PROXYSTATE_CLOSED:
		return ProxyStateClosed(input, msg, nLen);
	default:
		Assert(0);
		break;
	}
	
	return 0;
}

void CProxyServerState::SetState(int nState)
{
	m_nProxyState = nState;

	m_pProxyServer->UpReport();

	QLOG_SYSTEM_S << "ProxyServerState:" << GetState();

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", GetState()} });
	second_gauge.Increment();
	CATCH_END
}

int CProxyServerState::ProxyStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PROXYINPUT_WAITINGOPEN:
		SetState(PROXYSTATE_WAITINGOPEND);
		break;
	default:
		break;
	}

	return 1;
}

int CProxyServerState::ProxyStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PROXYINPUT_PAUSED:
	{
		SetState(PROXYSTATE_PAUSED);

		m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_OPEN_PROXYSERVER, std::string());
	}
	break;
	default:
		break;
	}

	return 1;
}

int CProxyServerState::ProxyStatePaused(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PROXYINPUT_OPEN:
	{
		SetState(PROXYSTATE_OPENED);

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "Proxy Opened";

		// ����report��Ϣ
		m_pProxyServer->UpReport();
	}
	break;
	default:
		break;
	}

	return 1;
}

int CProxyServerState::ProxyStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PROXYINPUT_CLOSE:
		SetState(PROXYSTATE_CLOSING);
		break;
	case PROXYINPUT_PAUSED:
		SetState(PROXYINPUT_PAUSED);
		break;
	default:
		break;
	}

	return 1;
}

int CProxyServerState::ProxyStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PROXYINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}
	return 1;
}

int CProxyServerState::ProxyStateClosing(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PROXYINPUT_TIMER:
	{
		// �ر����пͻ�������
		m_pProxyServerLogicModule->CloseAllClient();

		// ���ͻ�Ӧ
		if (m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_CLOSE_PROXYSERVER, string()))
		{
			SetState(PROXYSTATE_CLOSED);
		}
		else
		{
			//��־���  ���ݿ�ط���Ϣ����ʧ��
			QLOG_ERROR << __FUNCTION__ << " proxy close send world failed";
		}
	}
	break;
	default:
		break;
	}

	return 1;
}
