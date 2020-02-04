///--------------------------------------------------------------------
/// �ļ���:		WorldClientLogicModule.cpp
/// ��  ��:		World��Ϊ�ͻ����߼�ģ��
/// ˵  ��:		
/// ��������:	2019��9��7��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "WorldClientLogicModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "WorldServer.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFUtils/StringUtil.h"
#include "WorldServerState.h"
#include <sys/timeb.h>
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"

bool CWorldClientLogicModule::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pSecurityModule = pPluginManager->FindModule<NFISecurityModule>();
	m_pWorldServerLogicModule = pPluginManager->FindModule<CWorldServerLogicModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pWorldServer = pPluginManager->FindModule<CWorldServer>();
	m_pWorldServerState = pPluginManager->FindModule<CWorldServerState>();

	return true;
}

bool CWorldClientLogicModule::Shut()
{
	return true;
}

bool CWorldClientLogicModule::Execute()
{
	return true;
}

bool CWorldClientLogicModule::AfterInit()
{
	//����Master���������͹����Ŀͻ���ѡ�������
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::CS_REQ_CONNECT_WORLD, this, (NetMsgFun)&CWorldClientLogicModule::OnSelectServerProcess);
	// masterҪ��ط�
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::SS_REQ_CLOSE_SERVER, this, (NetMsgFun)&CWorldClientLogicModule::OnCloseServer);
	//����ͨ�Ų���
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CWorldClientLogicModule::OnNetTest);
	//���Ӵ�Master������
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::SS_REQ_SYN_INFO, this, (NetMsgFun)&CWorldClientLogicModule::OnConnectMaster);

	// ͳ�Ƹ��ӷ�����������Ϣ
	ExtraConnectServerInfo();

	return true;
}

// �رշ�����
void CWorldClientLogicModule::OnCloseServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	//����ط�����
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_CLOSE);
}

//����Master���������͹����Ŀͻ���ѡ��������ص�
void CWorldClientLogicModule::OnSelectServerProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqConnectWorld xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqConnectWorld ParseFromArray failed";
		return;
	}

	// �汾�Ź���
	if (xMsg.clientVersion() < m_pWorldServer->GetClientVersion())
	{
		return;
	}

	// ����IP
	std::string sLoginIP = xMsg.client_ip();
	unsigned int nLoginIP = convert_ip(sLoginIP.c_str());
	if (m_pWorldServer->GetWhiteIP() && !m_pWorldServer->IsWhiteIP(nLoginIP))
	{
		return;
	}

	//ȡ��һ���������� ���������
	const InsideServerData* xServerData = m_pWorldServerLogicModule->GetSuitProxyForEnter();
	OuterMsg::AckConnectWorldResult xData;
	if (xServerData)
	{
		const std::string strSecurityKey = m_pSecurityModule->GetSecurityKey(xMsg.account());

		xData.set_world_id(xMsg.world_id());
		xData.mutable_sender()->CopyFrom(xMsg.sender());
		xData.set_login_id(xMsg.login_id());
		xData.set_account(xMsg.account());

		std::string strProxyIP = "";
		int nProxyPort = 0;
		if (!xServerData->report.wlan_ip().empty())
		{
			strProxyIP = xServerData->report.wlan_ip();
			nProxyPort = xServerData->report.wlan_port();
		}
		else
		{
			strProxyIP = xServerData->report.server_ip();
			nProxyPort = xServerData->report.server_port();
		}

		xData.set_world_ip(strProxyIP);
		xData.set_world_port(nProxyPort);
		xData.set_world_key(strSecurityKey);
		xData.mutable_world_param()->CopyFrom(m_xWorldparam);

		//���͸����������
		m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_CONNECT_WORLD, xData, xServerData->nSockIndex);

		//���͸� ��������
		m_pNetClientModule->SendMsgByType(NF_ST_MASTER, OuterMsg::SC_ACK_CONNECT_WORLD, xData);
	}
	else
	{
		QLOG_ERROR << __FUNCTION__ << " None Suit Proxy";
	}
}

void CWorldClientLogicModule::OnConnectMaster(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ServerConnectList xServerConnectList;
	if (!xServerConnectList.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ServerConnectList ParseFromArray failed";
		return;
	}

	int nOptType = xServerConnectList.OptType();
	for (int i = 0; i < xServerConnectList.data_size(); ++i)
	{
		ConnectCfg xConnectCfg;
		std::string ip = xServerConnectList.data(i).ip();
		xConnectCfg.SetIP(ip);
		xConnectCfg.SetPort(xServerConnectList.data(i).port());
		xConnectCfg.SetServerType((NF_SERVER_TYPES)xServerConnectList.data(i).type());
		if (nOptType == OuterMsg::ServerConnectList::OPT_ADD)
		{
			m_pNetClientModule->AddServerConnect(xConnectCfg);
		}
		else if (nOptType == OuterMsg::ServerConnectList::OPT_REMOVE)
		{
			m_pNetClientModule->RemoveServerConnect(xConnectCfg);
		}
	}
}

void CWorldClientLogicModule::ExtraConnectServerInfo()
{
	vector<NotifyData> vec_Notify = m_pWorldServer->GetNotifyList();
	for (auto& it : vec_Notify)
	{
		auto& info = (*m_xWorldparam.mutable_extra_server())[it.GetName()];
		info.set_id(it.GetName());
		info.set_ip(it.GetIP());
		info.set_port(it.GetPort());
		info.set_webport(it.GetWebPort());
	}
}

//����ͨ�Ų���
void CWorldClientLogicModule::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	//��¼
	struct timeb t1;
	ftime(&t1);

	xmsg.mutable_data()->append("World:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetClientModule->SendMsgBySock(nSockIndex, OuterMsg::SC_TEST_NET, xmsg);
}