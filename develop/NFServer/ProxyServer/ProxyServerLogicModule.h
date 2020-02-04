///--------------------------------------------------------------------
/// 文件名:		NFProxyServerLogicModule.h
/// 内  容:		Proxy作为服务器逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef NF_PROXY_SERVERLOGIC_MODULE_H
#define NF_PROXY_SERVERLOGIC_MODULE_H

#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFISecurityModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"

class CProxyClientLogicModule;
class CProxyServerState;
class CProxyServer;
class CProxyServerLogicModule : public NFIModule
{
public:
	CProxyServerLogicModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

	virtual int Transpond(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	virtual int EnterGameSuccessEvent(const NFGUID xClientID, const NFGUID xPlayerID);
	virtual int CloseAllClient();
	
private:
	void OnSocketClientEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);
	void OnClientDisconnect(const NFSOCK nAddress);
	void OnClientConnected(const NFSOCK nAddress);
	void OnConnectKeyProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqCreateRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqDelRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqEnterGameServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnOtherMessage(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqBreakClient(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	int CloseClient(const NFSOCK nSockIndex);

	//网络测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NFMapEx<NFGUID, NFSOCK> mxClientIdent;

	NFIClassModule* m_pClassModule = nullptr;
    NFIKernelModule* m_pKernelModule = nullptr;
	NFISecurityModule* m_pSecurityModule = nullptr;
	CProxyClientLogicModule* m_pProxyClientModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	CProxyServerState* m_pProxyServerState = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	CProxyServer* m_pProxyServer = nullptr;
};

#endif