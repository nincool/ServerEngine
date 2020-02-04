///--------------------------------------------------------------------
/// 文件名:		GameServer.h
/// 内  容:		游戏服
/// 说  明:		
/// 创建日期:	2019年9月28日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/ICreateRoleModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "GameServerState.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CGameServer : public NFIModule
{
public:
	CGameServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	virtual void SendMsgPBToGate(const uint16_t nMsgID, const google::protobuf::Message& xMsg, const NFGUID& self);
	virtual void SendMsgToGate(const uint16_t nMsgID, const std::string& strMsg, const NFGUID& self);
	//发给消息给当前世界所有玩家客户端
	virtual bool SendMsgToClientAll(const uint16_t nMsgID, const google::protobuf::Message& xMsg);

	// 角色名字符最大个数
	void SetRoleNameMaxSize(int value) { m_nRoleNameMaxSize = value; }
	int GetRoleNameMaxSize() { return m_nRoleNameMaxSize; }

	// 角色数据保存事件
	void SetSaveDataTime(int value) { m_nSaveDataTime = value; }
	int GetSaveDataTime() { return m_nSaveDataTime; }

	// 编辑的角色生命时间
	void SetAlterPlayerLifeTime(int value) { m_nAlterPlayerLifeTime = value; }
	int GetAlterPlayerLifeTime() { return m_nAlterPlayerLifeTime; }

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
	void OnReqCloseGameServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadPubComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadGuildComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadGuidNameComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	void OnCommandToGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 客户端网络完成
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// 检查是否完成本身服务
	int CheckServerStatus();

	//网络通信测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;

	// 角色名最大长度
	int m_nRoleNameMaxSize = 10;
	// 角色数据保存心跳时间
	int m_nSaveDataTime = 180000;
	// 编辑角色的生命事件 毫秒
	int m_nAlterPlayerLifeTime = 180000;

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	CGameServerState* m_pGameServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	ICreateRoleModule* m_pCreateRoleModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
