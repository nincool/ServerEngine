///--------------------------------------------------------------------
/// �ļ���:		PlatformServerState.cpp
/// ��  ��:		ƽ̨��״̬��
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "PlatformServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "PlatformServer.h"
#include "PlatformKernel.h"
#include "NFComm/NFUtils/QuickLog.h"

bool CPlatformServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pPlatformServer = pPluginManager->FindModule<CPlatformServer>();
	m_pPlatformKernel = pPluginManager->FindModule<IPlatformKernel>();

	return true;
}

bool CPlatformServerState::AfterInit()
{
	return true;
}

bool CPlatformServerState::Execute()
{
	// ��ʱ��1��ˢ��
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		PlatformStateMachine(PLATFORMINPUT_TIMER);
	}

	return true;
}

const char* CPlatformServerState::GetState() const
{
	switch (m_nPlatformState)
	{
	case PLATFORMSTATE_UNKNOWN:
		return "Unknown";
	case PLATFORMSTATE_LOADINGDB:
		return "LoadingDB";
	case PLATFORMSTATE_WAITINGOPEN:
		return "WaitingOpen";
	case PLATFORMSTATE_OPENED:
		return "Opened";
	case PLATFORMSTATE_CLOSING:
		return "Closing";
	case PLATFORMSTATE_CLOSED:
		return "Closed";
	default:
		Assert(0);
		break;
	}

	return "";
}

int CPlatformServerState::PlatformStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nPlatformState)
	{
	case PLATFORMSTATE_UNKNOWN:
		return PlatformStateUnknown(input, msg, nLen);
	case PLATFORMSTATE_WAITINGALLNET:
		return PlatformStateWaitingAllNet(input, msg, nLen);
	case PLATFORMSTATE_LOADINGDB:
		return PlatformStateLoadingDB(input, msg, nLen);
	case PLATFORMSTATE_WAITINGOPEN:
		return PlatformStateWaitingOpen(input, msg, nLen);
	case PLATFORMSTATE_OPENED:
		return PlatformStateOpened(input, msg, nLen);
	case PLATFORMSTATE_CLOSING:
		return PlatformStateClosing(input, msg, nLen);
	case PLATFORMSTATE_CLOSED:
		return PlatformStateClosed(input, msg, nLen);
	default:
		Assert(0);
		break;
	}

	return 0;
}

void CPlatformServerState::SetState(int nState)
{
	m_nPlatformState = nState;

	QLOG_SYSTEM_S << "State:" << GetState();
}

int CPlatformServerState::PlatformStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_WAITINGALLNET:
		SetState(PLATFORMSTATE_WAITINGALLNET);
		break;
	default:
		break;
	}

	return 1;
}

int CPlatformServerState::PlatformStateWaitingAllNet(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_ROOMNET:
	{
		// ��������DB����
		if (m_pNetClientModule->SendMsgByType(NF_ST_DB, OuterMsg::SS_REQ_START_LOAD_PLATFORM, std::string()))
		{
			//����ȴ�����״̬
			SetState(PLATFORMSTATE_LOADINGDB);
		}
		else
		{
			QLOG_ERROR << __FUNCTION__ << " send db failed";
		}
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPlatformServerState::PlatformStateLoadingDB(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_WAITINGOPEN:
	{
		// ���͸�room
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_ROOM, OuterMsg::SS_SERVER_COMPLETE_ALLNET, std::string());

		SetState(PLATFORMSTATE_WAITINGOPEN);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPlatformServerState::PlatformStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_ROOMOPEN:
	{
		SetState(PLATFORMSTATE_OPENED);

		// ����report��Ϣ
		m_pPlatformServer->UpReport();

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "PlatformServer Opened";

		// ֪ͨROOMȫ�����
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_ROOM, OuterMsg::SS_OPEN_ALLSERVER, std::string());

		// �����������״̬�ص�
		NF_SHARE_PTR<NFIClass> pPlatformClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
		if (pPlatformClass != nullptr)
		{
			pPlatformClass->RunEventCallBack(EVENT_OnAllServerComplete, NFGUID(), NFGUID(), NFDataList());
		}
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPlatformServerState::PlatformStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_CLOSE:
		SetState(PLATFORMSTATE_CLOSING);
		break;
	default:
		break;
	}

	return 1;
}

int CPlatformServerState::PlatformStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CPlatformServerState::PlatformStateClosing(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case PLATFORMINPUT_TIMER:
	{
		// ֪ͨROOM�ر����л 

		// �浵Platform���� �����Ż���Ӵ浵��Ӧ��ȷ�����ݴ浵���
		dynamic_cast<CPlatformKernel*>(m_pPlatformKernel)->SaveAllPlatformData();

		SetState(PLATFORMSTATE_CLOSED);
	}
	break;
	default:
		break;
	}

	return 1;
}