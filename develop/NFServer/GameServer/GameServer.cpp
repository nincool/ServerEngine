///--------------------------------------------------------------------
/// 文件名:		GameServer.cpp
/// 内  容:		游戏服
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "GameServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFUtils/QLOG.h"
#include <sys/timeb.h>
#include "NFComm/NFCore/NFPlayer.h"

bool CGameServer::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pGameServerState = pPluginManager->FindModule<CGameServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pCreateRoleModule = pPluginManager->FindModule<ICreateRoleModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	return true;
}

bool CGameServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CGameServer>("CGameServer")
		.addProperty("RoleNameMaxSize", &CGameServer::GetRoleNameMaxSize, &CGameServer::SetRoleNameMaxSize)
		.addProperty("SaveDataTime", &CGameServer::GetSaveDataTime, &CGameServer::SetSaveDataTime)
		.addProperty("AlterPlayerLifeTime", &CGameServer::GetAlterPlayerLifeTime, &CGameServer::SetAlterPlayerLifeTime)

		.addFunction("connect_server", &CGameServer::ConnectServer)
		.addFunction("create_server", &CGameServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CGameServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CGameServer::OnClientNetState));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_SERVER_COMPLETE_ALLNET, this, (NetMsgFun)&CGameServer::OnServerCompleteAllNet);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_CLOSE_GAMESERVER, this, (NetMsgFun)&CGameServer::OnReqCloseGameServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_OPEN_ALLSERVER, this, (NetMsgFun)&CGameServer::OnOpenAllServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_LOAD_PUB_COMPLETE, this, (NetMsgFun)&CGameServer::AckLoadPubComplete);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_LOAD_GUILD_COMPLETE, this, (NetMsgFun)&CGameServer::AckLoadGuildComplete);
	m_pKernelModule->AddCommondCallBack(pPluginManager->GetAppName(), OuterMsg::SS_ACK_LOAD_GUID_NAME_COMPLETE, METHOD_ARGS(CGameServer::AckLoadGuidNameComplete));

	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_COMMAND_TOGAME, this, (NetMsgFun)&CGameServer::OnCommandToGame);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_COMMAND_TOGAME, this, (NetMsgFun)&CGameServer::OnCommandToGame);

	//网络通信测试
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SC_TEST_NET, this, (NetMsgFun)&CGameServer::OnNetTest);

	return true;
}

bool CGameServer::InitNetServer()
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

bool CGameServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	// 更新report信息
	UpReport();

	return true;
}

bool CGameServer::Execute()
{
	return true;
}

void CGameServer::AckLoadPubComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pGameServerState->GameStateMachine(CGameServerState::GAMEINPUT_PUBLOAD);
}

void CGameServer::AckLoadGuildComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pGameServerState->GameStateMachine(CGameServerState::GAMEINPUT_GUILDLOAD);
}

void CGameServer::AckLoadGuidNameComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	m_pGameServerState->GameStateMachine(CGameServerState::GAMEINPUT_GUIDNAMELOAD);
}

int CGameServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
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
	
	return 0;
}

int CGameServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete)
	{
		//进入等待开启状态
		m_pGameServerState->GameStateMachine(CGameServerState::GAMEINPUT_WAITINGOPEN);
		if (!m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_NET_COMPLETE_GAMESERVER, std::string()))
		{
			QLOG_ERROR << __FUNCTION__ << "server net complete send world failed";
		}
	}

	return 0;
}

void CGameServer::OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// 启动回调事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllServerComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CGameServer::OnReqCloseGameServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pGameServerState->GameStateMachine(CGameServerState::GAMEINPUT_CLOSE, msg, nLen);
}

void CGameServer::OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pGameServerState->GameStateMachine(CGameServerState::GAMEINPUT_LOADING, msg, nLen);
}

void CGameServer::OnCommandToGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::CommandMsg commandMsg;
	if (!commandMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << "CommandMsg ParseFromArray failed";
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

void CGameServer::SendMsgPBToGate(const uint16_t nMsgID, const google::protobuf::Message& xMsg, const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		return;
	}

	const InsideServerData* pServerData = m_pNetServerInsideModule->GetSameGroupServer(pPlayer->GetGateID(), NF_ST_PROXY);
	if (pServerData != nullptr)
	{
		std::string strData;
		AddNetPB(xMsg, pPlayer->GetClientID(), strData);
		m_pNetServerInsideModule->SendMsg(nMsgID, strData, pServerData->nSockIndex);
	}
	else
	{
		QLOG_WARING << __FUNCTION__ << " " << pPlayer->GetGateID() << "Can't find";
	}
}

void CGameServer::SendMsgToGate(const uint16_t nMsgID, const std::string& strMsg, const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		return;
	}

	const InsideServerData* pServerData = m_pNetServerInsideModule->GetSameGroupServer(pPlayer->GetGateID(), NF_ST_PROXY);
	if (pServerData != nullptr)
	{
		std::string strData;
		AddNetPB(strMsg, pPlayer->GetClientID(), strData);
		m_pNetServerInsideModule->SendMsg(nMsgID, strMsg, pServerData->nSockIndex);
	}
}

//发给消息给当前世界所有玩家客户端
bool CGameServer::SendMsgToClientAll(const uint16_t nMsgID, const google::protobuf::Message& xMsg)
{
	OuterMsg::MsgBase xMsgBase;
	if (!xMsg.SerializeToString(xMsgBase.mutable_msg_data()))
	{
		return false;
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_PROXY, nMsgID, xMsgBase);

	return true;
}

void CGameServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CGameServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CGameServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CGameServer::GetTypePort(ENUM_NET_TYPE type)
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
void CGameServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (m_pGameServerState->GetGameState() == CGameServerState::GAMESTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pGameServerState->GetState());
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
		m_pNetClientModule->UpReport(true);
		m_pNetServerInsideModule->UpReport(true);
	}
}

//网络通信测试
void CGameServer::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	struct timeb t1;
	ftime(&t1);
	xmsg.mutable_data()->append("Game:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetServerInsideModule->SendMsg(OuterMsg::SC_TEST_NET, xmsg, nSockIndex);
}