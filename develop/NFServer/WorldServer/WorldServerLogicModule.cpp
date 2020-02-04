///--------------------------------------------------------------------
/// �ļ���:		WorldServerLogicModule.cpp
/// ��  ��:		world��Ϊ�������߼�ģ��
/// ˵  ��:		
/// ��������:	2019��9��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "WorldServerLogicModule.h"
#include "NFComm/NFMessageDefine/NFDefine.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include <sys/timeb.h>

CWorldServerLogicModule::CWorldServerLogicModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

CWorldServerLogicModule::~CWorldServerLogicModule()
{
}

bool CWorldServerLogicModule::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pWorldPlayers = pPluginManager->FindModule<CWorldPlayers>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CWorldServerLogicModule::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_NET_COMMAND_TRANSMIT, this, (NetMsgFun)&CWorldServerLogicModule::OnCommandTransmit);

	//����ͨ�Ų���
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CWorldServerLogicModule::OnNetTest);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SC_TEST_NET, this, (NetMsgFun)&CWorldServerLogicModule::OnNetTestAck);

	return true;
}

//ȡ������е� ���������
const InsideServerData* CWorldServerLogicModule::GetSuitProxyForEnter()
{
	int nConnectNum = 99999;
	const InsideServerData* pReturnServerData = nullptr;
	std::vector<const InsideServerData*> vecValue;
	if (m_pNetServerInsideModule->GetTypeServer(pPluginManager->GetServerID(), NF_ST_PROXY, vecValue))
	{
		for (int i = 0; i < vecValue.size(); ++i)
		{
			if (vecValue[i]->report.server_cur_count() < nConnectNum)
			{
				nConnectNum = vecValue[i]->report.server_cur_count();
				pReturnServerData = vecValue[i];
			}
		}
	}

	return pReturnServerData;
}

//��Command��Ϣת������
void CWorldServerLogicModule::OnCommandTransmit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::CommandMsg xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " CommandMsg ParseFromArray failed";
		return;
	}

	//ת����GAME
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_GAME, OuterMsg::SS_COMMAND_TOGAME, xMsg);
}

//����ͨ�Ų���
void CWorldServerLogicModule::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	xmsg.set_src_server_sock(nSockIndex);

	struct timeb t1;
	ftime(&t1);
	xmsg.mutable_data()->append("World:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_DB, OuterMsg::CS_TEST_NET, xmsg);
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PUB, OuterMsg::CS_TEST_NET, xmsg);
}

void CWorldServerLogicModule::OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	m_pNetServerInsideModule->SendMsg(OuterMsg::SC_TEST_NET, xmsg, xmsg.src_server_sock());
}