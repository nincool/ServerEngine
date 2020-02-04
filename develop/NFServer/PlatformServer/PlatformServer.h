///--------------------------------------------------------------------
/// 文件名:		PlatformServer.h
/// 内  容:		平台服
/// 说  明:		
/// 创建日期:	2019年10月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef PLATFORM_SERVER_H
#define PLATFORM_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "PlatformServerState.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPlatformServer : public NFIModule
{
public:
	CPlatformServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// 创建服务
	void CreateServer(const NetData& net_data);
	// 添加链接信息
	void ConnectServer(const ConnectCfg& connect_data);
	// 获取某类型IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// 获取某类型Port
	int GetTypePort(ENUM_NET_TYPE type);

	//更新程序报文
	void UpReport(bool force = false);
private:
	void OnReqClosePlatformServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// 客户端网络完成
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	void OnRoomServerNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnRoomServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCommandToPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// 检查是否完成本身服务
	int CheckServerStatus();
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CPlatformServerState* m_pPlatformServerState = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
