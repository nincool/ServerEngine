///--------------------------------------------------------------------
/// �ļ���:		NFHeartBeatModule.h
/// ��  ��:		��ʱ�����
/// ˵  ��:		
/// ��������:	2019��8��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
	int64_t nSlice;	// ����ʱ��
	beat_data_t* pHead;	// ͷָ��
	beat_data_t* pTail;	// βָ��
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

	/// \brief �����������
	/// \param obj ����
	/// \param func ����������
	/// \param time ����ʱ��
	/// \param count ��������
	/// ע�����ӵĺ��������ǹ̶��ģ�����
	/// int func(const NFGUID&, const std::string&, const float, const int) 
	/// time�ĵ�λΪ����
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count);
	/// \brief ɾ����������
	/// \param obj ����
	/// \param func ����������
	virtual bool RemoveHeartBeat(const NFGUID& obj, const char* func);
	/// \brief �����������
	/// \param obj ����
	virtual bool ClearHeartBeat(const NFGUID& obj);
	/// \brief ������������
	/// \param obj ����
	/// \param func ����������
	virtual bool FindHeartBeat(const NFGUID& obj, const char* func);
	/// \brief ��������б�
	/// \param obj ����
	virtual bool GetHeartBeatList(const NFGUID& obj, list<beat_data_t*>& beatlist);
	/// \brief ����������
	/// \param obj ����
	/// \param func ����������
	virtual beat_data_t* GetHeartBeat(const NFGUID& obj, const char* func);

private:
	int OnDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �������������Ϣ
	void AddBeatInfo(const NFGUID& obj, beat_data_t* p);
	// ɾ���������
	void RemoveBeatInfo(const NFGUID& obj, const char* beat_name);
	// �����������
	beat_data_t* FindBeat(const NFGUID& obj, const char* beat_name);
	// ��������
	void Expand();
	// ɾ������
	void DeleteBeat(beat_data_t* p);
	// ����ĩβ���
	void SlotAppend(slot_data_t* pSlot, beat_data_t* p);
	// �����в���
	void SlotInsert(slot_data_t* pSlot, beat_data_t* p);
	// ������ɾ��
	void SlotRemove(slot_data_t* pSlot, beat_data_t* p);
	// ��λ�������
	void AddToSlot(beat_data_t* p, bool bInsert);
protected:
	NFIKernelModule* m_pKernelModule = nullptr;
	PerformanceCountModule* m_pPerformanceCount = nullptr;
private:
	// ��ҵĶ�ʱ���б�
	unordered_map<NFGUID, list<beat_data_t*>> m_MapObjBeat;
	// ����Ͱ
	slot_data_t* m_pBeatSlots = nullptr;
	// ���ɾ���������
	std::vector<beat_data_t*> m_pDeleteBeatPool;
	// ��ǰ�������
	size_t m_nCapacity = 0;
	// ��ǰ����
	size_t m_nSlotSize = 0;
	// �ϴ�ʱ��
	NFINT64 nLastTime = 0;
};

#endif