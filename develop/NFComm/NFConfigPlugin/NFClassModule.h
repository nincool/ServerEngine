///--------------------------------------------------------------------
/// �ļ���:		NFClassModule.h
/// ��  ��:		�߼������ģ��
/// ˵  ��:		
/// ��������:	2019��8��8��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_LOGICCLASS_MODULE_H
#define NF_LOGICCLASS_MODULE_H

#include <string>
#include <map>
#include <vector>
#include "NFElementModule.h"
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFList.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFRecord.h"
#include "NFComm/NFCore/NFPropertyManager.h"
#include "NFComm/NFCore/NFRecordManager.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

//����������������
class NFClassModule : public NFIClassModule
{
public:
    NFClassModule(NFIPluginManager* p);
    virtual ~NFClassModule();
	
	virtual bool Awake();
    virtual bool Init();
    virtual bool Shut();
    virtual bool Load();

    virtual NF_SHARE_PTR<NFIPropertyManager> GetClassPropertyManager(const std::string& strClassName);
    virtual NF_SHARE_PTR<NFIRecordManager> GetClassRecordManager(const std::string& strClassName);

	// ����߼���
    virtual bool AddClass(const std::string& strClassName, const std::string& strParentName);

	// ����¼�
	virtual bool AddEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);
	// �Ƴ��¼�
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner);
	virtual bool RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner);
	virtual bool RemoveEventCallBack(NFIModule* pOwner);
	// �����¼�
	virtual bool FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	// ִ���¼�
	virtual int RunEventCallBack(const std::string& strClassName, EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// ִ���ڲ���Ϣ�ص�
	virtual int RunCommandCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// ִ�пͻ�����Ϣ�ص�
	virtual int RunCustomCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const std::string& strMsg);
	/// \breif   RunExtraCallBack ִ�и��ӷ�������Ϣ�ص�
	virtual int RunExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg);
	/// \breif   OpenEvent �����¼�
	virtual void OpenEvent();
	/// \breif   CloseEvent �ر��¼�
	virtual void CloseEvent();
	/// \breif   IsOpenEvent ��ȡ�¼��Ƿ���
	virtual bool IsOpenEvent();
	/// \breif   IsBaseOf �߼���̳й�ϵ�ж�
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName);
protected:
	virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pParentClass);

    virtual NFDATA_TYPE ComputerType(const char* pstrTypeName, NFData& var);
    virtual bool AddPropertys(rapidxml::xml_node<>* pPropertyRootNode, NF_SHARE_PTR<NFIClass> pClass);
    virtual bool AddRecords(rapidxml::xml_node<>* pRecordRootNode, NF_SHARE_PTR<NFIClass> pClass);
    virtual bool AddClassInclude(const char* pstrClassFilePath, NF_SHARE_PTR<NFIClass> pClass);
    virtual bool AddClass(const char* pstrClassFilePath, NF_SHARE_PTR<NFIClass> pClass);

	std::string GetNodeValue(rapidxml::xml_attribute<>* pAttr);

protected:
    NFIElementModule* m_pElementModule = nullptr;

    std::string m_strLogicCalssPath = "";

	// �¼��Ƿ���
	bool m_bOpenEvent = true;
};

#endif
