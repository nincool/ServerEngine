///--------------------------------------------------------------------
/// 文件名:		PubServer.cpp
/// 内  容:		公共服
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PubServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include <sys/timeb.h>

bool CPubServer::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pPubServerState = pPluginManager->FindModule<CPubServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	return true;
}

bool CPubServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnAllNetComplete, METHOD_ARGS(CPubServer::OnAllNetComplete));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CPubServer::OnClientNetState));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_SERVER_COMPLETE_ALLNET, this, (NetMsgFun)&CPubServer::OnServerCompleteAllNet);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_CLOSE_PUBSERVER, this, (NetMsgFun)&CPubServer::OnReqClosePubServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_OPEN_ALLSERVER, this, (NetMsgFun)&CPubServer::OnOpenAllServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_PUB_COMPLETE, this, (NetMsgFun)&CPubServer::AckLoadPubComplete);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_GUILD_COMPLETE, this, (NetMsgFun)&CPubServer::AckLoadGuildComplete);

	//网络通信测试
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CPubServer::OnNetTest);

	return true;
}

bool CPubServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CPubServer>("CPubServer")
		.addFunction("connect_server", &CPubServer::ConnectServer)
		.addFunction("create_server", &CPubServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CPubServer::InitNetServer()
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

	return true;
}

bool CPubServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// 更新report信息
	UpReport();

	return true;
}

bool CPubServer::Execute()
{
	return true;
}

int CPubServer::OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	// 启动加载PUB
	m_pNetClientModule->SendMsgByType(NF_ST_DB, OuterMsg::SS_REQ_START_LOAD_PUB, std::string());

	// 启动加载Guild
	m_pNetClientModule->SendMsgByType(NF_ST_DB, OuterMsg::SS_REQ_START_LOAD_GUILD, std::string());

	return 0;
}

void CPubServer::AckLoadPubComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPubServerState->PubStateMachine(CPubServerState::PUBINPUT_PUBLOAD);
}

void CPubServer::AckLoadGuildComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPubServerState->PubStateMachine(CPubServerState::PUBINPUT_GUILDLOAD);
}

int CPubServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bClientNetComplete)
	{
		return 0;
	}

	int nState = args.Int(4);
	if (nState == (int)ConnectState::NORMAL)
	{
		//判定所有强连接是否都正常
		std::vector<const ConnectOBJ*> mConnectVec;
		m_pNetClientModule->GetConnectAll(mConnectVec);
		bool bComplete = true;
		size_t size = mConnectVec.size();
		for (size_t i = 0; i < size; ++i)
		{
			if (!mConnectVec[i]->ConnectCfg.GetIsWeakLink() &&
				mConnectVec[i]->report.server_type() == 0)
			{
				bComplete = false;
				break;
			}
		}

		if (bComplete)
		{
			bClientNetComplete = true;
			CheckServerStatus();
		}
	}
	
	return 0;
}

int CPubServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete)
	{
		//进入等待开启状态
		m_pPubServerState->PubStateMachine(CPubServerState::PUBINPUT_WAITINGOPEN);
		if (!m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_NET_COMPLETE_PUBSERVER, std::string()))
		{
			QLOG_ERROR << __FUNCTION__ << " pub net complete send world failed";
		}
	}

	return 0;
}

void CPubServer::OnReqClosePubServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPubServerState->PubStateMachine(CPubServerState::PUBINPUT_CLOSESAVE, msg, nLen);
}

void CPubServer::OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPubServerState->PubStateMachine(CPubServerState::PUBINPUT_LOADING, msg, nLen);
}

void CPubServer::OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// 启动回调事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllServerComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CPubServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CPubServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CPubServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CPubServer::GetTypePort(ENUM_NET_TYPE type)
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

//更新程序报文
void CPubServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (m_pPubServerState->GetPubState() == CPubServerState::PUBSTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pPubServerState->GetState());
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

//网络通信测试
void CPubServer::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	struct timeb t1;
	ftime(&t1);
	xmsg.mutable_data()->append("Pub:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetClientModule->SendMsgBySock(nSockIndex, OuterMsg::SC_TEST_NET, xmsg);
}