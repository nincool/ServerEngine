///--------------------------------------------------------------------
/// 文件名:		WorldServer.cpp
/// 内  容:		世界服
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "WorldServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFUtils/StringUtil.h"
#include "WorldServerState.h"

bool CWorldServer::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pWorldServerState = pPluginManager->FindModule<CWorldServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pCWorldPlayers = pPluginManager->FindModule<CWorldPlayers>();
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	return true;
}

bool CWorldServer::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_NET_COMPLETE_DBSERVER, this, (NetMsgFun)&CWorldServer::OnDBNetComplete);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_NET_COMPLETE_GAMESERVER, this, (NetMsgFun)&CWorldServer::OnGameNetComplete);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_NET_COMPLETE_PROXYSERVER, this, (NetMsgFun)&CWorldServer::OnProxyNetComplete);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_NET_COMPLETE_PUBSERVER, this, (NetMsgFun)&CWorldServer::OnPubNetComplete);

	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_OPEN_DBSERVER, this, (NetMsgFun)&CWorldServer::OnDBServerOpen);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_OPEN_GAMESERVER, this, (NetMsgFun)&CWorldServer::OnGameServerOpen);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_OPEN_PROXYSERVER, this, (NetMsgFun)&CWorldServer::OnProxyServerOpen);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_OPEN_PUBSERVER, this, (NetMsgFun)&CWorldServer::OnPubServerOpen);

	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_CLOSE_PROXYSERVER, this, (NetMsgFun)&CWorldServer::OnProxyServerClose);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_CLOSE_GAMESERVER, this, (NetMsgFun)&CWorldServer::OnGameServerClose);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_CLOSE_PUBSERVER, this, (NetMsgFun)&CWorldServer::OnPubServerClose);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_ACK_CLOSE_DBSERVER, this, (NetMsgFun)&CWorldServer::OnDBServerClose);

	return true;
}

bool CWorldServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CWorldServer>("CWorldServer")
		.addProperty("ClientVersion", &CWorldServer::GetClientVersion, &CWorldServer::SetClientVersion)
		.addProperty("Testing", &CWorldServer::GetWhiteIP, &CWorldServer::SetWhiteIP)
		.addProperty("ServerName", &CWorldServer::GetServerName, &CWorldServer::SetServerName)
		.addProperty("PlayersMax", &CWorldServer::GetPlayersMax, &CWorldServer::SetPlayersMax)
		
		.addFunction("connect_server", &CWorldServer::ConnectServer)
		.addFunction("create_server", &CWorldServer::CreateServer)
		.addFunction("notify_client", &CWorldServer::NotifyClient)
		.addFunction("add_whiteip", &CWorldServer::AddWhiteIPList)
		.addFunction("clear_whiteip", &CWorldServer::ClearWhiteIP)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CWorldServer::InitNetServer()
{
	// 创建服务器网络
	for (int i = 0; i < mxNetList.size(); ++i)
	{
		switch (mxNetList[i].GetNetType())
		{
		case NET_TYPE_INNER:
			bServerNetComplete = m_pNetServerInsideModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
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

	CheckServerStatus();

	return true;
}

bool CWorldServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// 更新report信息
	UpReport();

	return true;
}

bool CWorldServer::Execute()
{
	return true;
}

bool CWorldServer::CommandShut()
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_CLOSE);
	return true;
}

bool CWorldServer::Shut()
{
	delete m_pWhiteIP;
	return true;
}

int CWorldServer::CheckServerStatus()
{
	// world 不关注自己客户端网络是否完成
	if (bServerNetComplete)
	{
		//进入等待开启状态
		m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_WAITINGALLNET);
	}

	return 0;
}

bool CWorldServer::GetServerList(const REQGetServerInfoList& req, ACKGetServerInfoList& ack)
{
	std::vector<const InsideServerData*> mapValue;
	m_pNetServerInsideModule->GetTypeServerAll(req.t, mapValue);
	if (mapValue.empty())
	{
		QLOG_WARING << __FUNCTION__ << " not find server";
		return false;
	}

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
		ack.l.push_back(obj);
	}

	return true;
}

// DBNet完成
void CWorldServer::OnDBNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_DBNET);
}

// GameNet完成
void CWorldServer::OnGameNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_GAMENET);
}

// ProxyNet完成
void CWorldServer::OnProxyNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_PROXYNET);
}

// PubNet完成
void CWorldServer::OnPubNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_PUBNET);
}

// DBServer OPEN
void CWorldServer::OnDBServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	QLOG_INFO_S << "DBServerOpen";
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_DBOPEN);
}

// GameServer OPEN
void CWorldServer::OnGameServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	QLOG_INFO_S << "GameServerOpen";
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_GAMEOPEN);
}

// PubServer OPEN
void CWorldServer::OnPubServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	QLOG_INFO_S << "PubServerOpen";
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_PUBOPEN);
}

// ProxyServer OPEN
void CWorldServer::OnProxyServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	QLOG_INFO_S << "ProxyServerOpen";
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_PROXYOPEN);
}

// ProxyServer Close
void CWorldServer::OnProxyServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_CLOSEPROXY);
}

// GameServer Close
void CWorldServer::OnGameServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_CLOSEGAME);
}

// PubServer Close
void CWorldServer::OnPubServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_CLOSEPUB);
}

// DBServer Close
void CWorldServer::OnDBServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pWorldServerState->WorldStateMachine(CWorldServerState::WORLDINPUT_CLOSEDB);
}

void CWorldServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CWorldServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CWorldServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CWorldServer::GetTypePort(ENUM_NET_TYPE type)
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

void CWorldServer::NotifyClient(const NotifyData& notify_data)
{
	mxNotifyList.push_back(notify_data);
}

bool CWorldServer::AddWhiteIPList(const char* ip_list)
{
	vector<string> ip_vec = StringUtil::SplitString(ip_list, " ");
	if (ip_vec.size() == 2)
	{
		return m_pWhiteIP->AddWhiteIP(ip_vec[0].c_str(), ip_vec[1].c_str());
	}

	return false;
}

bool CWorldServer::ClearWhiteIP()
{
	m_pWhiteIP->ClearWhiteIP();

	return true;
}

bool CWorldServer::IsWhiteIP(unsigned int ip_addr)
{
	if (m_bUseWhiteIP)
	{
		// 白名单
		return m_pWhiteIP->IsWhiteIP(ip_addr);
	}
	
	return false;
}

//更新客户端报文
void CWorldServer::UpReport()
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = GetServerName();

	if (m_pWorldServerState->GetWorldState() == CWorldServerState::WORLDSTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pWorldServerState->GetState());
	xServerInfoReport.set_server_type(pPluginManager->GetServerType());
	xServerInfoReport.set_server_ip(GetTypeIP(NET_TYPE_INNER));
	xServerInfoReport.set_server_port(GetTypePort(NET_TYPE_INNER));
	xServerInfoReport.set_maintain_ip(GetTypeIP(NET_TYPE_MAINTAIN));
	xServerInfoReport.set_maintain_port(GetTypePort(NET_TYPE_MAINTAIN));
	xServerInfoReport.set_server_max_online(GetPlayersMax());
	xServerInfoReport.set_server_cur_count(m_pCWorldPlayers->GetPlayerCount());

	std::string strMsg;
	xServerInfoReport.SerializeToString(&strMsg);
	m_pNetClientModule->SetReport(strMsg);
	m_pNetServerInsideModule->SetReport(strMsg);
}