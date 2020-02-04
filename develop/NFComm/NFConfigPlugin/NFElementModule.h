///--------------------------------------------------------------------
/// 文件名:		NFElementModule.h
/// 内  容:		元素接口
/// 说  明:		逻辑类对应的对象数据
/// 创建日期:	2019年8月3日
/// 修改人:		谢宇
/// 版权所有:	血帆海盗团
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

	// 添加逻辑类数据
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

	// 添加一行数据
	void AddElementData(const google::protobuf::Message& msg, std::string& path, bool bReload = false);

	// 暂时保留读取xml接口，不使用protobuf的目的是加载Server.xml,使得运维修改服务器配置更方便一点
    virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pLogicClass);
	virtual bool LegalNumber(const char* str);
	virtual bool LegalFloat(const char* str);

protected:
    NFIClassModule* m_pClassModule = nullptr;
    bool mbLoaded;
};

#endif
