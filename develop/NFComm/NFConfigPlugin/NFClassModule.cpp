///--------------------------------------------------------------------
/// 文件名:		NFClassModule.cpp
/// 内  容:		逻辑类管理模块
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <time.h>
#include <algorithm>
#include "NFConfigPlugin.h"
#include "NFClassModule.h"
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "Dependencies/RapidXML/rapidxml_print.hpp"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFClass.h"

NFClassModule::NFClassModule(NFIPluginManager* p)
{
	pPluginManager = p;

	m_strLogicCalssPath = "NFDataCfg/Struct/LogicClass.xml";

	QLOG_SYSTEM_S << "Load xml [" << pPluginManager->GetConfigPath() + m_strLogicCalssPath << "]";
}

NFClassModule::~NFClassModule()
{
	ClearAll();
}

bool NFClassModule::Awake()
{
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();

    Load();
	
	return true;
}

bool NFClassModule::Init()
{
	return true;
}

bool NFClassModule::Shut()
{
    ClearAll();

    return true;
}

bool NFClassModule::Load()
{
	std::string strFile = pPluginManager->GetConfigPath() + m_strLogicCalssPath;
	std::string strContent;
	pPluginManager->GetFileContent(strFile, strContent);

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());

	rapidxml::xml_node<>* root = xDoc.first_node();
	for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
	{
		Load(attrNode, NULL);
	}

	return true;
}

bool NFClassModule::Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pParentClass)
{
	std::string strLogicClassName = GetNodeValue(attrNode->first_attribute("Id"));
	if (strLogicClassName.empty())
	{
		std::string msg = strLogicClassName + " Id not find";
		NFASSERT(0, msg, __FILE__, __FUNCTION__);
		return false;
	}

	std::string strPath = GetNodeValue(attrNode->first_attribute("Path"));
	std::string strInstancePath = GetNodeValue(attrNode->first_attribute("InstancePath"));

	std::string strType = GetNodeValue(attrNode->first_attribute("Type"));
	OuterMsg::ObjectType nObjectType = PBConvert::ToObjectType(strType);
	if (nObjectType == OuterMsg::OBJECT_TYPE_UNKNOW)
	{
		if (pParentClass == nullptr || pParentClass->GetType() == OuterMsg::OBJECT_TYPE_UNKNOW)
		{
			std::string msg = strLogicClassName + " error type";
			NFASSERT(0, msg, __FILE__, __FUNCTION__);
			return false;
		}

		nObjectType = pParentClass->GetType();
	}

	NF_SHARE_PTR<NFIClass> pClass(NF_NEW NFClass(pPluginManager, strLogicClassName));
	AddElement(strLogicClassName, pClass);
	pClass->SetParent(pParentClass);
	pClass->SetType(nObjectType);
	pClass->SetInstancePath(strInstancePath);

	// 添加逻辑类
	AddClass(strPath.c_str(), pClass);

	// 加载子类
	for (rapidxml::xml_node<>* pDataNode = attrNode->first_node(); pDataNode; pDataNode = pDataNode->next_sibling())
	{
		Load(pDataNode, pClass);
	}

	return true;
}

NFDATA_TYPE NFClassModule::ComputerType(const char* pstrTypeName, NFData& var)
{
    if (0 == strcmp(pstrTypeName, "int"))
    {
        var.SetInt(NULL_INT);
        return var.GetType();
    }
    else if (0 == strcmp(pstrTypeName, "string"))
    {
        var.SetString(NULL_STR);
        return var.GetType();
    }
    else if (0 == strcmp(pstrTypeName, "float"))
    {
        var.SetFloat(NULL_FLOAT);
        return var.GetType();
    }
    else if (0 == strcmp(pstrTypeName, "object"))
    {
        var.SetObject(NULL_OBJECT);
        return var.GetType();
    }
	else if (0 == strcmp(pstrTypeName, "bytes"))
	{
		var = NFData(TDATA_BYTES);
		var.SetString(NULL_STR);
		return var.GetType();
	}

    return TDATA_UNKNOWN;
}

bool NFClassModule::AddPropertys(rapidxml::xml_node<>* pPropertyRootNode, NF_SHARE_PTR<NFIClass> pClass)
{
    for (rapidxml::xml_node<>* pPropertyNode = pPropertyRootNode->first_node(); pPropertyNode; pPropertyNode = pPropertyNode->next_sibling())
    {
        if (pPropertyNode)
        {
            const char* strPropertyName = pPropertyNode->first_attribute("Id")->value();
			// 尝试删除之前的，目的是覆盖父类的定义
			pClass->GetPropertyManager()->RemoveElement(strPropertyName);

            const char* pstrType = pPropertyNode->first_attribute("Type")->value();
            const char* pstrPublic = pPropertyNode->first_attribute("Public")->value();
            const char* pstrPrivate = pPropertyNode->first_attribute("Private")->value();
            const char* pstrSave = pPropertyNode->first_attribute("Save")->value();

            bool bPublic = lexical_cast<bool>(pstrPublic);
            bool bPrivate = lexical_cast<bool>(pstrPrivate);
            bool bSave = lexical_cast<bool>(pstrSave);

            NFData varProperty;
            if (TDATA_UNKNOWN == ComputerType(pstrType, varProperty))
            {
                NFASSERT(0, strPropertyName, __FILE__, __FUNCTION__);
            }

            NF_SHARE_PTR<NFIProperty> xProperty = pClass->GetPropertyManager()->AddProperty(NFGUID(), strPropertyName, varProperty.GetTypeEx());
            xProperty->SetPublic(bPublic);
            xProperty->SetPrivate(bPrivate);
            xProperty->SetSave(bSave);
        }
    }

    return true;
}

bool NFClassModule::AddRecords(rapidxml::xml_node<>* pRecordRootNode, NF_SHARE_PTR<NFIClass> pClass)
{
    for (rapidxml::xml_node<>* pRecordNode = pRecordRootNode->first_node(); pRecordNode;  pRecordNode = pRecordNode->next_sibling())
    {
        if (pRecordNode)
        {
            const char* pstrRecordName = pRecordNode->first_attribute("Id")->value();
			// 尝试删除之前的，目的是覆盖父类的定义
			pClass->GetRecordManager()->RemoveElement(pstrRecordName);

            const char* pstrRow = pRecordNode->first_attribute("Row")->value();
            const char* pstrCol = pRecordNode->first_attribute("Col")->value();

            const char* pstrPublic = pRecordNode->first_attribute("Public")->value();
            const char* pstrPrivate = pRecordNode->first_attribute("Private")->value();
            const char* pstrSave = pRecordNode->first_attribute("Save")->value();

            bool bPublic = lexical_cast<bool>(pstrPublic);
            bool bPrivate = lexical_cast<bool>(pstrPrivate);
            bool bSave = lexical_cast<bool>(pstrSave);

			NF_SHARE_PTR<NFDataList> recordVar(NF_NEW NFDataList());
			NF_SHARE_PTR<NFDataList> recordTag(NF_NEW NFDataList());

            for (rapidxml::xml_node<>* recordColNode = pRecordNode->first_node(); recordColNode;  recordColNode = recordColNode->next_sibling())
            {
                NFData TData;
                const char* pstrColType = recordColNode->first_attribute("Type")->value();
                if (TDATA_UNKNOWN == ComputerType(pstrColType, TData))
                {
                    NFASSERT(0, pstrRecordName, __FILE__, __FUNCTION__);
                }

                recordVar->Add(TData);
            }

			NF_SHARE_PTR<NFRecord> xRecord =
				dynamic_pointer_cast<NFRecord>(
					pClass->GetRecordManager()->AddRecord(NFGUID(),
						pstrRecordName,
						recordVar,
						atoi(pstrRow)));

			xRecord->SetPublic(bPublic);
            xRecord->SetPrivate(bPrivate);
            xRecord->SetSave(bSave);
        }
    }

    return true;
}

bool NFClassModule::AddClassInclude(const char* pstrClassFilePath, NF_SHARE_PTR<NFIClass> pClass)
{
    if (pstrClassFilePath == nullptr
		|| pstrClassFilePath[0] == '\0'
		|| pClass->Find(pstrClassFilePath))
    {
        return false;
    }

    std::string strFile = pPluginManager->GetConfigPath() + pstrClassFilePath;
	std::string strContent;
	pPluginManager->GetFileContent(strFile, strContent);

	rapidxml::xml_document<> xDoc;
    xDoc.parse<0>((char*)strContent.c_str());

    //support for unlimited layer class inherits
    rapidxml::xml_node<>* root = xDoc.first_node();
	if (root == nullptr)
	{
		string sBuffer = "NFClassModule::AddClassInclude" + strFile;
		NFASSERT(0, sBuffer.c_str(), __FILE__, __FUNCTION__);
		return false;
	}

    rapidxml::xml_node<>* pRropertyRootNode = root->first_node("Propertys");
    if (pRropertyRootNode)
    {
        AddPropertys(pRropertyRootNode, pClass);
    }

    //and record
    rapidxml::xml_node<>* pRecordRootNode = root->first_node("Records");
    if (pRecordRootNode)
    {
        AddRecords(pRecordRootNode, pClass);
    }

    rapidxml::xml_node<>* pIncludeRootNode = root->first_node("Includes");
    if (pIncludeRootNode)
    {
        for (rapidxml::xml_node<>* includeNode = pIncludeRootNode->first_node(); includeNode; includeNode = includeNode->next_sibling())
        {
            const char* pstrIncludeFile = includeNode->first_attribute("Path")->value();

            if (AddClassInclude(pstrIncludeFile, pClass))
            {
                pClass->Add(pstrIncludeFile);
            }
        }
    }

    return true;
}

bool NFClassModule::AddClass(const char* pstrClassFilePath, NF_SHARE_PTR<NFIClass> pClass)
{
    NF_SHARE_PTR<NFIClass> pParent = pClass->GetParent();
    while (pParent)
    {
        //inherited some properties form class of parent
        std::string strFileName = "";
        pParent->First(strFileName);
        while (!strFileName.empty())
        {
            if (pClass->Find(strFileName))
            {
                strFileName.clear();
                continue;
            }

            if (AddClassInclude(strFileName.c_str(), pClass))
            {
                pClass->Add(strFileName);
            }

            strFileName.clear();
            pParent->Next(strFileName);
        }

        pParent = pParent->GetParent();
    }

    if (AddClassInclude(pstrClassFilePath, pClass))
    {
        pClass->Add(pstrClassFilePath);
    }

    return true;
}

bool NFClassModule::AddClass(const std::string& strClassName, const std::string& strParentName)
{
    NF_SHARE_PTR<NFIClass> pParentClass = GetElement(strParentName);
    NF_SHARE_PTR<NFIClass> pChildClass = GetElement(strClassName);
    if (!pChildClass)
    {
        pChildClass = NF_SHARE_PTR<NFIClass>(NF_NEW NFClass(pPluginManager, strClassName));
        AddElement(strClassName, pChildClass);

        if (pParentClass)
        {
            pChildClass->SetParent(pParentClass);
        }
    }

    return true;
}

NF_SHARE_PTR<NFIPropertyManager> NFClassModule::GetClassPropertyManager(const std::string& strClassName)
{
    NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
    if (pClass)
    {
        return pClass->GetPropertyManager();
    }

    return NULL;
}

NF_SHARE_PTR<NFIRecordManager> NFClassModule::GetClassRecordManager(const std::string& strClassName)
{
    NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
    if (pClass)
    {
        return pClass->GetRecordManager();
    }

    return NULL;
}

std::string NFClassModule::GetNodeValue(rapidxml::xml_attribute<>* pAttr)
{
	return (pAttr == nullptr || pAttr->value() == nullptr) ? "" : pAttr->value();
}

bool NFClassModule::AddEventCallBack(
	const std::string& strClassName,
	EventType eType, 
	NFIModule* pOwner, 
	EVENT_HOOK pHook, 
	int nPrior/* = 0*/)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->AddEventCallBack(eType, pOwner, pHook, nPrior);
}

bool NFClassModule::RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->RemoveEventCallBack(eType, pOwner, pHook);
}

bool NFClassModule::RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->RemoveEventCallBack(eType, pOwner);
}

bool NFClassModule::RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->RemoveEventCallBack(pOwner);
}

bool NFClassModule::RemoveEventCallBack(NFIModule* pOwner)
{
	NF_SHARE_PTR<NFIClass> pClass = First();
	while (pClass != nullptr)
	{
		pClass->RemoveEventCallBack(pOwner);
		pClass = Next();
	}

	return true;
}

bool NFClassModule::FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->FindEventCallBack(eType, pOwner, pHook);
}

int NFClassModule::RunEventCallBack(
	const std::string& strClassName, 
	EventType eType, 
	const NFGUID& self, 
	const NFGUID& sender, 
	const NFDataList& args)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return 0;
	}

	return pClass->RunEventCallBack(eType, self, sender, args);
}

int NFClassModule::RunCommandCallBack(
	const std::string& strClassName, 
	int nMsgId, 
	const NFGUID& self, 
	const NFGUID& sender, 
	const NFDataList& args)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return 0;
	}

	return pClass->RunCommandCallBack(nMsgId, self, sender, args);
}

int NFClassModule::RunCustomCallBack(
	const std::string& strClassName, 
	int nMsgId, 
	const NFGUID& self, 
	const std::string& strMsg)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return 0;
	}

	return pClass->RunCustomCallBack(nMsgId, self, strMsg);
}

int NFClassModule::RunExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg)
{
	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return 0;
	}

	return pClass->RunExtraCallBack(eServerType, nMsgId, self, strMsg);
}

void NFClassModule::OpenEvent()
{
	m_bOpenEvent = true;
}

void NFClassModule::CloseEvent()
{
	m_bOpenEvent = false;
}

bool NFClassModule::IsOpenEvent()
{
	return m_bOpenEvent;
}

bool NFClassModule::IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName)
{
	if (strBaseClassName.empty() || strClassName.empty())
	{
		return false;
	}

	NF_SHARE_PTR<NFIClass> pClass = GetElement(strClassName);
	if (pClass == nullptr)
	{
		return false;
	}

	while (true)
	{
		auto pParent = pClass->GetParent();
		if (pParent == nullptr)
		{
			return false;
		}

		if (pParent->GetClassName() == strBaseClassName)
		{
			return true;
		}

		pClass = pParent;
	}

	return false;
}
