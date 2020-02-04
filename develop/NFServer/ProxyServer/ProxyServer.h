///--------------------------------------------------------------------
/// 文件名:		ProxyServer.h
/// 内  容:		代理服
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "ProxyServerState.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"

class CProxyServer : public NFIModule
{
public:
	CProxyServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// 许可证地址
	void SetLicense(std::string& value) { m_nLicense = value; }
	std::string& GetLicense() { return m_nLicense; }

	// 许可证开始时间
	void SetLicStartTime(int64_t value) { m_nLicStartTime = value; }
	int64_t GetLicStartTime() const { return m_nLicStartTime; }

	// 许可证结束时间
	void SetLicEndTime(int64_t value) { m_nLicEndTime = value; }
	int64_t GetLicEndTime() const { return m_nLicEndTime; }

	// 许可证有效标志
	void SetLicVaild(bool value) { m_nLicVaild = value; }
	bool GetLicVaild() const { return m_nLicVaild; }

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
	void OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqCloseProxyServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqSynInfo(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 客户端网络完成
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// 检查是否完成本身服务
	int CheckServerStatus();
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;
	NFINT64 m_nLastCheckTime = 0;

	//许可证地址
	std::string m_nLicense = "";
	//许可证开始时间
	int64_t m_nLicStartTime = 0;
	//许可证结束时间
	int64_t m_nLicEndTime = 0;
	//许可证有效标志
	bool m_nLicVaild = false;

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	CProxyServerState* m_pProxyServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
};

#endif
