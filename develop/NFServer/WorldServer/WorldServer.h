///--------------------------------------------------------------------
/// 文件名:		WorldServer.h
/// 内  容:		世界服
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef WORLD_SERVER_H
#define WORLD_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "WhiteIP.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NotifyData.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFServer/GameServer/MaintainMsgDataDefine.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CWorldServerState;
class CWorldPlayers;
class CWorldServer : public NFIModule
{
public:
	CWorldServer(NFIPluginManager* p)
	{
		pPluginManager = p;
		m_pWhiteIP = NF_NEW CWhiteIP;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();
	virtual bool Shut();
	virtual bool CommandShut();

	// 本服游戏名称
	void SetServerName(std::string& value) { m_nServerName = value; }
	std::string& GetServerName() { return m_nServerName; }

	// 最小版本号
	void SetClientVersion(int value) { m_nClientVersion = value; }
	int GetClientVersion() const { return m_nClientVersion; }

	// 是否开启白名单
	void SetWhiteIP(bool value) { m_bUseWhiteIP = value; }
	bool GetWhiteIP() const { return m_bUseWhiteIP; }

	//最大在线人数
	void SetPlayersMax(int v) { m_PlayersMax = v; }
	int GetPlayersMax() const { return m_PlayersMax; }

	// 添加白名单
	bool AddWhiteIPList(const char* ip_list);
	// 清除所有白名单地址范围
	bool ClearWhiteIP();
	// 是否在白名单地址范围内
	bool IsWhiteIP(unsigned int ip_addr);

	//取得服务器信息列表
	bool GetServerList(const REQGetServerInfoList& req, ACKGetServerInfoList& ack);

	// 创建服务
	void CreateServer(const NetData& net_data);
	// 添加链接信息
	void ConnectServer(const ConnectCfg& connect_data);
	// 获取某类型IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// 获取某类型Port
	int GetTypePort(ENUM_NET_TYPE type);
	// 添加通知客户端服务
	void NotifyClient(const NotifyData& notify_data);

	// 获取通知客户端服务列表
	vector<NotifyData>& GetNotifyList() { return mxNotifyList; }

	//更新客户端报文
	void UpReport();
private:
	// 服务网络完成
	void OnDBNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnGameNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnProxyNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPubNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 服务OPEN
	void OnDBServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnGameServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnProxyServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPubServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 服务关闭
	void OnProxyServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnGameServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPubServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDBServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// 检查是否完成本身服务
	int CheckServerStatus();

private:
	bool bServerNetComplete = false;

	//本服游戏名称
	std::string m_nServerName = "";

	//最小版本号
	int m_nClientVersion = 0;
	//是否开启白名单
	bool m_bUseWhiteIP = false;
	//最大在线人数限制
	int m_PlayersMax = 3000;

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;
	//通知客户端的服务信息
	vector<NotifyData> mxNotifyList;

	// 白名单信息
	CWhiteIP* m_pWhiteIP;

	NetClientModule* m_pNetClientModule = nullptr;
	CWorldServerState* m_pWorldServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	CWorldPlayers* m_pCWorldPlayers = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
