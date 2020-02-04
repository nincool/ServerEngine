///--------------------------------------------------------------------
/// 文件名:		NFHeartBeatModule.h
/// 内  容:		定时器插件
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_HEARTBEAT_MODULE_H
#define NF_HEARTBEAT_MODULE_H

#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "PerformanceCountModule.h"

#define BEATRUN 100

struct slot_data_t
{
	int64_t nSlice;	// 心跳时长
	beat_data_t* pHead;	// 头指针
	beat_data_t* pTail;	// 尾指针
};

class NFKernelModule;
class NFHeartBeatModule : public NFIHeartBeatModule
{
public:
	NFHeartBeatModule(NFIPluginManager* p);

	virtual ~NFHeartBeatModule();

	virtual bool Init();
	virtual bool AfterInit();
	virtual bool Execute();
	virtual bool ReadyExecute();

	/// \brief 添加心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param time 心跳时长
	/// \param count 心跳次数
	/// 注：所加的函数参数是固定的，如下
	/// int func(const NFGUID&, const std::string&, const float, const int) 
	/// time的单位为毫秒
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count);
	/// \brief 删除心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool RemoveHeartBeat(const NFGUID& obj, const char* func);
	/// \brief 清除心跳函数
	/// \param obj 对象
	virtual bool ClearHeartBeat(const NFGUID& obj);
	/// \brief 查找心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool FindHeartBeat(const NFGUID& obj, const char* func);
	/// \brief 获得心跳列表
	/// \param obj 对象
	virtual bool GetHeartBeatList(const NFGUID& obj, list<beat_data_t*>& beatlist);
	/// \brief 获得玩家心跳
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual beat_data_t* GetHeartBeat(const NFGUID& obj, const char* func);

private:
	int OnDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 增加玩家心跳信息
	void AddBeatInfo(const NFGUID& obj, beat_data_t* p);
	// 删除玩家心跳
	void RemoveBeatInfo(const NFGUID& obj, const char* beat_name);
	// 查找玩家心跳
	beat_data_t* FindBeat(const NFGUID& obj, const char* beat_name);
	// 扩充容器
	void Expand();
	// 删除心跳
	void DeleteBeat(beat_data_t* p);
	// 链表末尾添加
	void SlotAppend(slot_data_t* pSlot, beat_data_t* p);
	// 链表中插入
	void SlotInsert(slot_data_t* pSlot, beat_data_t* p);
	// 链表中删除
	void SlotRemove(slot_data_t* pSlot, beat_data_t* p);
	// 定位链表并添加
	void AddToSlot(beat_data_t* p, bool bInsert);
protected:
	NFIKernelModule* m_pKernelModule = nullptr;
	PerformanceCountModule* m_pPerformanceCount = nullptr;
private:
	// 玩家的定时器列表
	unordered_map<NFGUID, list<beat_data_t*>> m_MapObjBeat;
	// 心跳桶
	slot_data_t* m_pBeatSlots = nullptr;
	// 添加删除缓存队列
	std::vector<beat_data_t*> m_pDeleteBeatPool;
	// 当前最大容量
	size_t m_nCapacity = 0;
	// 当前容器
	size_t m_nSlotSize = 0;
	// 上次时间
	NFINT64 nLastTime = 0;
};

#endif