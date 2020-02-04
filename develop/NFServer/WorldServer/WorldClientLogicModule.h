///--------------------------------------------------------------------
/// 文件名:		WorldClientLogicModule.h
/// 内  容:		World作为客户端逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef _WORLD_CLIENTLOGIC_MODULE_H
#define _WORLD_CLIENTLOGIC_MODULE_H

#include "NFComm/NFPluginModule/NFISecurityModule.h"
#include "WorldServerLogicModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"

class CWorldServer;
class CWorldServerState;
class CWorldClientLogicModule : public NFIModule
{
public:
	CWorldClientLogicModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

private:
	void OnSelectServerProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCloseServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnConnectMaster(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 统计附加服务器连接信息
	void ExtraConnectServerInfo();

	//网络通信测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NetClientModule* m_pNetClientModule = nullptr;
	NFISecurityModule* m_pSecurityModule = nullptr;
	CWorldServerLogicModule* m_pWorldServerLogicModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CWorldServer* m_pWorldServer = nullptr;
	OuterMsg::WorldParam m_xWorldparam;
	CWorldServerState* m_pWorldServerState = nullptr;
};

#endif