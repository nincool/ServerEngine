///--------------------------------------------------------------------
/// �ļ���:		NFIScheduleModule.h
/// ��  ��:		��ʱ���ӿ�
/// ˵  ��:		
/// ��������:	2019��8��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
	beat_data_t* pPrev; // ˫������
	beat_data_t* pNext;
	size_t nSlot;		// �������
	OBJECT_HEARTBEAT_FUNCTOR_PTR pFunc;
	bool bDelete;		//�Ƿ��Ѿ���ɾ��
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
	
	/// \brief �����������
	/// \param obj ����
	/// \param func ����������
	/// \param time ����ʱ��
	/// \param count ��������
	/// ע�����ӵĺ��������ǹ̶��ģ�����
	/// int func(const NFGUID&, const std::string&, const int, const int) 
	/// time�ĵ�λΪ����
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count) = 0;
	/// \brief ɾ����������
	/// \param obj ����
	/// \param func ����������
	virtual bool RemoveHeartBeat(const NFGUID& obj, const char* func) = 0;
	/// \brief �����������
	/// \param obj ����
	virtual bool ClearHeartBeat(const NFGUID& obj) = 0;
	/// \brief ������������
	/// \param obj ����
	/// \param func ����������
	virtual bool FindHeartBeat(const NFGUID& obj, const char* func) = 0;
	/// \brief ��������б�
	/// \param obj ����
	virtual bool GetHeartBeatList(const NFGUID& obj, list<beat_data_t*>& beatlist) = 0;
	/// \brief ����������
	/// \param obj ����
	/// \param func ����������
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
