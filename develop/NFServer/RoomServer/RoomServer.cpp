///--------------------------------------------------------------------
/// 文件名:		RoomServer.cpp
/// 内  容:		房间服
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "RoomServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/LuaExt.hpp"

bool CRoomServer::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pRoomServerState = pPluginManager->FindModule<CRoomServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();

	return true;
}

bool CRoomServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CRoomServer::OnClientNetState));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PLATFORM, OuterMsg::SS_SERVER_COMPLETE_ALLNET, this, (NetMsgFun)&CRoomServer::OnServerCompleteAllNet);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PLATFORM, OuterMsg::SS_OPEN_ALLSERVER, this, (NetMsgFun)&CRoomServer::OnServerAllOpen);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PLATFORM, OuterMsg::SS_COMMAND_TOROOM, this, (NetMsgFun)&CRoomServer::OnCommandToRoom);

	return true;
}

bool CRoomServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CRoomServer>("CRoomServer")
		.addFunction("connect_server", &CRoomServer::ConnectServer)
		.addFunction("create_server", &CRoomServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CRoomServer::InitNetServer()
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

bool CRoomServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// 更新report信息
	UpReport();

	return true;
}

bool CRoomServer::Execute()
{
	return true;
}

int CRoomServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
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

int CRoomServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete)
	{
		if (m_pNetClientModule->SendMsgByType(NF_ST_PLATFORM, OuterMsg::SS_NET_COMPLETE_ROOMSERVER, NULL_STR))
		{
			//进入等待开启状态
			m_pRoomServerState->RoomStateMachine(CRoomServerState::ROOMINPUT_WAITINGOPEN);
		}
		else
		{
			QLOG_ERROR << __FUNCTION__ << "room net complete send world failed";
		}
	}

	return 0;
}

void CRoomServer::OnServerAllOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllServerComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CRoomServer::OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pRoomServerState->RoomStateMachine(CRoomServerState::ROOMINPUT_OPEN, msg, nLen);
}

void CRoomServer::OnCommandToRoom(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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

void CRoomServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CRoomServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CRoomServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CRoomServer::GetTypePort(ENUM_NET_TYPE type)
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
void CRoomServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (m_pRoomServerState->GetRoomState() == CRoomServerState::ROOMSTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pRoomServerState->GetState());
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