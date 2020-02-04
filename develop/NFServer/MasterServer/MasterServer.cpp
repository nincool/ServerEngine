///--------------------------------------------------------------------
/// �ļ���:		MasterServer.cpp
/// ��  ��:		Master����ģ���߼�
/// ˵  ��:		
/// ��������:	2019��9��7��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "MasterServer.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include <sys/timeb.h>
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"

bool CMasterServer::Init()
{
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	return true;
}

bool CMasterServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CMasterServer>("CLoginServer")
		.addProperty("ModifyFile", &CMasterServer::GetModifyServerFile, &CMasterServer::SetModifyServerFile)
		.addProperty("IsMaster", &CMasterServer::GetIsMaster, &CMasterServer::SetIsMaster)

		.addFunction("connect_server", &CMasterServer::ConnectServer)
		.addFunction("create_server", &CMasterServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CMasterServer::AfterInit()
{
	//��������ѡ���������������
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_CONNECT_WORLD, this, (NetMsgFun)&CMasterServer::OnSelectWorldProcess);
	//����ѡ����������������������
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SC_ACK_CONNECT_WORLD, this, (NetMsgFun)&CMasterServer::OnSelectServerResultProcess);
	//ȡ�÷�������Ϣ�б�
	m_pNetServerMaintainModule->AddReceiveCallBack(OuterMsg::GET_SERVER_LIST, this, (NetMsgFun)&CMasterServer::OnGetServerList);
	//����������(�޸�״̬)
	m_pNetServerMaintainModule->AddReceiveCallBack(OuterMsg::OPT_SET_SERVER_NOTES, this, (NetMsgFun)&CMasterServer::OnOptSetServerNotes);

	//����ͨ�Ų���
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CMasterServer::OnNetTest);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SC_TEST_NET, this, (NetMsgFun)&CMasterServer::OnNetTestAck);
	
	//���������¼�����
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CMasterServer::OnServerStateChange));
	return true;
}

bool CMasterServer::InitNetServer()
{
	// ��������������
	for (int i = 0; i < mxNetList.size(); ++i)
	{
		switch (mxNetList[i].GetNetType())
		{
		case NET_TYPE_INNER:
			m_pNetServerInsideModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_OUTER:
			break;
		case NET_TYPE_MAINTAIN:
			m_pNetServerMaintainModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_HTTP:
			m_pHttpServerModule->InitServer(mxNetList[i].GetIP().c_str(), mxNetList[i].GetPort());
			break;
		default:
			break;
		}
	}

	// ��ȡ������Ϣ
	OnLoadModifyFile();

	return true;
}

bool CMasterServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// ����report��Ϣ
	UpReport();

	QuickLog::GetRef().OpenNextSystemEchoGreen();
	QLOG_SYSTEM_S << "MasterServer Opened";

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", "Opened"} });
	second_gauge.Increment();
	CATCH_END

	return true;
}

bool CMasterServer::Shut()
{
	return true;
}

bool CMasterServer::Execute()
{
	// �����·���������Ϣ
	if (m_nCheckTime + 5 > pPluginManager->GetNowTime())
	{
		return true;
	}
	m_nCheckTime = pPluginManager->GetNowTime();

	OnWorldInfoToLogin();

	return true;
}

// �����������
int CMasterServer::OnServerStateChange(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	// ���ڵ�Żᴦ��ַ�
	if (!m_bMaster)
	{
		return 0;
	}

	// �ж��Ƿ������ͽ��յ�
	int mServer = args.Int(7);
	if (mServer == 0)
	{
		//�����Master������
		int nServerState = args.Int(0);
		int nServerID = args.Int(1);
		int nAppID = args.Int(2);
		int nServerType = args.Int(3);
		int nSockID = args.Int(6);
		if (nServerType == NF_ST_MASTER)
		{
			//�ж��Ƿ�Ͽ�����
			int nNetState = args.Int(4);
			if (nNetState == (int)ConnectState::NORMAL && nServerState == OuterMsg::EST_NARMAL)
			{
				// �������е�master����
				std::vector<const InsideServerData*> vec_Master;
				if (m_pNetServerInsideModule->GetTypeServerAll(NF_ST_MASTER, vec_Master))
				{
					std::vector<const InsideServerData*> vec_World;
					if (m_pNetServerInsideModule->GetTypeServerAll(NF_ST_WORLD, vec_World))
					{
						// �����������ӷ���
						OuterMsg::ServerConnectList xServerConnectList;
						xServerConnectList.set_OptType(OuterMsg::ServerConnectList::OPT_ADD);
						for (auto& master_it : vec_Master)
						{
							OuterMsg::ServerConnectInfo* pServerConnectInfo = xServerConnectList.add_data();
							pServerConnectInfo->set_ip(master_it->report.server_ip());
							pServerConnectInfo->set_port(master_it->report.server_port());
							pServerConnectInfo->set_type(master_it->report.server_type());
						}

						for (auto& it : vec_World)
						{
							m_pNetServerInsideModule->SendMsg(OuterMsg::SS_REQ_SYN_INFO, xServerConnectList, it->nSockIndex);
						}
					}
				}
			}
			else if (nNetState == (int)ConnectState::DISCONNECT && nServerState == OuterMsg::EST_MAINTEN)
			{
				//����master������Ϣ
				std::vector<const InsideServerData*> vec_World;
				if (m_pNetServerInsideModule->GetTypeServerAll(NF_ST_WORLD, vec_World))
				{
					OuterMsg::ServerConnectList xServerConnectList;
					xServerConnectList.set_OptType(OuterMsg::ServerConnectList::OPT_REMOVE);
					OuterMsg::ServerConnectInfo* pServerConnectInfo = xServerConnectList.add_data();
					const InsideServerData* xInsideServerData = m_pNetServerInsideModule->GetSockIDServer(nSockID);
					if (xInsideServerData != nullptr && pServerConnectInfo != nullptr)
					{
						pServerConnectInfo->set_ip(xInsideServerData->report.server_ip());
						pServerConnectInfo->set_port(xInsideServerData->report.server_port());
						pServerConnectInfo->set_type(xInsideServerData->report.server_type());
					}

					for (auto& it : vec_World)
					{
						m_pNetServerInsideModule->SendMsg(OuterMsg::SS_REQ_SYN_INFO, xServerConnectList, it->nSockIndex);
					}
				}
			}
		}
		else if (nServerType == NF_ST_WORLD && nServerState == OuterMsg::EST_NARMAL)
		{
			//����master������Ϣ
			std::vector<const InsideServerData*> vec_Master;
			if (m_pNetServerInsideModule->GetTypeServerAll(NF_ST_MASTER, vec_Master))
			{
				OuterMsg::ServerConnectList xServerConnectList;
				xServerConnectList.set_OptType(OuterMsg::ServerConnectList::OPT_ADD);
				for (auto& master_it : vec_Master)
				{
					OuterMsg::ServerConnectInfo* pServerConnectInfo = xServerConnectList.add_data();
					pServerConnectInfo->set_ip(master_it->report.server_ip());
					pServerConnectInfo->set_port(master_it->report.server_port());
					pServerConnectInfo->set_type(master_it->report.server_type());
				}
				m_pNetServerInsideModule->SendMsg(OuterMsg::SS_REQ_SYN_INFO, xServerConnectList, nSockID);
			}
		}
	}

	return 0;
}

// ��ȡ��ע�ļ�
void CMasterServer::OnLoadModifyFile()
{
	mapModifyInfo.clear();
	CIniFile ini(m_nModifyServerFile.c_str());
	if (ini.LoadFromFile())
	{
		int nSecIndex = ini.FindSection("ServerNotes");
		if (nSecIndex >= 0)
		{
			int nItemCount = ini.GetSectItemCount(nSecIndex);
			const char* strKey;
			const char* strValue;
			for (int i = 0; i < nItemCount; ++i)
			{
				if (ini.GetSectItemData(nSecIndex, i, strKey, strValue))
				{
					if (isnum(strKey))
					{
						mapModifyInfo.insert(make_pair(lexical_cast<int>(strKey), strValue));
					}
				}
			}
		}
	}
}

bool CMasterServer::GetServerList(const REQGetServerInfoList& req, ACKGetServerInfoList& ack)
{
	mapValue.clear();
	if (m_pNetServerInsideModule->GetTypeServerAll(NF_ST_WORLD, mapValue))
	{
		for (int i = 0; i < mapValue.size(); ++i)
		{
			ACKGetServerInfo obj;
			obj.s_id = mapValue[i]->report.server_id();
			obj.s_name = mapValue[i]->report.server_name();
			obj.s_ip = mapValue[i]->report.server_ip();
			obj.s_port = mapValue[i]->report.server_port();
			obj.m_ip = mapValue[i]->report.maintain_ip();
			obj.m_port = mapValue[i]->report.maintain_port();
			obj.max_ol = mapValue[i]->report.server_max_online();
			obj.cur_c = mapValue[i]->report.server_cur_count();
			obj.state = mapValue[i]->report.server_state();
			obj.type = mapValue[i]->report.server_type();
			obj.app_id = mapValue[i]->report.app_id();
			obj.dist_id = mapValue[i]->report.district_id();
			obj.notes = mapValue[i]->report.notes();
			//���ӱ�ע��Ϣ
			std::map<int, std::string>::iterator itFind = mapModifyInfo.find(mapValue[i]->report.server_id());
			if (itFind != mapModifyInfo.end())
			{
				obj.notes = itFind->second;
			}

			ack.l.push_back(obj);
		}
	}

	return true;
}

void CMasterServer::OnOptSetServerNotes(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	REQOptServerNotes req;
	ajson::load_from_buff(req, msg, nLen);

	bool bRtn = OnModifyWorldServer(lexical_cast<int>(req.sid), req.notes);

	ACKOptServerNotes ack;
	ack.result = bRtn ? 1 : 0;

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);

	m_pNetServerMaintainModule->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

// world��Ϣ���¸�login
void CMasterServer::OnWorldInfoToLogin()
{
	mapValue.clear();
	if (m_pNetServerInsideModule->GetTypeServerAll(NF_ST_WORLD, mapValue))
	{
		// �������world��Ϣ
		OuterMsg::ServerInfoReportList xData;
		for (int i = 0; i < mapValue.size(); ++i)
		{
			(*xData.mutable_server_list())[mapValue[i]->report.server_id()] = mapValue[i]->report;
			//���ӱ�ע��Ϣ
			std::map<int, std::string>::iterator itFind = mapModifyInfo.find(mapValue[i]->report.server_id());
			if (itFind != mapModifyInfo.end())
			{
				(*xData.mutable_server_list())[mapValue[i]->report.server_id()].set_notes(itFind->second.c_str());
			}
		}
		
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_LOGIN, OuterMsg::SS_STS_NET_INFO, xData);
		mapValue.clear();
	}
}

// ����עĳ��������
bool CMasterServer::OnModifyWorldServer(int server_id, std::string& value)
{
	if (m_nModifyServerFile.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " modifyfile is empty";
		return false;
	}

	//���ش洢��Ϣ
	CIniFile ini(m_nModifyServerFile.c_str());
	ini.LoadFromFile();
	//�޸��ڴ���Ϣ
	std::map<int, std::string>::iterator itFind = mapModifyInfo.find(server_id);
	if (itFind != mapModifyInfo.end())
	{
		itFind->second = value;
	}
	else
	{
		mapModifyInfo.insert(make_pair(server_id, value));
	}

	ini.WriteString("ServerNotes", lexical_cast<std::string>(server_id).c_str(), value.c_str());
	ini.SaveToFile();

	return true;
}

// ȡ�÷�������Ϣ�б�
void CMasterServer::OnGetServerList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	REQGetServerInfoList req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetServerInfoList ack;
	ack.u = req.u;

	GetServerList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);

	m_pNetServerMaintainModule->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//����ѡ���������������
void CMasterServer::OnSelectWorldProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqConnectWorld xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqConnectWorld ParseFromArray failed";
		return;
	}

	std::vector<const InsideServerData*> vec;
	m_pNetServerInsideModule->GetTypeServer(xMsg.world_id(), NF_ST_WORLD, vec);
	if (vec.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " not find world server";
		return;
	}

	//�Զ�ȡ��һ�������
	m_pNetServerInsideModule->SendMsg(OuterMsg::CS_REQ_CONNECT_WORLD, xMsg, vec[0]->nSockIndex);
}

//ѡ����������������������
void CMasterServer::OnSelectServerResultProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckConnectWorldResult xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckConnectWorldResult ParseFromArray failed";
		return;
	}

	const InsideServerData* pInsideServerData = m_pNetServerInsideModule->GetSameGroupServer(xMsg.login_id(), NF_ST_LOGIN);
	if (nullptr == pInsideServerData)
	{
		QLOG_ERROR << __FUNCTION__ << " not find same login server";
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_CONNECT_WORLD, xMsg, pInsideServerData->nSockIndex);
}

void CMasterServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CMasterServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CMasterServer::GetTypeIP(ENUM_NET_TYPE type)
{
	for (auto& it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetIP();
		}
	}

	return NULL_STR;
}

int CMasterServer::GetTypePort(ENUM_NET_TYPE type)
{
	for (auto& it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetPort();
		}
	}

	return 0;
}

//���³�����
void CMasterServer::UpReport(bool force/* = false*/)
{
	// ����report��Ϣ
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	xServerInfoReport.set_state_info("Opened");

	xServerInfoReport.set_server_type(pPluginManager->GetServerType());
	xServerInfoReport.set_server_ip(GetTypeIP(NET_TYPE_INNER));
	xServerInfoReport.set_server_port(GetTypePort(NET_TYPE_INNER));
	xServerInfoReport.set_maintain_ip(GetTypeIP(NET_TYPE_MAINTAIN));
	xServerInfoReport.set_maintain_port(GetTypePort(NET_TYPE_MAINTAIN));

	std::string strMsg;
	xServerInfoReport.SerializeToString(&strMsg);
	m_pNetClientModule->SetReport(strMsg);
	m_pNetServerInsideModule->SetReport(strMsg);

	if (force)
	{
		m_pNetClientModule->UpReport(force);
		m_pNetServerInsideModule->UpReport(force);
	}
}

//����ͨ�Ų���
void CMasterServer::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	//��¼
	xmsg.set_src_server_sock(nSockIndex);

	struct timeb t1;
	ftime(&t1);
	xmsg.mutable_data()->append("Master:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	std::vector<const InsideServerData*> vec;
	m_pNetServerInsideModule->GetTypeServer(xmsg.word_server_id(), NF_ST_WORLD, vec);
	if (vec.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " not find world server";
		return;
	}
	m_pNetServerInsideModule->SendMsg(OuterMsg::CS_TEST_NET, xmsg, vec[0]->nSockIndex);
}
//����ͨ�Ų���
void CMasterServer::OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	m_pNetServerInsideModule->SendMsg(OuterMsg::SC_TEST_NET, xmsg, xmsg.src_server_sock());
}