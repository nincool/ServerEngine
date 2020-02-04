///--------------------------------------------------------------------
/// 文件名:		PlatformKernel.h
/// 内  容:		平台服核心模块
/// 说  明:		
/// 创建日期:	2019年10月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __PLATFORM_KERNEL_H
#define __PLATFORM_KERNEL_H
#include "NFComm/NFPluginModule/IPlatformKernel.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "PlatformServerState.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPlatformKernel : public IPlatformKernel
{
public:
	CPlatformKernel(NFIPluginManager* p);
    virtual ~CPlatformKernel();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();

	// 获取共用核心
	virtual NFIKernelModule* CKernel();

	// 获得主辅助对象
	virtual NF_SHARE_PTR<NFIObject> GetPlatformHelper() { return m_pPlatformObject; }

	// 保存平台服对象
	virtual bool SavePlatformData(const NFGUID& object_id);

	// 删除平台服对象
	virtual bool DeletePlatformData(const NFGUID& object_id);

	// 开启战场
	virtual NF_SHARE_PTR<NFIObject> OnStartBattle(const std::string& sBattleName, const NFDataList& args);

	// 发送消息给游戏服GUID
	virtual bool SendCommandToGameGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args);

	// 发送消息给Game
	virtual bool SendCommandToGame(int nCommandId, int world_id, const NFGUID& sendobj, const NFDataList& args);

	// 发送消息给所有Game
	virtual bool SendCommandToAllGame(int nCommandId, const NFGUID& sendobj, const NFDataList& args);

	// 发送消息给战场服
	virtual bool SendCommandToBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& battle_guid, const NFDataList& args);
	
	bool SaveAllPlatformData();
	
private:
	void OnAckStartLoadPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreatePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadPlatformData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadPlatformComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	int SavePlatformDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	// battlehelper销毁完成
	int OnBattleDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	bool OnMapAddIndex(map<NFGUID, std::string>& mapInfo, const NFGUID& key, const std::string& id);

private:
	//对象号存储
	map<NFGUID, std::string> mxMapPlatformGuid;
	// 主辅助对象
	NF_SHARE_PTR<NFIObject> m_pPlatformObject;

	//通用内核模块
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	CPlatformServerState* m_pPlatformServerState = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
