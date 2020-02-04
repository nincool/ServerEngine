///--------------------------------------------------------------------
/// �ļ���:		NFIKernelModule.h
/// ��  ��:		����ģ��ӿ�
/// ˵  ��:		
/// ��������:	2019��8��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_KERNEL_MODULE_H
#define NFI_KERNEL_MODULE_H

#include <iostream>
#include <string>
#include <functional>
#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "KConstDefine.h"
#include "Dependencies/protobuf/src/google/protobuf/message.h"
 
class NFIKernelModule : public NFIModule
{

public:
	virtual ~NFIKernelModule() {};

	template<typename BaseType>
	bool AddPropertyCallBack(const NFGUID& self, const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
	{
		NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
		if (pObject.get())
		{
			return pObject->AddPropertyCallBack(strPropertyName, pBase, handler);
		}

		return false;
	}

    template<typename BaseType>
    bool AddRecordCallBack(const NFGUID& self, const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
    {
        NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
        if (pObject.get())
        {
            return pObject->AddRecordCallBack(strRecordName, pBase, handler);
        }

        return false;
    }

	template<typename BaseType>
	bool AddHeartBeat(const NFGUID self, const char* strHeartBeatName, BaseType* pBase, int (BaseType::* handler)(const NFGUID&, const char*, const int, const int), const int nIntervalTime, const int nCount)
	{
		OBJECT_HEARTBEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		OBJECT_HEARTBEAT_FUNCTOR_PTR functorPtr(NF_NEW OBJECT_HEARTBEAT_FUNCTOR(functor));
		return AddHeartBeat(self, strHeartBeatName, functorPtr, nIntervalTime, nCount);
	}
    
    template<typename BaseType>
    bool RegisterCommonPropertyEvent(BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(NF_NEW PROPERTY_EVENT_FUNCTOR(functor));
        return RegisterCommonPropertyEvent(functorPtr);
    }

    template<typename BaseType>
    bool RegisterCommonRecordEvent(BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(NF_NEW RECORD_EVENT_FUNCTOR(functor));
        return RegisterCommonRecordEvent(functorPtr);
    }

	template<typename BaseType>
	bool RegisterClassPropertyEvent(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
	{
		PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		PROPERTY_EVENT_FUNCTOR_PTR functorPtr(NF_NEW PROPERTY_EVENT_FUNCTOR(functor));
		return RegisterClassPropertyEvent(strClassName, functorPtr);
	}

	template<typename BaseType>
	bool RegisterClassRecordEvent(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
	{
		RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		RECORD_EVENT_FUNCTOR_PTR functorPtr(NF_NEW RECORD_EVENT_FUNCTOR(functor));
		return RegisterClassRecordEvent(strClassName, functorPtr);
	}

	virtual bool ExistObject(const NFGUID& ident) const = 0;

    virtual NF_SHARE_PTR<NFIObject> GetObject(const NFGUID& ident) const = 0;
    /// \breif   CreateObject ��������
    /// \Access  virtual public 
    /// \param   const std::string & strClassName �߼����� �������strConfigID��ȡ
    /// \param   const std::string & strConfigID ����ID ��Ϊ�� ���ҵ����õ�ClassName�����������
    /// \param   const NFDataList & args �趨����ֵ
    /// \return  NF_SHARE_PTR<NFIObject> ���ض���ָ�� ʧ���򷵻�nullptr
    virtual NF_SHARE_PTR<NFIObject> CreateObject(const std::string& strClassName, const std::string& strConfigID, const NFDataList& args) = 0;
	
	// ��������
	// nCap ����
	virtual NF_SHARE_PTR<NFIObject> CreateContainer(const NFGUID& self, std::string& strName, int nCap, std::string& strClassName) = 0;

	virtual bool DestroyObjectAny(const NFGUID self) = 0;
    virtual bool DestroyObject(const NFGUID self) = 0;

    virtual NFGUID CreateGUID() = 0;

	/// \breif   IsBaseOf �߼���̳й�ϵ�ж�
	/// \Access  virtual public 
	/// \param   const std::string & strBaseClassName ������
	/// \param   const std::string & strClassName ��������
	/// \return  bool
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName) = 0;

	///////////////////////////////////////////////////////////////////������� BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief �������������Ӵ�
	/// \param player ��Ҷ���
	/// \param id �������
	/// \param container ��������
	virtual bool AddViewport(const NFGUID& player, int id, const NFGUID& container) = 0;
	/// \brief ���ɾ�������Ӵ�
	/// \param player ��Ҷ���
	/// \param id �������
	virtual bool RemoveViewport(const NFGUID& player, int id) = 0;
	/// \brief ��ҵ��Ӵ��Ƿ����
	/// \param player ��Ҷ���
	/// \param id �������
	virtual bool FindViewport(const NFGUID& player, int id) = 0;
	/// \brief �������Ӵ���Ӧ������
	/// \param player ��Ҷ���
	/// \param id �������
	virtual const NFGUID& GetViewportContainer(const NFGUID& player, int id) = 0;
	/// \brief �����������Ӵ�
	/// \param player ��Ҷ���
	virtual bool ClearViewport(const NFGUID& player) = 0;
	/// \brief ȡ�������Ӵ���
	/// \param container ��������
	virtual int GetViewerCount(const NFGUID& container) = 0;
	/// \brief �ر������������Ӵ�
	/// \param container ��������
	virtual bool CloseViewers(const NFGUID& container) = 0;

	/// \brief �������ƶ���һ��������
	/// \param obj ���ƶ��Ķ���
	/// \param container ��������
	virtual bool Place(const NFGUID& obj, const NFGUID& container) = 0;
	/// \brief ������ŵ�������ָ��λ��
	/// \param obj ���ƶ��Ķ���
	/// \param container ��������
	/// \param index �����е�λ�ã���0��ʼ��
	virtual bool PlaceIndex(const NFGUID& obj, const NFGUID& container, int index) = 0;
	/// \brief ����֮�佻��ָ��λ���ϵ��Ӷ���
	/// \param container1 ����һ
	/// \param index1 ����һ��λ��
	/// \param container2 ������
	/// \param index2 ��������λ��
	virtual bool Exchange(const NFGUID& container1, int index1, const NFGUID& container2, int index2) = 0;
	/// \brief �ƶ�����������ָ��λ��
	/// \param obj ���ƶ��Ķ���
	/// \param newIndex �µ�����λ�ã���0��ʼ��
	virtual bool ChangeIndex(const NFGUID& obj, int newIndex) = 0;

	/// \brief ��������������
	/// \param container ��������
	virtual int GetCapacity(const NFGUID& container) = 0;
	/// \brief ����
	/// \param container ��������
	virtual int ExtendCapacity(const NFGUID& container, int nCap) = 0;
	/// \brief ȡ�����������е�λ��
	/// \param obj ����
	/// \return ��0��ʼ -1��ʾû��
	virtual int GetIndex(const NFGUID& obj) = 0;

	/// \brief ȡ����ָ��λ�õ��Ӷ���
	/// \param obj ��������
	/// \param index �����е�λ�ã���0��ʼ��
	virtual const NFGUID& GetChild(const NFGUID& obj, int index) = 0;
	/// \brief �����ֲ����Ӷ���
	/// \param obj ��������
	/// \param name �Ӷ��������
	virtual const NFGUID& GetChild(const NFGUID& obj, const std::string& name) = 0;
	/// \brief �����Ӷ�������
	/// \param obj ��������
	virtual int GetChildCount(const NFGUID& obj) = 0;
	/// \brief ����Ӷ����б�
	/// \param obj ��������
	virtual const std::vector<NFGUID>& GetChildList(const NFGUID& obj) = 0;
	/// \brief ��������Ӷ���
	/// \param obj ��������
	virtual bool ClearChild(const NFGUID& obj) = 0;

	/// \breif   GetPropertyList ��ȡ����������
	/// \Access  virtual public 
	/// \param   const NFGUID & obj ����
	/// \param   NFDataList & args �������б�
	/// \return  bool
	virtual bool GetPropertyList(const NFGUID& obj, NFDataList& args) = 0;
	/// \breif   GetRecordList ��ȡ���б���
	/// \Access  virtual public 
	/// \param   const NFGUID & obj ����
	/// \param   NFDataList & args �����б�
	/// \return  bool
	virtual bool GetRecordList(const NFGUID& obj, NFDataList& args) = 0;
	///////////////////////////////////////////////////////////////////������� END//////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////������� BEGIN//////////////////////////////////////////////////////////////////////
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
	/// \brief �������������
	/// \param obj ����
	/// \param func ����������
	virtual int GetHeartBeatCount(const NFGUID& obj, const char* func) = 0;
	/// \brief ��������������
	/// \param obj ����
	/// \param func ����������
	virtual void SetHeartBeatCount(const NFGUID& obj, const char* func, int nCount) = 0;
	/// \brief �������ʱ��
	/// \param obj ����
	/// \param func ����������
	virtual int GetHeartBeatTime(const NFGUID& obj, const char* func) = 0;
	///////////////////////////////////////////////////////////////////������� END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////�¼���� BEGIN//////////////////////////////////////////////////////////////////////
	/// \breif   AddEventCallBack ����¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼����� EVENT_OnDefault ΪĬ���¼�������Ӧ�����¼�, �¼����ͺʹ������߼�����ͨ��args{int �¼�����, string �߼�����, ...��������}������
	/// \param   NFIModule * pOwner �¼��ص�����
	/// \param   EVENT_HOOK pHook �¼��ص�
	/// \param   int nPrior ���ȼ�
	/// \return  bool �Ƿ�ɹ�
	virtual bool AddEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;

	/// \breif   RemoveEventCallBack �Ƴ��¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   void * pOwner �¼��ص�����
	/// \param   EVENT_HOOK pHook �¼��ص�
	/// \return  bool �Ƿ�ɹ�
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;

	/// \breif   RemoveEventCallBack �Ƴ��¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   void * pOwner �¼��ص�����
	/// \return  bool �Ƿ�ɹ�
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner) = 0;
	/// \breif   RemoveEventCallBack �Ƴ��¼�����������¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   void * pOwner �¼��ص�����
	/// \return  bool �Ƿ�ɹ�
	virtual bool RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner) = 0;
	/// \breif   RemoveEventCallBack �Ƴ��¼�����������¼�
	/// \Access  virtual public 
	/// \param   void * pOwner �¼��ص�����
	/// \return  bool
	virtual bool RemoveEventCallBack(NFIModule* pOwner) = 0;

	/// \breif   FindEventCallBack �����¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   void * pOwner �¼��ص�����
	/// \param   EVENT_HOOK pHook �¼��ص�
	/// \return  bool �Ƿ�ɹ�
	virtual bool FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;

	/// \breif   AddCommondCallBack ����ڲ���Ϣ�ص�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   int nMsgId ��Ϣ�� 0ΪĬ����Ϣ������Ӧ������Ϣ����ϢIdͨ��args{int ��ϢId, string �߼�����, ...��������}
	/// \param   NFIModule * pOwner �ص�����
	/// \param   EVENT_HOOK pHook �ص�����
	/// \param   int nPrior ���ȼ�
	/// \return  bool �Ƿ�ɹ�
	virtual bool AddCommondCallBack(const std::string& strClassName, int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;

	/// \breif   AddExtraCallBack ��Ӹ��ӷ�������Ϣ�ص�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   NF_SERVER_TYPES eServerType ����������
	/// \param   int nMsgId ��ϢID
	/// \param   NF_SHARE_PTR<IMethodMsg> pMethod ��Ϣ�ص�
	/// \return  bool �Ƿ�ɹ�
	virtual bool AddExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;

	/// \breif   SendExtraMessage ������Ϣ�����ӷ�����
	/// \Access  virtual public 
	/// \param   NF_SERVER_TYPES eServerType ���ӷ���������
	/// \param   int nMsgId	��ϢID	
	/// \param   const NFGUID & self ������
	/// \param   const google::protobuf::Message & msg ����
	/// \return  bool �Ƿ�ɹ�
	virtual bool SendExtraMessage(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const google::protobuf::Message& msg) = 0;

	/// \breif   AddMaintainCallBack ���ά���˿ڷ�����Ϣ�ص�
	/// \Access  virtual public 
	/// \param   int nMsgId ��ϢID
	/// \param   NFIModule * pBase �ص�����
	/// \param   NetMsgFun fun �ص�����
	virtual void AddMaintainCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun) = 0;

	/// \breif   SendMaintainMessage ������Ϣ��ά���˿�
	/// \Access  virtual public 
	/// \param   int nMsgId ��ϢID
	/// \param   string & xData ����
	/// \param   NFSOCK nSockIndex Ŀ������
	virtual bool SendMaintainMessage(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex) = 0;

	///////////////////////////////////////////////////////////////////�¼���� END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////���ܴ��� BEGIN////////////////////////////////////////////////////////////////////
	/// \breif   Load ���������ļ�, ��������������
	/// \Access  virtual public
	/// \param   const char * szPath �����ļ�����·��
	/// \param   std::function<void(void)> pfReload = nullptr ����ʱ�����õĻص� Ĭ�Ͽ�
	/// \return  google::protobuf::Message* ��Ϣ����ָ��
	virtual const google::protobuf::Message* LoadRes(const char* szPath, ResReloadFunction pfReload = nullptr) = 0;

	/// \breif   ReloadRes �����Ѿ����ع�������
	/// \Access  virtual public 
	/// \param   const char * szPath �����ļ�����·��
	/// \return  bool �Ƿ�ɹ�
	virtual bool ReloadRes(const char* szPath) = 0;

	/// \breif   ReloadResAll ���������Ѿ����ع�������
	/// \Access  virtual public 
	/// \return  bool
	virtual bool ReloadResAll() = 0;

	/// \breif   ConvertMapData �� pb ����ת��Ϊmap
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName) = 0;
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName) = 0;

	// ��ȡ�����ļ��У��߼�������ֵ
	virtual int64_t GetResInt(const std::string& strConfigID, const std::string& strPropName) = 0;
	virtual double GetResFloat(const std::string& strConfigID, const std::string& strPropName) = 0;
	virtual const std::string& GetResString(const std::string& strConfigID, const std::string& strPropName) = 0;

	//����һ�����д�
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord) = 0;

	//�����ַ��������е����д�
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out) = 0;

	//���ַ��������е����д��滻 �����´�
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*') = 0;

	//����ַ������Ƿ������д�
	virtual bool ExistSensitiveWord(const std::string& text) const = 0;
	///////////////////////////////////////////////////////////////////���ܴ��� END//////////////////////////////////////////////////////////////////////

	/// \breif   PlayerLeaveGame �뿪��Ϸ
	/// \Access  virtual protected 
	/// \param   const NFGUID & player ���
	/// \return  bool
	virtual bool PlayerLeaveGame(const NFGUID& player) = 0;

	// prometheus���ݲɼ�
	virtual prometheus::Family<prometheus::Counter>& GetMetricsCounter(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Gauge>& GetMetricsGauge(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Histogram>& GetMetricsHistogram(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Summary>& GetMetricsSummary(int nTempIndex) = 0;
protected:

	/// \brief �����������
	/// \param obj ����
	/// \param func ����������
	/// \param time ����ʱ��
	/// \param count ��������
	/// ע�����ӵĺ��������ǹ̶��ģ�����
	/// int func(const NFGUID&, const std::string&, const int, const int) 
	/// time�ĵ�λΪ����
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count) = 0;
	///////////////////////////////////////////////////////////////////�¼���� END//////////////////////////////////////////////////////////////////////

protected:
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
	
	virtual bool RegisterClassPropertyEvent(const std::string& strClassName, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
	virtual bool RegisterClassRecordEvent(const std::string& strClassName, const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
};

#endif
