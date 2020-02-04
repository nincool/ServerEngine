///--------------------------------------------------------------------
/// �ļ���:		RoomServerState.cpp
/// ��  ��:		�����״̬��
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "RoomServerState.h"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "RoomServer.h"

bool CRoomServerState::Init()
{
	m_nLastTime = pPluginManager->GetNowTime();

	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pRoomServer = pPluginManager->FindModule<CRoomServer>();
	return true;
}

bool CRoomServerState::AfterInit()
{
	return true;
}

bool CRoomServerState::Execute()
{
	// ��ʱ��1��ˢ��
	if (m_nLastTime != pPluginManager->GetNowTime())
	{
		m_nLastTime = pPluginManager->GetNowTime();

		RoomStateMachine(ROOMINPUT_TIMER);
	}

	return true;
}

const char* CRoomServerState::GetState() const
{
	switch (m_nRoomState)
	{
	case ROOMSTATE_UNKNOWN:
		return "Unknown";
	case ROOMSTATE_WAITINGOPEND:
		return "WaitingOpen";
	case ROOMSTATE_OPENED:
		return "Opened";
	case ROOMSTATE_CLOSING:
		return "Closing";
	case ROOMSTATE_CLOSED:
		return "Closed";
	default:
		Assert(0);
		break;
	}

	return "";
}

int CRoomServerState::RoomStateMachine(int input, const char* msg, const uint32_t nLen)
{
	switch (m_nRoomState)
	{
	case ROOMSTATE_UNKNOWN:
		return RoomStateUnknown(input, msg, nLen);
	case ROOMSTATE_WAITINGOPEND:
		return RoomStateWaitingOpen(input, msg, nLen);
	case ROOMSTATE_OPENED:
		return RoomStateOpened(input, msg, nLen);
	case ROOMSTATE_CLOSING:
		return RoomStateClosing(input, msg, nLen);
	case ROOMSTATE_CLOSED:
		return RoomStateClosed(input, msg, nLen);
	default:
		Assert(0);
		break;
	}

	return 0;
}

void CRoomServerState::SetState(int nState)
{
	m_nRoomState = nState;

	m_pRoomServer->UpReport();

	std::ostringstream str;
	str << "state:" << GetState();
	m_pLogModule->LogInfo(str);
}

int CRoomServerState::RoomStateUnknown(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case ROOMINPUT_WAITINGOPEN:
		SetState(ROOMSTATE_WAITINGOPEND);
		break;
	default:
		break;
	}

	return 1;
}

int CRoomServerState::RoomStateWaitingOpen(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case ROOMINPUT_OPEN:
	{
		// ֪ͨPlatform�Ѿ�OPEN
		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_PLATFORM, OuterMsg::SS_OPEN_ROOMSERVER, NULL_STR);

		SetState(ROOMSTATE_OPENED);

		// ����report��Ϣ
		m_pRoomServer->UpReport();

		m_pLogModule->LogInfo("Room Open");

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

int CRoomServerState::RoomStateOpened(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case ROOMINPUT_CLOSE:
		SetState(ROOMSTATE_CLOSING);
		break;
	default:
		break;
	}

	return 1;
}

int CRoomServerState::RoomStateClosed(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case ROOMINPUT_TIMER:
	{
		pPluginManager->SetExit(true);
	}
	break;
	default:
		break;
	}

	return 1;
}

int CRoomServerState::RoomStateClosing(int input, const char* msg, const uint32_t nLen)
{
	switch (input)
	{
	case ROOMINPUT_TIMER:
	{
		// �����ͬ����ƽ̨��

		SetState(ROOMSTATE_CLOSED);
	}
	break;
	default:
		break;
	}

	return 1;
}