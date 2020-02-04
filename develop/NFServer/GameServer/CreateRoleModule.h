
///--------------------------------------------------------------------
/// 文件名:		CreateRoleModule.h
/// 内  容:		角色创建
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _CREATEROLE_MODULE_H
#define _CREATEROLE_MODULE_H

#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/ICreateRoleModule.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "GameServer.h"
#include "NFComm/NFPluginModule/IGameKernel.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

// 玩家连接信息
struct PlayerConnInfo
{
	int nGateID = 0;
	NFGUID xClientID = NULL_OBJECT;
	std::string strIP = "";
	std::string strAccount = "";
	std::string strDeviceMac = "";
	std::string strDeviceID = "";
	int nOsType = 0;
};

struct AlterPlayerInfo
{
	std::function<void (const NFGUID& self, const NFDataList& args)> fp;
	NFDataList args;// 附带的参数
	int64_t dnDeleteTime = 0; // 超时删除时间
}; 

class CCreateRoleModule : public ICreateRoleModule
{
public:
	CCreateRoleModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual ~CCreateRoleModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

	// 获取连接信息
	const PlayerConnInfo* GetConnInfo(const NFGUID& player) const;
	// 获取所有连接信息
	const std::map<NFGUID, PlayerConnInfo>& GetConnInfoMap() const;

	// 编辑玩家数据
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp);

protected:
	void OnReqiureRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReposeRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreateRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckPreCreateRole(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnClienEnterGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDBLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnClientEnterGameFinishProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCommandPlayerRemove(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCommandPlayerOffline(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 加载角色
	bool LoadRole(const OuterMsg::ObjectDataPack& xMsg);

	// 玩家创建事件
	int OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// 编辑玩家数据
	virtual void OnAlterPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	virtual bool EnableAlterPlayer(const NFGUID& player);
	virtual bool DisableAlterPlayer(const NFGUID& player);
	// 定时保存编辑玩家数据
	int CB_AlterPlayerSaveData(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	// 定时清除编辑玩家
	int CB_DestroyAlterPlayer(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);

private:
	// 删除玩家连接信息
	bool RemoveConnInfo(const NFGUID& nRoleID);
	// 获取玩家存储数据
	bool GetData(const NFGUID& self, OuterMsg::ObjectDataPack& xRoleDataPack);
	// 存储玩家数据
	void SaveData(const NFGUID& self, bool bExit = false);
	// 定时存储玩家数据
	int SaveDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	// 通知world玩家上线
	void SendOnline(const NFGUID& self);
	// 玩家建立连接
	bool PlayerConnect(const NFGUID& self);
	// 玩家断开连接
	bool PlayerDisconnect(const NFGUID& self);
private:
	std::map<NFGUID, PlayerConnInfo> m_mapConnInfo;
	std::map<NFGUID, std::list<AlterPlayerInfo>> m_mapAlterPlayer;

	NFIClassModule* m_pClassModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;
	NFKernelModule* m_pKernelModule = nullptr;
	IGameKernel* m_pGameKernel = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif