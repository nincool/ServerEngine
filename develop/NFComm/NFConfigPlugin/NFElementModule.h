///--------------------------------------------------------------------
/// �ļ���:		NFElementModule.h
/// ��  ��:		Ԫ�ؽӿ�
/// ˵  ��:		�߼����Ӧ�Ķ�������
/// ��������:	2019��8��3��
/// �޸���:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_ELEMENT_MODULE_H
#define NF_ELEMENT_MODULE_H

#include <map>
#include <string>
#include <iostream>
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "Dependencies/RapidXML/rapidxml_iterators.hpp"
#include "Dependencies/RapidXML/rapidxml_print.hpp"
#include "Dependencies/RapidXML/rapidxml_utils.hpp"
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFList.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFRecord.h"
#include "NFComm/NFCore/NFPropertyManager.h"
#include "NFComm/NFCore/NFRecordManager.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"

class NFClass;
class ElementConfigInfo
{
public:
    ElementConfigInfo()
    {
        m_pPropertyManager = NF_SHARE_PTR<NFIPropertyManager>(NF_NEW NFPropertyManager(NFGUID()));
        m_pRecordManager = NF_SHARE_PTR<NFIRecordManager>(NF_NEW NFRecordManager(NFGUID()));
    }

    virtual ~ElementConfigInfo()
    {
    }

    NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager()
    {
        return m_pPropertyManager;
    }

    NF_SHARE_PTR<NFIRecordManager> GetRecordManager()
    {
        return m_pRecordManager;
    }

protected:
    NF_SHARE_PTR<NFIPropertyManager> m_pPropertyManager;
    NF_SHARE_PTR<NFIRecordManager> m_pRecordManager;
};

class NFElementModule: public NFIElementModule, NFMapEx<std::string, ElementConfigInfo>
{
public:
    NFElementModule(NFIPluginManager* p);
    virtual ~NFElementModule();

	virtual bool Awake();
    virtual bool Init();
    virtual bool Shut();

    virtual bool AfterInit();
    virtual bool BeforeShut();
    virtual bool Execute();

	virtual bool Load();
	virtual bool Save();
    virtual bool Clear();

	// ����߼�������
	void AddClassData(const google::protobuf::Message& msg, std::string& path, bool bReload = false);

	virtual bool ExistElement(const std::string& strConfigName);
    virtual bool ExistElement(const std::string& strClassName, const std::string& strConfigName);

    virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager(const std::string& strConfigName);
    virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager(const std::string& strConfigName);

    virtual NFINT64 GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName);
    virtual double GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName);
	virtual const std::string& GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName);

	virtual const std::vector<std::string> GetListByProperty(const std::string& strClassName, const std::string& strPropertyName, const NFINT64 nValue);
	virtual const std::vector<std::string> GetListByProperty(const std::string& strClassName, const std::string& strPropertyName, const std::string& nValue);

protected:
    virtual NF_SHARE_PTR<NFIProperty> GetProperty(const std::string& strConfigName, const std::string& strPropertyName);

	// ���һ������
	void AddElementData(const google::protobuf::Message& msg, std::string& path, bool bReload = false);

	// ��ʱ������ȡxml�ӿڣ���ʹ��protobuf��Ŀ���Ǽ���Server.xml,ʹ����ά�޸ķ��������ø�����һ��
    virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pLogicClass);
	virtual bool LegalNumber(const char* str);
	virtual bool LegalFloat(const char* str);

protected:
    NFIClassModule* m_pClassModule = nullptr;
    bool mbLoaded;
};

#endif
