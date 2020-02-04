///--------------------------------------------------------------------
/// 文件名:		PlatformServer.cpp
/// 内  容:		平台服
/// 说  明:		
/// 创建日期:	2019年10月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PlatformServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/LuaExt.hpp"

bool CPlatformServer::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pPlatformServerState = pPluginManager->FindModule<CPlatformServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();

	return true;
}

bool CPlatformServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CPlatformServer>("CPlatformServer")
		.addFunction("connect_server", &CPlatformServer::ConnectServer)
		.addFunction("create_server", &CPlatformServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CPlatformServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CPlatformServer::OnClientNetState));

	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_NET_COMPLETE_ROOMSERVER, this, (NetMsgFun)&CPlatformServer::OnRoomServerNetComplete);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_OPEN_ROOMSERVER, this, (NetMsgFun)&CPlatformServer::OnRoomServerOpen);

	//world command事件
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_COMMAND_TOPLATFORM, this, (NetMsgFun)&CPlatformServer::OnCommandToPlatform);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_ROOM, OuterMsg::SS_COMMAND_TOPLATFORM, this, (NetMsgFun)&CPlatformServer::OnCommandToPlatform);
	return true;
}

bool CPlatformServer::InitNetServer()
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

bool CPlatformServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// 更新report信息
	UpReport();

	return true;
}

bool CPlatformServer::Execute()
{
	return true;
}

int CPlatformServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
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

int CPlatformServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete)
	{
		//进入等待开启状态
		m_pPlatformServerState->PlatformStateMachine(CPlatformServerState::PLATFORMINPUT_WAITINGALLNET);
	}

	return 0;
}

void CPlatformServer::OnRoomServerNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPlatformServerState->PlatformStateMachine(CPlatformServerState::PLATFORMINPUT_ROOMNET, msg, nLen);
}

void CPlatformServer::OnRoomServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPlatformServerState->PlatformStateMachine(CPlatformServerState::PLATFORMINPUT_ROOMOPEN, msg, nLen);
}

void CPlatformServer::OnReqClosePlatformServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pPlatformServerState->PlatformStateMachine(CPlatformServerState::PLATFORMINPUT_CLOSE, msg, nLen);
}

void CPlatformServer::OnCommandToPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::CommandMsg commandMsg;
	if (!commandMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " CommandMsg ParseFromArray failed";
		return;
	}

	NFGUID self;
	PBConvert::ToNF(commandMsg.self(), self);
	NFGUID target;
	PBConvert::ToNF(commandMsg.target(), target);
	NFDataList dataList;
	PBConvert::PBDataListToNFDataList(commandMsg.data(), dataList);

	if (!target.IsNull())
	{
		//有指定目标
		m_pClassModule->RunCommandCallBack(commandMsg.target_class_Name(), commandMsg.Id(), target, self, dataList);
	}
	else
	{
		// 没有指定的就给辅助对象
		m_pClassModule->RunCommandCallBack(pPluginManager->GetAppName(), commandMsg.Id(), target, self, dataList);
	}
}

void CPlatformServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CPlatformServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CPlatformServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CPlatformServer::GetTypePort(ENUM_NET_TYPE type)
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
void CPlatformServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (m_pPlatformServerState->GetPlatformState() == CPlatformServerState::PLATFORMSTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pPlatformServerState->GetState());
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