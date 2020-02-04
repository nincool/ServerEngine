///--------------------------------------------------------------------
/// 文件名:		RankRedis.h
/// 内  容:		排行榜数据库
/// 说  明:		
/// 创建日期:	2019年9月26日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __RANK_REDIS_MODULE_H__
#define __RANK_REDIS_MODULE_H__

#include "NFServer/DBSever/CommonRedis.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "RankDBServer.h"
#include "../DBSever/ColdData.h"

class RankRedis : public NFIModule
{
public:
	RankRedis(NFIPluginManager* p);
	
	virtual bool Init();
	virtual bool AfterInit();

	// 初始化
	void OnRankInit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 创建排行榜
	void OnRankCreate(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 删除排行榜
	void OnRankDelete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 更新榜单数据
	void OnRankUpdate(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 设置数据
	void OnRankSetData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 获取数据
	void OnRankGetData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 批量获取数据
	void OnRankGetDataArray(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 获取排名数据
	void OnRankGetRankData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 获取范围数据
	void OnRankGetRangeData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	ICommonRedis* m_pCommonRedis = nullptr;
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CRankDBServer* m_pDBServer = nullptr;
	CColdData* m_pColdData = nullptr;

	// 是否已经反馈初始化的数据
	int m_nInit = 0;
};

#endif // __RANK_REDIS_MODULE_H__
