///--------------------------------------------------------------------
/// 文件名:		NFHeartBeatModule.cpp
/// 内  容:		定时器插件
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFHeartBeatModule.h"
#include "../NFUtils/NFTool.hpp"
#include "../NFPublic/Debug.h"
#include "../NFCore/NFDateTime.hpp"
#include "../NFCore/NFObject.h"
#include "../NFCore/NFPlayer.h"
#include <crtdbg.h>
#include <cmath>
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"

bool NFHeartBeatModule::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pPerformanceCount = pPluginManager->FindModule<PerformanceCountModule>();
	return true;
}

bool NFHeartBeatModule::AfterInit()
{
	m_pKernelModule->AddEventCallBack(LC::IObject::ThisName(), EVENT_OnDestroy, METHOD_ARGS(NFHeartBeatModule::OnDestroy));

	return true;
}

bool NFHeartBeatModule::ReadyExecute()
{
	nLastTime = NFGetTimeMS();
	return true;
}

bool NFHeartBeatModule::Execute()
{
	// 轮次事件检测一次
	NFINT64 nNowTime = NFGetTimeMS();
	if (nLastTime + BEATRUN > nNowTime)
	{
		return true;
	}

	for (size_t i = 0; i < m_nSlotSize; ++i)
	{
		// 这个指针可能失效
		beat_data_t* tail = m_pBeatSlots[i].pTail;
		beat_data_t* p = m_pBeatSlots[i].pHead;

		while (p)
		{
			if (p->bDelete)
			{
				if (p == tail)
				{
					// 已经遍历结束
					break;
				}

				p = p->pNext;
				continue;
			}

			int64_t tick = NFGetTimeMS();
			if ((tick - p->nTicks) < p->nTime)
			{
				// 不再需要扫描队列后面的心跳
				break;
			}

			int slice = tick - p->nTicks;
			p->nTicks = tick;

			NFPerformance pPerformance;

			OBJECT_HEARTBEAT_FUNCTOR* pFunc = p->pFunc.get();
			pFunc->operator()(p->ObjId, p->strName, slice, p->nCount);
			if (pPerformance.CheckTimePoint(5))
			{
				QLOG_WARING_S << "[HeartBeat callback performance]"
					<< " consume:" << int(pPerformance.TimeScope()) << "ms"
					<< " name:" << p->strName;
			}
			m_pPerformanceCount->Recording(0, p->strName, pPerformance.TimeScope());

			CATCH_BEGIN
			auto& histogram_family = pPluginManager->GetMetricsHistogram(6);
			vector<double> vecTemp;
			if (pPluginManager->GetHistogramConfig(6, vecTemp))
			{
				auto& second_histogram = histogram_family.Add({ {"Func", p->strName} }, vecTemp);
				second_histogram.Observe((double)pPerformance.TimeScope());
			}
			CATCH_END

			//p->bDelete 避免逻辑层在回调中删除心跳
			if (p->nMax > 0 && !p->bDelete)
			{
				// 已达到设定的心跳次数
				if (++p->nCount >= p->nMax)
				{
					RemoveBeatInfo(p->ObjId, p->strName);

					// 移除心跳
					DeleteBeat(p);
				}
			}

			beat_data_t* t = p;
			p = p->pNext;

			// 如果不止一个成员
			if (m_pBeatSlots[i].pHead != m_pBeatSlots[i].pTail)
			{
				// 将心跳移到队列的最后
				SlotRemove(m_pBeatSlots + i, t);
				SlotAppend(m_pBeatSlots + i, t);
			}

			if (t == tail)
			{
				beat_data_t* end = m_pBeatSlots[i].pTail;
				// 已经遍历结束
				break;
			}
		}
	}

	// 清除已删除的心跳
	if (!m_pDeleteBeatPool.empty())
	{
		const size_t delete_num = m_pDeleteBeatPool.size();
		for (size_t i = 0; i < delete_num; ++i)
		{
			beat_data_t* p = m_pDeleteBeatPool[i];
			Assert(p->bDelete);

			SlotRemove(m_pBeatSlots + p->nSlot, p);
			delete p;
		}

		m_pDeleteBeatPool.clear();
	}

	return true;
}

NFHeartBeatModule::NFHeartBeatModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

NFHeartBeatModule::~NFHeartBeatModule()
{
	m_MapObjBeat.clear();
	m_pDeleteBeatPool.clear();

	for (int i = 0; i < m_nSlotSize; ++i)
	{
		beat_data_t* p = m_pBeatSlots[i].pHead;
		if (p)
		{
			beat_data_t* t = p;
			p = p->pNext;
			delete t;
			t = nullptr;
		}
	}

	if (m_pBeatSlots)
	{
		delete[] m_pBeatSlots;
	}
}

int NFHeartBeatModule::OnDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	this->ClearHeartBeat(self);

	return 0;
}

bool NFHeartBeatModule::AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count)
{
	beat_data_t* pFind = FindBeat(obj, func);
	if (pFind)
	{
		QLOG_WARING << __FUNCTION__ << " already exists beat:" << func
			<< " obj:" << obj;
		return false;
	}

	size_t name_len = strlen(func);
	if (name_len >= STRNAME_LENGTH)
	{
		QLOG_WARING << __FUNCTION__ << " func:" << func
			<< "name_len too long:" << name_len;
		return false;
	}

	beat_data_t* p = NF_NEW beat_data_t;
	if (p == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " p == NULL";
		return false;
	}

	p->pFunc = cb;
	p->nTime = time;
	p->nMax = count;
	p->nCount = 0;
	p->nTicks = NFGetTimeMS();
	p->ObjId = obj;
	p->bDelete = false;
	p->pNext = nullptr;
	p->pPrev = nullptr;
	memcpy(p->strName, func, name_len + 1);

	AddToSlot(p, true);

	AddBeatInfo(p->ObjId, p);

	return true;
}

bool NFHeartBeatModule::RemoveHeartBeat(const NFGUID& obj, const char* func)
{
	Assert(!obj.IsNull());
	Assert(func != nullptr);

	beat_data_t* p = FindBeat(obj, func);
	if (nullptr == p)
	{
		return false;
	}

	DeleteBeat(p);

	RemoveBeatInfo(obj, func);

	return true;
}

bool NFHeartBeatModule::ClearHeartBeat(const NFGUID& obj)
{
	unordered_map<NFGUID, list<beat_data_t*>>::iterator iterFind = m_MapObjBeat.find(obj);
	if (iterFind == m_MapObjBeat.end())
	{
		return false;
	}

	list<beat_data_t*>& listBeatInfo = iterFind->second;
	list<beat_data_t*>::iterator iterBeat = listBeatInfo.begin();
	while (iterBeat != listBeatInfo.end())
	{
		// 删除排序心跳
		DeleteBeat(*iterBeat);
		++iterBeat;
	}

	listBeatInfo.clear();
	m_MapObjBeat.erase(iterFind);

	return true;
}

bool NFHeartBeatModule::FindHeartBeat(const NFGUID& obj, const char* func)
{
	Assert(!obj.IsNull());
	Assert(func != nullptr);

	return (FindBeat(obj, func) != nullptr);
}

bool NFHeartBeatModule::GetHeartBeatList(const NFGUID& obj, list<beat_data_t*>& beatlist)
{
	unordered_map<NFGUID, list<beat_data_t*>>::iterator iterFind = m_MapObjBeat.find(obj);
	if (iterFind == m_MapObjBeat.end())
	{
		return false;
	}

	beatlist = iterFind->second;

	return true;
}

beat_data_t* NFHeartBeatModule::GetHeartBeat(const NFGUID& obj, const char* func)
{
	Assert(!obj.IsNull());
	Assert(func != nullptr);

	return FindBeat(obj, func);
}

void NFHeartBeatModule::DeleteBeat(beat_data_t* p)
{
	p->bDelete = true;
	m_pDeleteBeatPool.push_back(p);
}

void NFHeartBeatModule::SlotAppend(slot_data_t* pSlot, beat_data_t* p)
{
	Assert(pSlot != nullptr);
	Assert(p != nullptr);

	if (pSlot->pHead == nullptr)
	{
		pSlot->pHead = p;
	}

	if (pSlot->pTail)
	{
		pSlot->pTail->pNext = p;
	}

	p->pPrev = pSlot->pTail;
	p->pNext = nullptr;
	pSlot->pTail = p;
}

void NFHeartBeatModule::SlotInsert(slot_data_t* pSlot, beat_data_t* p)
{
	Assert(pSlot != nullptr);
	Assert(p != nullptr);

	p->pPrev = nullptr;
	p->pNext = nullptr;

	if (nullptr == pSlot->pHead)
	{
		// 头部插入
		pSlot->pHead = p;
		pSlot->pTail = p;
		return;
	}

	beat_data_t* pNode = pSlot->pHead;

	int64_t curTickCount = NFGetTimeMS();
	while (pNode != nullptr)
	{
		//用逝去时间做比较可以解决无符号反转问题，逝去时间多的放前面
		if (curTickCount - pNode->nTicks < curTickCount - p->nTicks)
		{
			// 插入有序的时间
			p->pNext = pNode;

			if (pNode->pPrev != nullptr)
			{
				pNode->pPrev->pNext = p;
				p->pPrev = pNode->pPrev;
			}
			else
			{
				pSlot->pHead = p;
			}

			pNode->pPrev = p;

			return;
		}

		pNode = pNode->pNext;
	}

	// 从尾部插入
	if (pSlot->pTail != nullptr)
	{
		pSlot->pTail->pNext = p;
	}

	p->pPrev = pSlot->pTail;
	p->pNext = nullptr;
	pSlot->pTail = p;
}

void NFHeartBeatModule::SlotRemove(slot_data_t* pSlot, beat_data_t* p)
{
	Assert(pSlot != nullptr);
	Assert(p != nullptr);

	if (pSlot->pHead == p)
	{
		pSlot->pHead = p->pNext;
	}

	if (pSlot->pTail == p)
	{
		pSlot->pTail = p->pPrev;
	}

	if (p->pNext)
	{
		p->pNext->pPrev = p->pPrev;
	}

	if (p->pPrev)
	{
		p->pPrev->pNext = p->pNext;
	}
}

void NFHeartBeatModule::AddToSlot(beat_data_t* p, bool bInsert)
{
	// 获得列表
	slot_data_t* pSlot = nullptr;
	size_t slot_size = m_nSlotSize;
	slot_data_t* pBeatSlots = m_pBeatSlots;
	int64_t beat_time = p->nTime;

	for (size_t k = 0; k < slot_size; ++k)
	{
		// 优先使用时间相等的列表
		if (pBeatSlots[k].nSlice == beat_time)
		{
			pSlot = &pBeatSlots[k];
			break;
		}

		// 可使用空的列表
		if (pBeatSlots[k].pHead == nullptr)
		{
			pSlot = &pBeatSlots[k];
			pSlot->nSlice = beat_time;
			pSlot->pTail = nullptr;
			break;
		}
	}

	// 增加一个新的列表
	if (nullptr == pSlot)
	{
		if (m_nSlotSize == m_nCapacity)
		{
			Expand();
		}

		pSlot = m_pBeatSlots + m_nSlotSize;
		pSlot->nSlice = p->nTime;
		pSlot->pHead = nullptr;
		pSlot->pTail = nullptr;
		++m_nSlotSize;
	}

	p->nSlot = pSlot - m_pBeatSlots;

	if (bInsert)
	{
		SlotInsert(pSlot, p);
	}
	else
	{
		SlotAppend(pSlot, p);
	}
}

void NFHeartBeatModule::Expand()
{
	const size_t capacity = m_nCapacity * 2 + 1;
	slot_data_t* p = NF_NEW slot_data_t[capacity];

	memset(p, 0, sizeof(slot_data_t) * capacity);

	if (m_pBeatSlots)
	{
		memcpy(p, m_pBeatSlots, sizeof(slot_data_t) * m_nSlotSize);
		delete[] m_pBeatSlots;
	}

	m_pBeatSlots = p;
	m_nCapacity = capacity;
}

void NFHeartBeatModule::AddBeatInfo(const NFGUID& pid, beat_data_t* p)
{
	unordered_map<NFGUID, list<beat_data_t*>>::iterator iterFind = m_MapObjBeat.find(pid);
	if (iterFind != m_MapObjBeat.end())
	{
		iterFind->second.push_back(p);
	}
	else
	{
		list<beat_data_t*> m_pBeatList;
		m_pBeatList.push_back(p);
		m_MapObjBeat.insert(make_pair(pid, m_pBeatList));
	}
}

void NFHeartBeatModule::RemoveBeatInfo(const NFGUID& pid, const char* beat_name)
{
	Assert(beat_name != nullptr);
	unordered_map<NFGUID, list<beat_data_t*>>::iterator iterFind = m_MapObjBeat.find(pid);
	if (iterFind == m_MapObjBeat.end())
	{
		return;
	}

	list<beat_data_t*>& listBeatInfo = iterFind->second;
	list<beat_data_t*>::iterator iterBeat = listBeatInfo.begin();
	while (iterBeat != listBeatInfo.end())
	{
		if (strcmp((*iterBeat)->strName, beat_name) == 0)
		{
			listBeatInfo.erase(iterBeat);
			break;
		}

		++iterBeat;
	}

	if (listBeatInfo.empty())
	{
		m_MapObjBeat.erase(iterFind);
	}
}

beat_data_t* NFHeartBeatModule::FindBeat(const NFGUID& obj, const char* beat_name)
{
	unordered_map<NFGUID, list<beat_data_t*>>::iterator iterFind = m_MapObjBeat.find(obj);
	if (iterFind == m_MapObjBeat.end())
	{
		return nullptr;
	}

	list<beat_data_t*>& listBeatInfo = iterFind->second;
	list<beat_data_t*>::iterator iterP = listBeatInfo.begin();
	for (; iterP != listBeatInfo.end(); ++iterP)
	{
		if (strcmp((*iterP)->strName, beat_name) == 0)
		{
			return *iterP;
		}
	}

	return nullptr;
}
