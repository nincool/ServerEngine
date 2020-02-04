///--------------------------------------------------------------------
/// 文件名:		LoginServer.cpp
/// 内  容:		Login逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "LoginServer.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFUtils/NFTool.hpp"
#include <sys/timeb.h>
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "counter.h"
#include "NFComm/NFPluginLoader/NFPluginManager.h"

bool CLoginServer::Init()
{
	m_pHttpNetModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pLogicClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pSecurityModule = pPluginManager->FindModule<NFISecurityModule>();
	
	return true;
}

bool CLoginServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CLoginServer>("CLoginServer")
		.addFunction("connect_server", &CLoginServer::ConnectServer)
		.addFunction("create_server", &CLoginServer::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CLoginServer::Shut()
{
	return true;
}

bool CLoginServer::AfterInit()
{
	m_pLogicClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CLoginServer::OnClientNetState));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::SS_STS_NET_INFO, this, (NetMsgFun)&CLoginServer::OnServerInfoProcess);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::SC_ACK_CONNECT_WORLD, this, (NetMsgFun)&CLoginServer::OnAckConnectWorld);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::CS_VERIFY_ACCOUNT, this, (NetMsgFun)&CLoginServer::OnVerifyAccount);

	//取得服务器列表
	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::CS_REQ_WORLD_LIST), NFHttpType::NF_HTTP_REQ_POST, this, &CLoginServer::OnWorldView);
	//连接服务器
	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::CS_REQ_LOGIN), NFHttpType::NF_HTTP_REQ_POST, this, &CLoginServer::OnWorldSelect);

	//网络通信测试
	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::CS_TEST_NET), NFHttpType::NF_HTTP_REQ_POST, this, &CLoginServer::OnNetTest);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::SC_TEST_NET, this, (NetMsgFun)&CLoginServer::OnNetTestAck);

	m_pHttpNetModule->AddNetTimeOut(this, &CLoginServer::OnTimeOut);

	return true;
}

bool CLoginServer::InitNetServer()
{
	int nServerNet = 0;
	// 创建服务器网络
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
			break;
		case NET_TYPE_MAINTAIN:
			break;
		case NET_TYPE_HTTP:
			if (m_pHttpNetModule->InitServer(mxNetList[i].GetIP().c_str(), mxNetList[i].GetPort()) == 0)
			{
				++nServerNet;
			}
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

bool CLoginServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	UpReport();

	return true;
}

//更新程序报文
void CLoginServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (bClientNetComplete && bServerNetComplete)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
		xServerInfoReport.set_state_info("Opened");
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
		xServerInfoReport.set_state_info("Closed");
	}
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

bool CLoginServer::Execute()
{
	return true;
}

// 客户端网络完成
int CLoginServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
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

int CLoginServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete)
	{
		// 更新report信息
		UpReport();

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "LoginServer Opened";

		CATCH_BEGIN
		auto& gauge_family = pPluginManager->GetMetricsGauge(1);
		auto& second_gauge = gauge_family.Add({ {"State", "Opened"} });
		second_gauge.Increment();
		CATCH_END
	}

	return 0;
}

void CLoginServer::OnServerInfoProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	if (!mServerMap.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ParseFromArray failed";
		return;
	}
}

//取得服务器列表
bool CLoginServer::OnWorldView(const NFHttpRequest& req)
{
	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(req.body.c_str(), LOGINCRYPTO, req.body.length(), iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_WARING << __FUNCTION__ << " DecodeMsg failed";
		return false;
	}

	OuterMsg::AckWorldList xData;
	for (auto &it: mServerMap.server_list())
	{
		OuterMsg::ServerInfo* pServerInfo = xData.add_info();

		pServerInfo->set_name(it.second.server_name());
		pServerInfo->set_status(it.second.server_state());
		pServerInfo->set_server_id(it.second.server_id());
		pServerInfo->set_wait_count(0);
	}
	m_pHttpNetModule->ResponseMsgByHead(req, xData, OuterMsg::SC_ACK_WORLD_LIST);

	return true;
}

//连接服务器
bool CLoginServer::OnWorldSelect(const NFHttpRequest & req)
{
	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(req.body.c_str(), LOGINCRYPTO, req.body.length(), iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_WARING << __FUNCTION__ << " DecodeMsg failed";
		return false;
	}

	OuterMsg::ReqAccountLogin xMsg;
	if (!xMsg.ParseFromArray(strMsgData, iDecodeLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqAccountLogin ParseFromArray failed";
		return false;
	}

	auto it = mServerMap.server_list().find(xMsg.world_id());
	if (it == mServerMap.server_list().end())
	{
		OuterMsg::AckEventResult xAckMsg;
		xAckMsg.set_event_code(OuterMsg::EGEC_SELECTSERVER_FAIL);
		return m_pHttpNetModule->ResponseMsgByHead(req, xAckMsg, OuterMsg::SC_ACK_LOGIN);
	}
	else
	{
		if (it->second.server_state() != OuterMsg::EST_NARMAL)
		{
			OuterMsg::AckEventResult xAckMsg;
			xAckMsg.set_event_code(OuterMsg::EGEC_SELECTSERVER_FAIL);
			return m_pHttpNetModule->ResponseMsgByHead(req, xAckMsg, OuterMsg::SC_ACK_LOGIN);
		}

		//人数满
		if (it->second.server_cur_count() >= it->second.server_max_online())
		{
			OuterMsg::AckEventResult xAckMsg;
			xAckMsg.set_event_code(OuterMsg::EGEC_PLAYER_FULL);
			return m_pHttpNetModule->ResponseMsgByHead(req, xAckMsg, OuterMsg::SC_ACK_LOGIN);
		}
	}

	// 异步验证帐号
	if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::CS_VERIFY_ACCOUNT, std::string(strMsgData, iDecodeLen)))
	{
		OuterMsg::AckEventResult xAckMsg;
		xAckMsg.set_event_code(OuterMsg::EGEC_ACCOUNTVERIFY_TIMEOUT);
		return m_pHttpNetModule->ResponseMsgByHead(req, xAckMsg, OuterMsg::SC_ACK_LOGIN);
	}
		
	mWaitHttpRequest.insert(std::make_pair(xMsg.account(), req.id));
	mWaitHttpRequest_byID.insert(std::make_pair(req.id, xMsg.account()));

	return true;
}

void CLoginServer::OnVerifyAccount(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::MsgReply reply;
	reply.ParseFromArray(msg, nLen);

	OuterMsg::ReqAccountLogin xMsg;
	if (!xMsg.ParseFromString(reply.extra_data()))
	{
		QLOG_WARING << __FUNCTION__ << " ReqAccountLogin ParseFromString failed";
		return;
	}

	auto it = mWaitHttpRequest.find(xMsg.account());
	if (mWaitHttpRequest.end() == it)
	{
		QLOG_DEBUG << " mWaitHttpRequest not find:" << xMsg.account();
		return;
	}

	NFHttpRequest* pReq = m_pHttpNetModule->GetNFHttpRequest(it->second);
	if (nullptr == pReq)
	{
		QLOG_DEBUG << " pReq not find:" << xMsg.account();
		return;
	}

	//判断验证结查
	if (reply.ret_code() != OuterMsg::EGEC_SUCCESS)
	{
		QLOG_DEBUG << "Check password failed, Account = " << reply.extra_data();

		OuterMsg::AckEventResult xAckMsg;
		xAckMsg.set_event_code(OuterMsg::EGEC_ACCOUNTPWD_INVALID);
		m_pHttpNetModule->ResponseMsgByHead(*pReq, xAckMsg, OuterMsg::SC_ACK_LOGIN);

		return;
	}

	//通知世界服务器
	OuterMsg::ReqConnectWorld xData;
	xData.set_world_id(xMsg.world_id());
	xData.set_login_id(pPluginManager->GetAppID());
	xData.mutable_sender()->CopyFrom(NFToPB(NFGUID()));
	xData.set_account(xMsg.account());
	xData.set_clientVersion(xMsg.clientVersion());
	xData.set_client_ip(pReq->remoteHost);

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::CS_REQ_CONNECT_WORLD, xData);
}

void CLoginServer::OnTimeOut(const NFHttpRequest& req)
{
	//输出日志
	QLOG_SYSTEM_S << "Login Time Out.";

	//删了记录
	auto it_byID= mWaitHttpRequest_byID.find(req.id);
	if (it_byID != mWaitHttpRequest_byID.end())
	{
		auto it = mWaitHttpRequest.find(it_byID->second);
		if (it != mWaitHttpRequest.end())
		{
			mWaitHttpRequest.erase(it);
		}

		mWaitHttpRequest_byID.erase(it_byID);
	}
}

//接收选择请求进入世界服务器结果返回
void CLoginServer::OnAckConnectWorld(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckConnectWorldResult xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckConnectWorldResult ParseFromArray failed";
		return;
	}

	auto it = mWaitHttpRequest.find(xMsg.account());
	if (mWaitHttpRequest.end() == it)
	{
		QLOG_DEBUG << " mWaitHttpRequest not find:" << xMsg.account();
		return;
	}

	int nReqId = it->second;

	mWaitHttpRequest.erase(it);
	auto it_byID = mWaitHttpRequest_byID.find(nReqId);
	if (it_byID != mWaitHttpRequest_byID.end())
	{
		mWaitHttpRequest_byID.erase(it_byID);
	}

	NFHttpRequest* pReq = m_pHttpNetModule->GetNFHttpRequest(nReqId);
	if (nullptr == pReq)
	{
		QLOG_DEBUG << " pReq not find:" << xMsg.account();
		return;
	}

	m_pHttpNetModule->ResponseMsgByHead(*pReq, xMsg, OuterMsg::SC_ACK_CONNECT_WORLD);
}

void CLoginServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CLoginServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CLoginServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CLoginServer::GetTypePort(ENUM_NET_TYPE type)
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

//网络通信测试
bool CLoginServer::OnNetTest(const NFHttpRequest& req)
{
	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(req.body.c_str(), LOGINCRYPTO, req.body.length(), iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_WARING << __FUNCTION__ << " DecodeMsg failed";
		return false;
	}

	OuterMsg::NetTest xmsg;
	if (!xmsg.ParseFromArray(strMsgData, iDecodeLen))
	{
		QLOG_WARING << __FUNCTION__ << " NetTest ParseFromArray failed";
		return false;
	}

	//记录
	struct timeb t1;
	ftime(&t1);
	xmsg.set_client_sock(req.id);
	xmsg.mutable_data()->append("Login:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_MASTER, OuterMsg::CS_TEST_NET, xmsg);

	return true;
}

void CLoginServer::OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	NFHttpRequest* pReq = m_pHttpNetModule->GetNFHttpRequest(xmsg.client_sock());
	if (nullptr == pReq)
	{
		QLOG_DEBUG << " pReq not find:" << xmsg.client_sock();
		return;
	}

	m_pHttpNetModule->ResponseMsgByHead(*pReq, xmsg, OuterMsg::SC_TEST_NET);
}