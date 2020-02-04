///--------------------------------------------------------------------
/// 文件名:		PlayerRedis.h
/// 内  容:		玩家数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PLAYER_REDIS_MODULE_H
#define _PLAYER_REDIS_MODULE_H

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "ObjectRedis.h"
#include "CommonRedis.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "DBServer.h"

class CPlayerRedis : public CObjectRedis
{
public:
	CPlayerRedis(NFIPluginManager* p) : CObjectRedis(p)
	{
	}

	virtual bool Init();
	virtual bool AfterInit();

	virtual bool ExistPlayerName(const std::string& strPlayerName, int nServerID);
	//在数据库创建角色
	virtual OuterMsg::EGameEventCode CreateRole(OuterMsg::ReqCreateRole& createRoleData, int nServerID);
	//改名
	virtual bool Rename(const std::string& strAccount, const NFGUID& self, const std::string& oldName, const std::string& newName, int nServerID);
	
protected:
	void OnRequireRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPreCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnSaveRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//改名
	void OnRename(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 获取玩家PB数据
	void OnGetPlayerData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 获取所有玩家ID与名称
	void OnGetPlayerGuidName(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 获取需要明文保存的数据
	bool GetFrankData(const OuterMsg::ObjectDataPack& xMsg, string_vector& vec_key, string_vector& vec_value);
private:
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetClientModule* m_pNetClient = nullptr;
	CDBServer* m_pDBServer = nullptr;
};

#endif
