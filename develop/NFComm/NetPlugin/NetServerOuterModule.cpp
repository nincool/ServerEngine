///--------------------------------------------------------------------
/// �ļ���:		NetServerOuterModule.cpp
/// ��  ��:		���⹫���õ��������ģ��
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "NetServerOuterModule.h"
#include "NFComm/NFUtils/QLOG.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "../NFPluginModule/KConstDefine.h"

NetServerOuterModule::NetServerOuterModule(NFIPluginManager* p)
{
	pPluginManager = p;
	mClassName = "NetServerOuterModule";
}

bool NetServerOuterModule::Init()
{
	mCallBackMap.OpenLog(pPluginManager->FindModule<PerformanceCountModule>(), pPluginManager);

	return true;
}

bool NetServerOuterModule::InitNet(const size_t maxSendBufferSize, const size_t maxReadBufferSize,
	const int nMaxClient, const std::string& ip, const int port)
{
	mCallBackMap.AddEventCallBack(this, (NetEventFun)&NetServerOuterModule::OnSocketEvent);
	mCallBackMap.AddReceiveCallBack(OuterMsg::CS_NET_HEARTBEAT, this, (NetMsgFun)&NetServerOuterModule::OnNetHeartbeat);

	return NetServerModuleBase::Initialization(maxSendBufferSize, maxReadBufferSize, nMaxClient, false, ip, port);
}

bool NetServerOuterModule::Execute()
{
	SafetyCheck();
	return NetServerModuleBase::Execute();
}

void NetServerOuterModule::OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent)
{
	if ((int)eEvent & (int)NET_EVENT::NET_EVENT_CONNECTED)
	{
		mWaitVerifyQueue.push(nSockIndex);
	}
}

void NetServerOuterModule::SafetyCheck()
{
	if (mnVerifyLastTime + 1 > GetPluginManager()->GetNowTime())
	{
		return;
	}
	mnVerifyLastTime = GetPluginManager()->GetNowTime();

	while (!mWaitVerifyQueue.empty())
	{
		NFSOCK nSockIndex = mWaitVerifyQueue.front();
		NetObj* pNetObj = GetNetObj(nSockIndex);
		if (nullptr == pNetObj)
		{
			mWaitVerifyQueue.pop();
			continue;
		}

		if (mnVerifyLastTime - pNetObj->GetLogInTime() < 10)
		{
			//�����г���6�����
			return;
		}

		if (pNetObj->GetConnectKeyState() == 0)
		{
			// �ɼ�TCP�ĳ�ʱ
			if (pPluginManager)
			{
				CATCH_BEGIN
				auto& counter_family = pPluginManager->GetMetricsCounter(4);
				auto& second_counter = counter_family.Add({ {"Type", "Tcp"} });
				second_counter.Increment();
				CATCH_END
			}

			//����6�뻹û��֤���ر�����
			QLOG_WARING << "client time out connect key state. " << pNetObj->GetAccount();
			CloseNetObj(nSockIndex);
		}

		mWaitVerifyQueue.pop();
	}
}

void NetServerOuterModule::OnNetHeartbeat(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	if (m_pNet)
	{
		m_pNet->SendMsg(nMsgID, msg, nLen, nSockIndex);
	}
}
