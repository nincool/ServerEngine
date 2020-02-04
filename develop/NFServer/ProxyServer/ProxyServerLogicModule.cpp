///--------------------------------------------------------------------
/// 文件名:		ProxyServerLogicModule.cpp
/// 内  容:		Proxy作为服务器逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ProxyServerLogicModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/NFDefine.pb.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "ProxyServerState.h"
#include "ProxyClientLogicModule.h"
#include "NFComm/NFUtils/FuncUtil.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "ProxyServer.h"
#include <sys/timeb.h>

bool CProxyServerLogicModule::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pSecurityModule = pPluginManager->FindModule<NFISecurityModule>();
	m_pProxyClientModule = pPluginManager->FindModule<CProxyClientLogicModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pProxyServerState = pPluginManager->FindModule<CProxyServerState>();
	m_pNetServerOuterModule = pPluginManager->FindModule<NetServerOuterModule>();
	m_pProxyServer = pPluginManager->FindModule<CProxyServer>();

    return true;
}

bool CProxyServerLogicModule::Shut()
{
    return true;
}

bool CProxyServerLogicModule::Execute()
{
    return true;
}

bool CProxyServerLogicModule::AfterInit()
{
	//监听登录验证
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_REQ_VERIFY_KEY, this, (NetMsgFun)&CProxyServerLogicModule::OnConnectKeyProcess);
	//监听请求取得角色列表
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_REQ_ROLE_LIST, this, (NetMsgFun)&CProxyServerLogicModule::OnReqRoleListProcess);
	//监听创建角色请求
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_REQ_CREATE_ROLE, this, (NetMsgFun)&CProxyServerLogicModule::OnReqCreateRoleProcess);
	//监听删除角色请求
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_REQ_DELETE_ROLE, this, (NetMsgFun)&CProxyServerLogicModule::OnReqDelRoleProcess);
	//监听请求进入游戏
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_REQ_ENTER_GAME, this, (NetMsgFun)&CProxyServerLogicModule::OnReqEnterGameServer);
	//监听客户端消息转发到 场景服务器
	m_pNetServerOuterModule->AddReceiveCallBack(this, (NetMsgFun)&CProxyServerLogicModule::OnOtherMessage);

	//监听服务网络事件
	m_pNetServerOuterModule->AddEventCallBack(this, (NetEventFun)&CProxyServerLogicModule::OnSocketClientEvent);

	// 断开连接
	m_pNetClientModule->AddReceiveCallBack(NF_ST_WORLD, OuterMsg::SS_REQ_BREAK_CLIENT, this, (NetMsgFun)&CProxyServerLogicModule::OnReqBreakClient);

	//网络测试
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CProxyServerLogicModule::OnNetTest);
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::SC_TEST_NET, this, (NetMsgFun)&CProxyServerLogicModule::OnNetTestAck);

    return true;
}

//监听客户端消息转发到 场景服务器 回调
void CProxyServerLogicModule::OnOtherMessage(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (!pNetObject || pNetObject->GetConnectKeyState() <= 0 || pNetObject->GetGameServerAppID() <= 0)
	{
		QLOG_ERROR << __FUNCTION__ << " state error";
		return;
	}

	//解密
	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(msg, pNetObject->GetSecurityKey(), nLen, iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_ERROR << __FUNCTION__ << " DecodeMsg failed";
		return;
	}

	OuterMsg::MsgBase xMsg;
	xMsg.set_msg_data(strMsgData, iDecodeLen);
	*xMsg.mutable_player_id() = NFToPB(pNetObject->GetPlayerID());

	std::string strMsg;
	if (!xMsg.SerializeToString(&strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " SerializeToString failed";
		return;
	}

	if (xMsg.has_hash_ident())
	{
		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_GAME, nMsgID, xMsg);
	}
	else
	{
		m_pNetClientModule->SendMsg(pNetObject->GetGameServerAppID(), NF_SERVER_TYPES::NF_ST_GAME, nMsgID, strMsg);
	}
}

//监听登录验证 回调
void CProxyServerLogicModule::OnConnectKeyProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// 如果许可证没通过
	if (!m_pProxyServer->GetLicVaild())
	{
		CloseClient(nSockIndex);
		return;
	}

	// 查看服务状态
	if (m_pProxyServerState->GetProxyState() != CProxyServerState::PROXYSTATE_OPENED)
	{
		CloseClient(nSockIndex);
		return;
	}
	
	OuterMsg::ReqAccountLogin xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " SerializeToString failed";
		return;
	}

	bool bRet = m_pProxyClientModule->VerifyConnectData(xMsg.account(), xMsg.security_code());
	if (bRet)
	{
		NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
		if (pNetObject)
		{
			int nGameID = 0;
			std::vector<const ConnectOBJ*> vec_game;
			m_pNetClientModule->GetConnectByType(NF_SERVER_TYPES::NF_ST_GAME, vec_game);
			if (vec_game.size() > 0)
			{
				int nRand = util_random_int(vec_game.size());
				nGameID = vec_game[nRand]->report.app_id();
			}

			if (nGameID > 0)
			{
				pNetObject->SetGameServerAppID(nGameID);
			}
			else
			{
				OuterMsg::AckEventResult xSendMsg;
				xSendMsg.set_event_code(OuterMsg::EGEC_SELECTSERVER_FAIL);

				m_pNetServerOuterModule->SendMsg(OuterMsg::SC_ACK_VERIFY_KEY, xSendMsg, nSockIndex);
				return;
			}

			//this net-object verify successful and set state as true
			pNetObject->SetConnectKeyState(1);
			pNetObject->SetSecurityKey(xMsg.security_code());

			//this net-object bind a user's account
			pNetObject->SetAccount(xMsg.account());

			OuterMsg::AckEventResult xSendMsg;
			xSendMsg.set_event_code(OuterMsg::EGEC_SUCCESS);
			*xSendMsg.mutable_event_client() = NFToPB(pNetObject->GetClientID());

			m_pNetServerOuterModule->SendMsg(OuterMsg::SC_ACK_VERIFY_KEY, xSendMsg, nSockIndex);
		}
	}
	else
	{
		//帐号登录串验证不通过
		OuterMsg::AckEventResult xSendMsg;
		xSendMsg.set_event_code(OuterMsg::EGEC_ACCOUNTVERIFY_TIMEOUT);

		m_pNetServerOuterModule->SendMsg(OuterMsg::SC_ACK_VERIFY_KEY, xSendMsg, nSockIndex);

		//if verify failed then close this connect
		CloseClient(nSockIndex);
	}
}

//转发
int CProxyServerLogicModule::Transpond(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	int amount = 0;
	OuterMsg::MsgBase xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " MsgBase ParseFromArray failed";
		return amount;
	}

	//broadcast many palyers
	for (int i = 0; i < xMsg.player_Client_list_size(); ++i)
	{
		NF_SHARE_PTR<NFSOCK> pFD = mxClientIdent.GetElement(PBToNF(xMsg.player_Client_list(i)));
		if (pFD)
		{
			if (m_pNetServerOuterModule->SendMsg(nMsgID, xMsg.msg_data(), *pFD))
			{
				++amount;
			}
		}
		else
		{
			QLOG_WARING << "Transpond fail, nMsgID:" << nMsgID;
		}
	}

	//send message to one player
	if (xMsg.player_Client_list_size() <= 0)
	{
		NFGUID xClientIdent = PBToNF(xMsg.player_id());
		NF_SHARE_PTR<NFSOCK> pFD = mxClientIdent.GetElement(xClientIdent);
		if (pFD)
		{
			if (m_pNetServerOuterModule->SendMsg(nMsgID, xMsg.msg_data(), *pFD))
			{
				++amount;
			}
		}
		else if (xClientIdent.IsNull())
		{
			//send this msessage to all clientss
			if (m_pNetServerOuterModule->SendMsgToAll(nMsgID, xMsg.msg_data()))
			{
				++amount;
			}
		}
		else
		{
			QLOG_WARING << "Transpond fail, nMsgID:" << nMsgID;
		}
	}

	return amount;
}

//监听请求取得角色列表 回调
void CProxyServerLogicModule::OnReqRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (!pNetObject)
	{
		QLOG_WARING << __FUNCTION__ << " nSockIndex not find:" << nSockIndex;
		return;
	}

	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(msg, pNetObject->GetSecurityKey(), nLen, iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_WARING << __FUNCTION__ << " DecodeMsg failed";
		return;
	}

	OuterMsg::ReqRoleList xData;
	if (!xData.ParseFromArray(strMsgData, iDecodeLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqRoleList ParseFromArray failed";
		return;
	}

	const ConnectOBJ* pServerData = m_pNetClientModule->GetConnect(pPluginManager->GetServerID(),
		NF_SERVER_TYPES::NF_ST_GAME, pNetObject->GetGameServerAppID());
	if (pServerData && ConnectState::NORMAL == pServerData->eState)
	{
		if (pNetObject->GetConnectKeyState() > 0
			&& pNetObject->GetAccount() == xData.account())
		{
			OuterMsg::MsgBase xMsg;
			if (!xData.SerializeToString(xMsg.mutable_msg_data()))
			{
				QLOG_WARING << __FUNCTION__ << " MsgBase SerializeToString failed";
				return;
			}

			//clientid
			if (pNetObject->GetClientID().IsNull())
			{
				QLOG_ERROR << "ClientID IsNull. account:" << xData.account() << ",SockIndex:" << nSockIndex;
				return;
			}
			*xMsg.mutable_player_id() = NFToPB(pNetObject->GetClientID());

			std::string strMsg;
			if (!xMsg.SerializeToString(&strMsg))
			{
				QLOG_WARING << __FUNCTION__ << " MsgBase SerializeToString failed";
				return;
			}

			m_pNetClientModule->SendMsg(pNetObject->GetGameServerAppID(), NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::CS_REQ_ROLE_LIST, strMsg);
		}
	}
	else
	{
		QLOG_ERROR << "game server or NetObject failed";
	}
}

//监听创建角色请求 回调
void CProxyServerLogicModule::OnReqCreateRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (!pNetObject)
	{
		QLOG_WARING << __FUNCTION__ << " nSockIndex not find:" << nSockIndex;
		return;
	}

	if (pNetObject->GetConnectKeyState() <= 0)
	{
		return;
	}

	const ConnectOBJ* pServerData = m_pNetClientModule->GetConnect(pPluginManager->GetServerID(),
		NF_SERVER_TYPES::NF_ST_GAME, pNetObject->GetGameServerAppID());
	if (!pServerData || ConnectState::NORMAL != pServerData->eState)
	{
		return;
	}

	//解密
	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(msg, pNetObject->GetSecurityKey(), nLen, iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_WARING << __FUNCTION__ << " DecodeMsg failed";
		return;
	}

	OuterMsg::ReqCreateRole xData;
	if (!xData.ParseFromArray(strMsgData, iDecodeLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
		return;
	}

	xData.set_account(pNetObject->GetAccount());
	//在玩家进入游戏服务器之前的客户端ID == 玩家ID
	PBConvert::ToPB(pNetObject->GetClientID(), *xData.mutable_client_id());

	std::string strMsg;
	if (!xData.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " ReqCreateRole SerializeToString failed";
		return;
	}

	//发到 GameServer
	m_pNetClientModule->SendMsg(pNetObject->GetGameServerAppID(), NF_SERVER_TYPES::NF_ST_GAME, nMsgID, strMsg);
}

//监听删除角色请求回调
void CProxyServerLogicModule::OnReqDelRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (!pNetObject)
	{
		QLOG_WARING << __FUNCTION__ << " nSockIndex not find:" << nSockIndex;
		return;
	}

	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(msg, pNetObject->GetSecurityKey(), nLen, iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_WARING << __FUNCTION__ << " DecodeMsg failed";
		return;
	}

	OuterMsg::ReqDeleteRole xData;
	if (!xData.ParseFromArray(strMsgData, iDecodeLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqDeleteRole ParseFromArray failed";
		return;
	}

	const ConnectOBJ* pServerData = m_pNetClientModule->GetConnect(pPluginManager->GetServerID(),
		NF_SERVER_TYPES::NF_ST_GAME, pNetObject->GetGameServerAppID());
	if (pServerData && ConnectState::NORMAL == pServerData->eState)
	{
		if (pNetObject->GetConnectKeyState() > 0
			&& pNetObject->GetAccount() == xData.account())
		{
			OuterMsg::MsgBase xMsg;
			if (!xData.SerializeToString(xMsg.mutable_msg_data()))
			{
				QLOG_WARING << __FUNCTION__ << " MsgBase SerializeToString failed";
				return;
			}

			//clientid
			xMsg.mutable_player_id()->CopyFrom(NFToPB(pNetObject->GetClientID()));

			std::string strMsg;
			if (!xMsg.SerializeToString(&strMsg))
			{
				QLOG_WARING << __FUNCTION__ << " MsgBase SerializeToString failed";
				return;
			}

			m_pNetClientModule->SendMsg(pNetObject->GetGameServerAppID(), NF_SERVER_TYPES::NF_ST_GAME, nMsgID, strMsg);
		}
	}
}

//监听请求进入游戏 回调
void CProxyServerLogicModule::OnReqEnterGameServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (!pNetObject)
	{
		QLOG_ERROR << __FUNCTION__ << " nSockIndex not find:" << nSockIndex;
		return;
	}

	int iDecodeLen = 0;
	const char* strMsgData = m_pSecurityModule->DecodeMsg(msg, pNetObject->GetSecurityKey(), nLen, iDecodeLen);
	if (strcmp(strMsgData, NO_ENCODE) == 0)
	{
		QLOG_ERROR << __FUNCTION__ << " DecodeMsg failed";
		return;
	}

	OuterMsg::ReqEnterGameServer xData;
	if (!xData.ParseFromArray(strMsgData, iDecodeLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqEnterGameServer ParseFromArray failed";
		return;
	}

	const ConnectOBJ* pServerData = m_pNetClientModule->GetConnect(pPluginManager->GetServerID(),
		NF_SERVER_TYPES::NF_ST_GAME, pNetObject->GetGameServerAppID());
	if (pServerData && ConnectState::NORMAL == pServerData->eState)
	{
		if (pNetObject->GetConnectKeyState() > 0)
		{
			xData.set_account(pNetObject->GetAccount());
			xData.set_ip(pNetObject->GetIP());

			OuterMsg::MsgBase xMsg;
			if (!xData.SerializeToString(xMsg.mutable_msg_data()))
			{
				QLOG_ERROR << __FUNCTION__ << " MsgBase SerializeToString failed";
				return;
			}

			//clientid
			xMsg.mutable_player_id()->CopyFrom(NFToPB(pNetObject->GetClientID()));
			std::string strMsg;
			if (!xMsg.SerializeToString(&strMsg))
			{
				QLOG_ERROR << __FUNCTION__ << " MsgBase SerializeToString failed";
				return;
			}

			m_pNetClientModule->SendMsg(pNetObject->GetGameServerAppID(), NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::CS_REQ_ENTER_GAME, strMsg);
		}
	}
	else
	{
		QLOG_ERROR << "GAME_SERVER failed";
	}
}

int CProxyServerLogicModule::EnterGameSuccessEvent(const NFGUID xClientID, const NFGUID xPlayerID)
{
	NF_SHARE_PTR<NFSOCK> pFD = mxClientIdent.GetElement(xClientID);
	if (pFD)
	{
		NetObj* pNetObeject = m_pNetServerOuterModule->GetNetObj(*pFD);
		if (pNetObeject)
		{
			pNetObeject->SetPlayerID(xPlayerID);
		}
	}

	return 0;
}

//服务 网络事件回调
void CProxyServerLogicModule::OnSocketClientEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent)
{
	std::string logStr;
	logStr = "SockIndex:" + lexical_cast<std::string>(nSockIndex);
	if ((int)eEvent & (int)NET_EVENT::NET_EVENT_EOF)
	{
		logStr.append(", connection closed,");
		OnClientDisconnect(nSockIndex);
	}
	else if ((int)eEvent & (int)NET_EVENT::NET_EVENT_ERROR)
	{
		logStr.append(", got an error on the connection,");
		OnClientDisconnect(nSockIndex);
	}
	else if ((int)eEvent & (int)NET_EVENT::NET_EVENT_TIMEOUT)
	{
		logStr.append(", read timeout,");
		OnClientDisconnect(nSockIndex);
	}
	else  if ((int)eEvent & (int)NET_EVENT::NET_EVENT_CONNECTED)
	{
		logStr.append(", connected success,");
		OnClientConnected(nSockIndex);
	}
	else
	{
		OnClientDisconnect(nSockIndex);
	}

	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (pNetObject)
	{
		logStr.append(", ip:");
		logStr.append(pNetObject->GetIP());
	}
	QLOG_SYSTEM_N << logStr << ", total connected:" << mxClientIdent.Count();
}

//有客户端断开连接事件回调
void CProxyServerLogicModule::OnClientDisconnect(const NFSOCK nAddress)
{
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nAddress);
	if (pNetObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find, sockIndex:" << nAddress;
		return;
	}

	// 删除连接信息
	mxClientIdent.RemoveElement(pNetObject->GetClientID());

	auto& self = pNetObject->GetPlayerID();
	if (self.IsNull())
	{
		return;
	}

	OuterMsg::ClientConnectInfo xMsg;
	PBConvert::ToPB(self, *xMsg.mutable_player());
	PBConvert::ToPB(pNetObject->GetClientID(), *xMsg.mutable_client());
	std::string strMsg;
	if (!xMsg.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " ClientConnectInfo SerializeToString failed";
		return;
	}

	// 通知world断开连接
	m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_USER_BREAK, strMsg);
}

//有客户端连接成功事件回调
void CProxyServerLogicModule::OnClientConnected(const NFSOCK nAddress)
{
	//bind client'id with socket id
	NetObj* pNetObject = m_pNetServerOuterModule->GetNetObj(nAddress);
	if (pNetObject)
	{
		mxClientIdent.AddElement(pNetObject->GetClientID(), NF_SHARE_PTR<NFSOCK>(new NFSOCK(nAddress)));
	}
}

int CProxyServerLogicModule::CloseAllClient()
{
	NF_SHARE_PTR<NFSOCK> pFD = mxClientIdent.First();
	while (pFD != nullptr)
	{
		m_pNetServerOuterModule->CloseNetObj(*pFD);
		pFD = mxClientIdent.Next();
	}

	return 0;
}

int CProxyServerLogicModule::CloseClient(const NFSOCK nSockIndex)
{
	m_pNetServerOuterModule->CloseNetObj(nSockIndex);

	return 0;
}

void CProxyServerLogicModule::OnReqBreakClient(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	NFGUID client;
	PBConvert::ToNF(xMsg, client);
	auto pSocket = mxClientIdent.GetElement(client);
	if (pSocket == nullptr)
	{
		return;
	}

	CloseClient(*pSocket);
}

//网络测试
void CProxyServerLogicModule::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	xmsg.set_client_sock(nSockIndex);

	struct timeb t1;
	ftime(&t1);
	xmsg.mutable_data()->append("Proxy:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetClientModule->SendMsgByType(NF_ST_GAME, OuterMsg::CS_TEST_NET, xmsg);
	m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::CS_TEST_NET, xmsg);
}

void CProxyServerLogicModule::OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	m_pNetClientModule->SendMsgBySock(xmsg.client_sock(), OuterMsg::SC_TEST_NET, xmsg);
}