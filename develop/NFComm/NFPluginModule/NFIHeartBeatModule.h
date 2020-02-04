///--------------------------------------------------------------------
/// 文件名:		NFIScheduleModule.h
/// 内  容:		定时器接口
/// 说  明:		
/// 创建日期:	2019年8月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_HEARTBEAT_MODULE_H
#define NFI_HEARTBEAT_MODULE_H

#include <string>
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "KConstDefine.h"

#define STRNAME_LENGTH 255

struct beat_data_t
{
	beat_data_t* pPrev; // 双向链表
	beat_data_t* pNext;
	size_t nSlot;		// 链表序号
	OBJECT_HEARTBEAT_FUNCTOR_PTR pFunc;
	bool bDelete;		//是否已经被删除
	int64_t nTime;
	int64_t nTicks;
	int nMax;
	int nCount;
	NFGUID ObjId;
	char strName[STRNAME_LENGTH];
};

class NFIHeartBeatModule : public  NFIModule
{
public:
	virtual ~NFIHeartBeatModule() {}
	
	/// \brief 添加心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param time 心跳时长
	/// \param count 心跳次数
	/// 注：所加的函数参数是固定的，如下
	/// int func(const NFGUID&, const std::string&, const int, const int) 
	/// time的单位为毫秒
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count) = 0;
	/// \brief 删除心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool RemoveHeartBeat(const NFGUID& obj, const char* func) = 0;
	/// \brief 清除心跳函数
	/// \param obj 对象
	virtual bool ClearHeartBeat(const NFGUID& obj) = 0;
	/// \brief 查找心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool FindHeartBeat(const NFGUID& obj, const char* func) = 0;
	/// \brief 获得心跳列表
	/// \param obj 对象
	virtual bool GetHeartBeatList(const NFGUID& obj, list<beat_data_t*>& beatlist) = 0;
	/// \brief 获得玩家心跳
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual beat_data_t* GetHeartBeat(const NFGUID& obj, const char* func) = 0;

	template<typename BaseType>
	bool AddHeartBeat(const NFGUID self, const char* strScheduleName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const char*, const int, const int), const int nIntervalTime, const int nCount)
	{
		OBJECT_HEARTBEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		OBJECT_HEARTBEAT_FUNCTOR_PTR functorPtr(NF_NEW OBJECT_HEARTBEAT_FUNCTOR(functor));
		return AddHeartBeat(self, strScheduleName, functorPtr, nIntervalTime, nCount);
	}
};

#endif
