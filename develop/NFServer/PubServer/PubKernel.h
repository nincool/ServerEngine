///--------------------------------------------------------------------
/// 文件名:		PubKernel.h
/// 内  容:		公共域核心模块
/// 说  明:		
/// 创建日期:	2019年8月26日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __PUBKERNEL_H
#define __PUBKERNEL_H

#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/IPubKernel.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "PubServerState.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

// 公共数据状态
enum PUB_STATE_ENUM
{
	PUBSTATE_UNKNOWN,	// 初始状态
	PUBSTATE_SAVING,	// 保存中
};

// 公共数据
struct PubData
{
	int nState = PUBSTATE_UNKNOWN; // 状态
	int nSavingSec = 0; // 保存等待秒数
};

class CPubKernel : public IPubKernel
{
public:
	CPubKernel(NFIPluginManager* p);
    virtual ~CPubKernel();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();
	
	// 获取共用核心
	virtual NFIKernelModule* CKernel();

	// 创建域名
	virtual NF_SHARE_PTR<NFIObject> CreatePub(const std::string& pub_name, bool bSave = false);
	// 创建公会
	virtual NF_SHARE_PTR<NFIObject> CreateGuild(const std::string& guild_name);
	// 删除公共域
	virtual bool DeletePub(const std::string& pub_name);
	// 删除公会
	virtual bool DeleteGuild(const std::string& guild_name);
	// 获得公共GUID
	virtual const NFGUID& GetPubGuid(const std::string& name);
	// 获得公会GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name);
	// 手动保存域数据
	virtual bool SavePubData(const std::string& name, const NFGUID& object_id);
	// 手动保存公会数据
	virtual bool SaveGuildData(const NFGUID& object_id);
	// Message事件给玩家
	virtual bool CommandToPlayer(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg);
	// Message给辅助对象
	virtual bool CommandToGame(int nCommandId, const NFGUID& sendobj, const NFDataList& msg);
	// Command事件给内部对象
	virtual bool Command(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg);
	// Command事件给辅助对象
	virtual bool CommandToHelper(int nCommandId, const NFGUID& sendobj, const NFDataList& msg);

	// 保存所有Pub数据
	bool SaveAllPubData();

	// 获取缓存对象数量
	int GetSavePubCount() { return m_mapPubInfo.size(); }
private:
	void OnReqSynServerInfo(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadGuildData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadPubData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckSaveObjectData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// guild加载完成
	int OnGuildRecover(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// pub加载完成
	int OnPubRecover(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	void OnCommandTransmit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	int SavePubDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	int SaveGuildDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);

	// 添加数据
	bool OnMapAddIndex(map<std::string, NFGUID>& mapInfo, const std::string& key, const NFGUID& id);
	// 请求同步给逻辑端对象数据
	void SynDataToGame(const NFGUID& id, const int nMsgID);
	// 显示所有数据
	void SynAllMapData(map<std::string, NFGUID>& mapValue, const int nMsgID);
	// 下发所有Guid数据
	void SynAllGuidInfo(map<std::string, NFGUID>& mapInfo, const int nMsgID);

	bool RemoveObject(const NFGUID& self);
	bool InnerSavePubData(const std::string& name, const NFGUID& object_id, bool bExit = false);
	bool InnerSaveGuildData(const NFGUID& object_id, bool bExit = false);
private:
	//对象号存储
	map<std::string, NFGUID> mxMapPubGuid;
	map<std::string, NFGUID> mxMapGuildGuid;

	NFINT64 nLastTime = 0;
	std::map<NFGUID, PubData> m_mapPubInfo;
	//即将被删除的数据
	std::list<NFGUID> m_listRemovePub;

	//通用内核模块
	NFIKernelModule* m_pKernelModule = nullptr;
	//ClientNet
	NetClientModule* m_pNetClientModule = nullptr;
	//心跳
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	CPubServerState* m_pPubServerState = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
