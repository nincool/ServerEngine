///--------------------------------------------------------------------
/// 文件名:		ProxyServer.cpp
/// 内  容:		代理服
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "ProxyServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"

extern void __cdecl lookup_thread(void* lpParameter);

bool CProxyServer::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pProxyServerState = pPluginManager->FindModule<CProxyServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetServerOuterModule = pPluginManager->FindModule<NetServerOuterModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CProxyServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CProxyServer::OnClientNetState));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_SERVER_COMPLETE_ALLNET, this, (NetMsgFun)&CProxyServer::OnServerCompleteAllNet);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_OPEN_ALLSERVER, this, (NetMsgFun)&CProxyServer::OnOpenAllServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_CLOSE_PROXYSERVER, this, (NetMsgFun)&CProxyServer::OnReqCloseProxyServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_SYN_INFO, this, (NetMsgFun)&CProxyServer::OnReqSynInfo);
	return true;
}

bool CProxyServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CProxyServer>("CProxyServer")
		.addProperty("LicAddr", &CProxyServer::GetLicense, &CProxyServer::SetLicense)

		.addFunction("connect_server", &CProxyServer::ConnectServer)
		.addFunction("create_server", &CProxyServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CProxyServer::InitNetServer()
{
	HANDLE handle = (HANDLE)_beginthread(lookup_thread, 0, this);
	if (WaitForSingleObject(handle, 2000) == WAIT_TIMEOUT)
	{
		// 反调试
		TerminateThread(handle, 0);
		printf("(CProxyServer)program abort!");
		abort();
		return false;
	}

	if (m_nLicEndTime > m_nLicStartTime)
	{
		time_t time_value = (time_t)m_nLicEndTime;
		tm* pTm = localtime(&time_value);
		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "license expired time is:" << pTm->tm_year + 1900 << "-" <<
			pTm->tm_mon + 1 << "-" << pTm->tm_mday << " " <<
			pTm->tm_hour << ":" << pTm->tm_min << ":" << pTm->tm_sec;
	}

	// 创建服务器网络
	int nServerNet = 0;
	for (int i = 0; i < mxNetList.size(); ++i)
	{
		switch (mxNetList[i].GetNetType())
		{
		case NET_TYPE_INNER:
			if (m_pNetServerInsideModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort()))
			{
				++nServerNet;
			}
			break;
		case NET_TYPE_OUTER:
			if (m_pNetServerOuterModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetReadBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort()))
			{
				++nServerNet;
			}
			break;
		case NET_TYPE_MAINTAIN:
			break;
		case NET_TYPE_HTTP:
			m_pHttpServerModule->InitServer(mxNetList[i].GetIP().c_str(), mxNetList[i].GetPort());
			break;
		default:
			break;
		}
	}

	if (nServerNet >= 2)
	{
		bServerNetComplete = true;
	}

	return true;
}

bool CProxyServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// 更新report信息
	UpReport();

	return true;
}

bool CProxyServer::Execute()
{
	// 定期下发服务器信息
	if (m_nLastCheckTime + 5 > pPluginManager->GetNowTime())
	{
		return true;
	}
	m_nLastCheckTime = pPluginManager->GetNowTime();

	//检查是否过期
	if (m_nLicVaild)
	{
		if ((m_nLastCheckTime > m_nLicEndTime) ||
			(m_nLastCheckTime < m_nLicStartTime))
		{
			m_nLicVaild = 0;
			m_nLicStartTime = 0;
			m_nLicEndTime = 0;

			QLOG_ERROR_S << "proxy license unmated";
		}
	}
	else
	{
		QLOG_ERROR_S << "proxy license unmated";
	}

	return true;
}

int CProxyServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bClientNetComplete)
	{
		return 0;
	}

	int nState = (int)args.Int(4);
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

int CProxyServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete)
	{
		//进入等待开启状态
		m_pProxyServerState->ProxyStateMachine(CProxyServerState::PROXYINPUT_WAITINGOPEN);
		if (!m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_NET_COMPLETE_PROXYSERVER, std::string()))
		{
			QLOG_ERROR << __FUNCTION__ << " proxy complete send world failed";
		}
	}

	return 0;
}

void CProxyServer::OnReqCloseProxyServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pProxyServerState->ProxyStateMachine(CProxyServerState::PROXYINPUT_CLOSE, msg, nLen);
}

void CProxyServer::OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pProxyServerState->ProxyStateMachine(CProxyServerState::PROXYINPUT_PAUSED, msg, nLen);

	//回调启动完成事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllNetComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CProxyServer::OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// 启动回调事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllServerComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CProxyServer::OnReqSynInfo(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// world 通知可以开启了
	int iValue = lexical_cast<int>(std::string(msg, nLen));
	if (iValue == CProxyServerState::CAN_OPEN)
	{
		m_pProxyServerState->ProxyStateMachine(CProxyServerState::PROXYINPUT_OPEN);
	}
	else
	{
		m_pProxyServerState->ProxyStateMachine(CProxyServerState::PROXYINPUT_PAUSED);
	}
}

void CProxyServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CProxyServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CProxyServer::GetTypeIP(ENUM_NET_TYPE type)
{
	for (auto &it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetIP();
		}
	}

	return NULL_STR;
}

int CProxyServer::GetTypePort(ENUM_NET_TYPE type)
{
	for (auto &it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetPort();
		}
	}

	return 0;
}

//更新程序报文
void CProxyServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	xServerInfoReport.set_server_ip(GetTypeIP(NET_TYPE_OUTER));
	xServerInfoReport.set_server_port(GetTypePort(NET_TYPE_OUTER));
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (m_pProxyServerState->GetProxyState() == CProxyServerState::PROXYSTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pProxyServerState->GetState());
	xServerInfoReport.set_server_type(pPluginManager->GetServerType());
	xServerInfoReport.set_maintain_ip(GetTypeIP(NET_TYPE_MAINTAIN));
	xServerInfoReport.set_maintain_port(GetTypePort(NET_TYPE_MAINTAIN));
	if (GetTypeIP(NET_TYPE_WLAN) != NULL_STR)
	{
		xServerInfoReport.set_wlan_ip(GetTypeIP(NET_TYPE_WLAN));
		xServerInfoReport.set_wlan_port(GetTypePort(NET_TYPE_WLAN));
	}

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