///--------------------------------------------------------------------
/// �ļ���:		NFIClassModule.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��8��3��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _NF_I_CLASS_MODULE_H_
#define _NF_I_CLASS_MODULE_H_

#include "NFIModule.h"
#include "NFIClass.h"
#include "KConstDefine.h"

//�߼������
class NFIClassModule 
	: public NFIModule
	, public NFMapEx<std::string, NFIClass>
{
public:
    virtual ~NFIClassModule() {}

    virtual NF_SHARE_PTR<NFIPropertyManager> GetClassPropertyManager(const std::string& strClassName) = 0;
    virtual NF_SHARE_PTR<NFIRecordManager> GetClassRecordManager(const std::string& strClassName) = 0;

	/// \breif   AddEventCallBack ����¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
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

	/// \breif   RunEventCallBack ִ���¼�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   EventType eType �¼�����
	/// \param   const NFGUID & self ��Ӧ�¼�����
	/// \param   const NFGUID & sender ��Ӧ�¼�����ض���
	/// \param   const NFDataList & args ����
	/// \return  int ����0 �ɹ� ����ֵ���վ����¼�����
	virtual int RunEventCallBack(const std::string& strClassName, EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;	
	
	/// \breif   RunCommandCallBack ִ���ڲ���Ϣ�ص�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   int nMsgId ��Ϣ��
	/// \param   const NFGUID & self ������Ϣ����
	/// \param   const NFGUID & sender ������Ϣ����
	/// \param   const NFDataList & args ����
	/// \return  int 
	virtual int RunCommandCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;

	/// \breif   RunCustomCallBack ִ�пͻ�����Ϣ�ص�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   int nMsgId ��Ϣ��
	/// \param   const NFGUID & self ������Ϣ����
	/// \param   const NFGUID & sender ������Ϣ����
	/// \param   const std::string& strMsg ��ϢPB������(��������Ϣͷ) 
	/// \return  int 
	virtual int RunCustomCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;

	/// \breif   RunExtraCallBack ִ�и��ӷ�������Ϣ�ص�
	/// \Access  virtual public 
	/// \param   const std::string & strClassName �߼�����
	/// \param   NF_SERVER_TYPES eServerType
	/// \param   int nMsgId
	/// \param   const NFGUID & self
	/// \param   const std::string & strMsg
	/// \return  int
	virtual int RunExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;

	/// \breif   OpenEvent �����¼�
	virtual void OpenEvent() = 0;
	/// \breif   CloseEvent �ر��¼�
	virtual void CloseEvent() = 0;
	/// \breif   IsOpenEvent ��ȡ�¼��Ƿ���
	virtual bool IsOpenEvent() = 0;

	/// \breif   IsBaseOf �߼���̳й�ϵ�ж�
	/// \Access  virtual public 
	/// \param   const std::string & strBaseClassName ������
	/// \param   const std::string & strClassName ��������
	/// \return  bool
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName) = 0;
};

#endif // _NF_I_CLASS_MODULE_H_