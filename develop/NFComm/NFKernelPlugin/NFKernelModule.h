///--------------------------------------------------------------------
/// �ļ���:		NFKernelModule.h
/// ��  ��:		����ģ��
/// ˵  ��:		
/// ��������:	2019��8��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_KERNEL_MODULE_H
#define NF_KERNEL_MODULE_H

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <unordered_map>
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFIRecord.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFPluginModule/NFIEventModule.h"
#include "NFComm/NFPluginModule/NFIResModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFMidWare/NFWordsFilterPlugin/WordsFilterModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class NFKernelModule : public NFIKernelModule
{
public:
	typedef std::unordered_map<NFGUID, NF_SHARE_PTR<NFIObject>> HashObject;
public:
    NFKernelModule(NFIPluginManager* p);
    virtual ~NFKernelModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute();

	virtual bool ExistObject(const NFGUID& ident) const;
	virtual const HashObject& GetObjectAll() const;
    virtual NF_SHARE_PTR<NFIObject> GetObject(const NFGUID& ident) const;
    virtual NF_SHARE_PTR<NFIObject> CreateObject(const std::string& strClassName, const std::string& strConfigID, const NFDataList& args);
	virtual NF_SHARE_PTR<NFIObject> CreateObject(const NFGUID& self, const std::string& strClassName, const std::string& strConfigID, const NFDataList& args, std::function<void(NF_SHARE_PTR<NFIObject>)> pf = nullptr);
	virtual NF_SHARE_PTR<NFIObject> CreateContainer(const NFGUID& self, std::string& strName, int nCap, std::string& strClassName);

	virtual bool DestroyAll();
    virtual bool DestroySelf(const NFGUID& self);
	// ���ٶ��󣬲������ò���, ����ѭ�����ã�ɾ�������ж�
	virtual bool DestroyObject(const NFGUID self);
    virtual bool DestroyObjectAny(const NFGUID self);
	virtual bool DestroyObject(NFObject* pObj);
	virtual bool DestroyPlayer(const NFGUID self);

	// ���󲻿���(ͨ��GetObject��ȡ����)
	virtual bool DisableObject(const NFGUID& self);
	virtual bool EnableObject(const NFGUID& self);
	virtual NF_SHARE_PTR<NFIObject> GetDisableObject(const NFGUID& self);

	virtual NFGUID CreateGUID();

	/// \breif   IsBaseOf �߼���̳й�ϵ�ж�
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName);

	///////////////////////////////////////////////////////////////////������� BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief �������������Ӵ�
	/// \param player ��Ҷ���
	/// \param id �������
	/// \param container ��������
	virtual bool AddViewport(const NFGUID& player, int id, const NFGUID& container);
	/// \brief ���ɾ�������Ӵ�
	/// \param player ��Ҷ���
	/// \param id �������
	virtual bool RemoveViewport(const NFGUID& player, int id);
	/// \brief ��ҵ��Ӵ��Ƿ����
	/// \param player ��Ҷ���
	/// \param id �������
	virtual bool FindViewport(const NFGUID& player, int id);
	/// \brief �������Ӵ���Ӧ������
	/// \param player ��Ҷ���
	/// \param id �������
	virtual const NFGUID& GetViewportContainer(const NFGUID& player, int id);
	/// \brief �����������Ӵ�
	/// \param player ��Ҷ���
	virtual bool ClearViewport(const NFGUID& player);
	/// \brief ȡ�������Ӵ���
	/// \param container ��������
	virtual int GetViewerCount(const NFGUID& container);
	/// \brief �ر������������Ӵ�
	/// \param container ��������
	virtual bool CloseViewers(const NFGUID& container);

	/// \brief �������ƶ���һ��������
	/// \param obj ���ƶ��Ķ���
	/// \param container ��������
	virtual bool Place(const NFGUID& obj, const NFGUID& container);
	/// \brief ������ŵ�������ָ��λ��
	/// \param obj ���ƶ��Ķ���
	/// \param container ��������
	/// \param index �����е�λ�ã���0��ʼ��
	virtual bool PlaceIndex(const NFGUID& obj, const NFGUID& container, int index);
	/// \brief ����֮�佻��ָ��λ���ϵ��Ӷ���
	/// \param container1 ����һ
	/// \param index1 ����һ��λ��
	/// \param container2 ������
	/// \param index2 ��������λ��
	virtual bool Exchange(const NFGUID& container1, int index1, const NFGUID& container2, int index2);
	/// \brief �ƶ�����������ָ��λ��
	/// \param obj ���ƶ��Ķ���
	/// \param newIndex �µ�����λ�ã���0��ʼ��
	virtual bool ChangeIndex(const NFGUID& obj, int newIndex);

	/// \brief ��������������
	/// \param container ��������
	virtual int GetCapacity(const NFGUID& container);
	/// \brief ����
	/// \param container ��������
	virtual int ExtendCapacity(const NFGUID& container, int nCap);
	/// \brief ȡ�����������е�λ��
	/// \param obj ����
	/// \return ��0��ʼ -1��ʾû��
	virtual int GetIndex(const NFGUID& obj);

	/// \brief ȡ����ָ��λ�õ��Ӷ���
	/// \param obj ��������
	/// \param index �����е�λ�ã���0��ʼ��
	virtual const NFGUID& GetChild(const NFGUID& obj, int index);
	/// \brief �����ֲ����Ӷ���
	/// \param obj ��������
	/// \param name �Ӷ��������
	virtual const NFGUID& GetChild(const NFGUID& obj, const std::string& name);
	/// \brief �����Ӷ�������
	/// \param obj ��������
	virtual int GetChildCount(const NFGUID& obj);
	/// \brief ����Ӷ����б�
	/// \param obj ��������
	virtual const std::vector<NFGUID>& GetChildList(const NFGUID& obj);
	/// \brief ��������Ӷ���
	/// \param obj ��������
	virtual bool ClearChild(const NFGUID& obj);

	/// \breif   GetPropertyList ��ȡ����������
	virtual bool GetPropertyList(const NFGUID& obj, NFDataList& args);
	/// \breif   GetRecordList ��ȡ���б���
	virtual bool GetRecordList(const NFGUID& obj, NFDataList& args);

	///////////////////////////////////////////////////////////////////������� END////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////������� BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief �����������
	/// \param obj ����
	/// \param func ����������
	/// \param time ����ʱ��
	/// \param count ��������
	/// ע�����ӵĺ��������ǹ̶��ģ�����
	/// int func(const NFGUID&, const std::string&, const int, const int) 
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
	/// \brief �������������
	/// \param obj ����
	/// \param func ����������
	virtual int GetHeartBeatCount(const NFGUID& obj, const char* func);
	/// \brief ��������������
	/// \param obj ����
	/// \param func ����������
	virtual void SetHeartBeatCount(const NFGUID& obj, const char* func, int nCount);
	/// \brief �������ʱ��
	/// \param obj ����
	/// \param func ����������
	virtual int GetHeartBeatTime(const NFGUID& obj, const char* func);
	///////////////////////////////////////////////////////////////////������� END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////�¼���� BEGIN//////////////////////////////////////////////////////////////////////
	/// \breif   AddEventCallBack ����¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   NFIModule * pOwner �¼��ص�����
	/// \param   EVENT_HOOK pHook �¼��ص�
	/// \param   int nPrior ���ȼ�
	/// \return  bool �Ƿ�ɹ�
	virtual bool AddEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);

	/// \breif   RemoveEventCallBack �Ƴ��¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   void * pOwner �¼��ص�����
	/// \param   EVENT_HOOK pHook �¼��ص�
	/// \return  bool �Ƿ�ɹ�
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);

	/// \breif   RemoveEventCallBack �Ƴ��¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   void * pOwner �¼��ص�����
	/// \return  bool �Ƿ�ɹ�
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner);
	/// \breif   RemoveEventCallBack �Ƴ��¼�����������¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   void * pOwner �¼��ص�����
	/// \return  bool �Ƿ�ɹ�
	virtual bool RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner);
	/// \breif   RemoveEventCallBack �Ƴ��¼�����������¼�
	/// \Access  virtual public 
	/// \param   void * pOwner �¼��ص�����
	/// \return  bool
	virtual bool RemoveEventCallBack(NFIModule* pOwner);

	/// \breif   FindEventCallBack �����¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   void * pOwner �¼��ص�����
	/// \param   EVENT_HOOK pHook �¼��ص�
	/// \return  bool �Ƿ�ɹ�
	virtual bool FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);

	/// \breif   AddCommondCallBack ����ڲ���Ϣ�ص�
	virtual bool AddCommondCallBack(const std::string& strClassName, int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);

	/// \breif   AddExtraCallBack ��Ӹ��ӷ�������Ϣ�ص�
	virtual bool AddExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);
	/// \breif   SendExtraMessage ������Ϣ�����ӷ�����
	virtual bool SendExtraMessage(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const google::protobuf::Message& msg);

	/// \breif   ���ά���˿ڷ�����Ϣ�ص�
	virtual void AddMaintainCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun);
	/// \breif   ������Ϣ��ά���˿�
	virtual bool SendMaintainMessage(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex);

	///////////////////////////////////////////////////////////////////�¼���� END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////����ģ��ӿ� BEGIN////////////////////////////////////////////////////////////////////
	// ��������
	virtual const google::protobuf::Message* LoadRes(const char* szPath, ResReloadFunction pfReload = nullptr);
	/// \breif   ReloadRes �����Ѿ����ع�������
	virtual bool ReloadRes(const char* szPath);
	/// \breif   ReloadResAll ���������Ѿ����ع�������
	virtual bool ReloadResAll();

	/// \breif   ConvertMapData �� pb ����ת��Ϊmap
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName);
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName);

	virtual int64_t GetResInt(const std::string& strConfigID, const std::string& strPropName);
	virtual double GetResFloat(const std::string& strConfigID, const std::string& strPropName);
	virtual const std::string& GetResString(const std::string& strConfigID, const std::string& strPropName);

	//����һ�����д�
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord);

	//�����ַ��������е����д�
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out);
	//���ַ��������е����д��滻 �����´�
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*');

	//����ַ������Ƿ������д�
	virtual bool ExistSensitiveWord(const std::string& text) const;
	///////////////////////////////////////////////////////////////////����ģ��ӿ� END//////////////////////////////////////////////////////////////////////

	// �뿪��Ϸ
	virtual bool PlayerLeaveGame(const NFGUID& player);
	
	// prometheus���ݲɼ�
	virtual prometheus::Family<prometheus::Counter>& GetMetricsCounter(int nTempIndex);
	virtual prometheus::Family<prometheus::Gauge>& GetMetricsGauge(int nTempIndex);
	virtual prometheus::Family<prometheus::Histogram>& GetMetricsHistogram(int nTempIndex);
	virtual prometheus::Family<prometheus::Summary>& GetMetricsSummary(int nTempIndex);
protected:
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb);
    virtual bool RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb);

	virtual bool RegisterClassPropertyEvent(const std::string& strClassName, const PROPERTY_EVENT_FUNCTOR_PTR& cb);
	virtual bool RegisterClassRecordEvent(const std::string& strClassName, const RECORD_EVENT_FUNCTOR_PTR& cb);

protected:
    int OnPropertyCommonEvent(const NFGUID& self, const std::string& strPropertyName, const NFData& oldVar, const NFData& newVar);
    int OnRecordCommonEvent(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar);

protected:

    std::list<NFGUID> mtDeleteSelfList;
    std::list<PROPERTY_EVENT_FUNCTOR_PTR> mtCommonPropertyCallBackList;
    std::list<RECORD_EVENT_FUNCTOR_PTR> mtCommonRecordCallBackList;

	std::map<std::string, std::list<PROPERTY_EVENT_FUNCTOR_PTR>> mtClassPropertyCallBackList;
	std::map<std::string, std::list<RECORD_EVENT_FUNCTOR_PTR>> mtClassRecordCallBackList;

private:
	// ���󼯺�
	HashObject m_hashObject;
	// �����õĶ��󼯺�
	std::map<NFGUID, NF_SHARE_PTR<NFIObject>> m_mapDisableObject;
    NFGUID mnCurExeObject;

    NFIClassModule* m_pClassModule = nullptr;
    NFIElementModule* m_pElementModule = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	NFIEventModule* m_pEventModule = nullptr;
	NFIResModule* m_pResModule = nullptr;
	//���д�
	IWordsFilterModule* m_pWordsFilterModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NetServerInsideModule* m_pNetServerInside = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
