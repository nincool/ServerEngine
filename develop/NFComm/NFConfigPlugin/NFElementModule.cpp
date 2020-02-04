///--------------------------------------------------------------------
/// 文件名:		NFElementModule.cpp
/// 内  容:		元素
/// 说  明:		逻辑类对应的对象数据
/// 创建日期:	2019年8月19日
/// 修改人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <algorithm>
#include <ctype.h>
#include "NFConfigPlugin.h"
#include "NFElementModule.h"
#include "NFClassModule.h"
#include "../NFCore/NFProperty.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "../NFUtils/QLOG.h"
NFElementModule::NFElementModule(NFIPluginManager* p)
{
	pPluginManager = p;
	mbLoaded = false;
}

NFElementModule::~NFElementModule()
{

}

bool NFElementModule::Awake()
{
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

	Load();
	return true;
}

bool NFElementModule::Init()
{
	return true;
}

bool NFElementModule::Shut()
{
	Clear();
	return true;
}

void NFElementModule::AddClassData(const google::protobuf::Message& msg, std::string& path, bool bReload/* = false*/)
{
	auto* pDesc = msg.GetDescriptor();
	if (pDesc == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pDesc == NULL";
		return;
	}

	auto* pReflection = msg.GetReflection();
	if (pReflection == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pReflection == NULL";
		return;
	}

	int nCount = pDesc->field_count();
	for (int i = 0; i < nCount; ++i)
	{
		auto* pField = pDesc->field(i);
		if (pField == nullptr)
		{
			continue;
		}
		if (pField->is_repeated())
		{
			int nRepeatedCount = pReflection->FieldSize(msg, pField);
			for (int j = 0; j < nRepeatedCount; ++j)
			{
				const auto& pRepeatedMsg = pReflection->GetRepeatedMessage(msg, pField, j);
				AddElementData(pRepeatedMsg, path, bReload);
			}
		}
	}
}

void NFElementModule::AddElementData(const google::protobuf::Message& msg, std::string& path, bool bReload/* = false*/)
{
	auto* pDesc = msg.GetDescriptor();
	if (pDesc == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pDesc == NULL file:" << path;
		return;
	}

	auto* pReflection = msg.GetReflection();
	if (pReflection == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pReflection == NULL file:" << path;
		return;
	}

	auto* pClassName = pDesc->FindFieldByName("ClassName");
	if (pClassName == nullptr)
	{
		return;
	}

	auto* pConfigID = pDesc->FindFieldByName("ConfigID");
	if (pConfigID == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pConfigID == NULL file:" << path;
		return;
	}

	std::string strClassName = pReflection->GetString(msg, pClassName);
	if (strClassName.empty())
	{
		QLOG_WARING << __FUNCTION__ << " strClassName empty file:" << path;
		return;
	}

	NF_SHARE_PTR<NFIClass> pClass = m_pClassModule->GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL file:" << path;
		return;
	}

	NF_SHARE_PTR<NFIPropertyManager> pClassPropertyManager = pClass->GetPropertyManager();
	if (pClassPropertyManager == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClassPropertyManager == NULL file:" << path;
		return;
	}

	std::string strConfigID = pReflection->GetString(msg, pConfigID);
	if (strConfigID.empty())
	{
		QLOG_WARING << __FUNCTION__ << " ConfigID empty name:" << strClassName << " file:" << path;
		return;
	}

	if (bReload)
	{
		RemoveElement(strConfigID);
	}
	else if (ExistElement(strConfigID))
	{
		QLOG_WARING << __FUNCTION__ << " ConfigID:" << strConfigID
			<< " already exist name:" << strClassName
			<< " file:" << path;
		return;
	}

	NF_SHARE_PTR<ElementConfigInfo> pElementInfo(NF_NEW ElementConfigInfo());
	AddElement(strConfigID, pElementInfo);
	pClass->AddId(strConfigID);
	NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = pElementInfo->GetPropertyManager();
	if (pPropertyManager == nullptr)
	{
		NFASSERT(0, "error PropertyManager", __FILE__, __FUNCTION__);
		return;
	}

	int nFieldCount = pDesc->field_count();
	for (int i = 0; i < nFieldCount; ++i)
	{
		auto* pField = pDesc->field(i);
		if (pField == nullptr)
		{
			continue;
		}

		const std::string& strName = pField->name();
		if (strName.empty())
		{
			continue;
		}

		NF_SHARE_PTR<NFIProperty> pClassProperty = pClassPropertyManager->GetElement(strName);
		if (pClassProperty == nullptr)
		{
			// 对象不需要此属性
			continue;
		}

		NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->AddProperty(NFGUID(), pClassProperty);
		if (pProperty == nullptr)
		{
			NFASSERT(0, "error property", __FILE__, __FUNCTION__);
			continue;
		}

		auto nType = pField->type();
		switch (nType)
		{
		case google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
			pProperty->SetFloat(pReflection->GetDouble(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
			pProperty->SetFloat(pReflection->GetFloat(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_INT64:
		case google::protobuf::FieldDescriptor::Type::TYPE_SFIXED64:
		case google::protobuf::FieldDescriptor::Type::TYPE_SINT64:
			pProperty->SetInt(pReflection->GetInt64(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_UINT64:
		case google::protobuf::FieldDescriptor::Type::TYPE_FIXED64:
			pProperty->SetInt(pReflection->GetUInt64(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_INT32:
		case google::protobuf::FieldDescriptor::Type::TYPE_SFIXED32:
		case google::protobuf::FieldDescriptor::Type::TYPE_SINT32:
			pProperty->SetInt(pReflection->GetInt32(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_UINT32:
		case google::protobuf::FieldDescriptor::Type::TYPE_FIXED32:
			pProperty->SetInt(pReflection->GetUInt32(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_BOOL:
			pProperty->SetInt(pReflection->GetBool(msg, pField) ? 1 : 0);
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_STRING:
		case google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
			pProperty->SetString(pReflection->GetString(msg, pField));
			break;
		}
	}
}

bool NFElementModule::Save()
{
	return true;
}

NFINT64 NFElementModule::GetPropertyInt(const std::string& strConfigName, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIProperty> pProperty = GetProperty(strConfigName, strPropertyName);
	if (pProperty)
	{
		return pProperty->GetInt();
	}

	return 0;
}

double NFElementModule::GetPropertyFloat(const std::string& strConfigName, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIProperty> pProperty = GetProperty(strConfigName, strPropertyName);
	if (pProperty)
	{
		return pProperty->GetFloat();
	}

	return 0.0;
}

const std::string& NFElementModule::GetPropertyString(const std::string& strConfigName, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIProperty> pProperty = GetProperty(strConfigName, strPropertyName);
	if (pProperty)
	{
		return pProperty->GetString();
	}

	return  NULL_STR;
}

const std::vector<std::string> NFElementModule::GetListByProperty(const std::string& strClassName, const std::string& strPropertyName, NFINT64 nValue)
{
	std::vector<std::string> xList;

	NF_SHARE_PTR<NFIClass> xClass = m_pClassModule->GetElement(strClassName);
	if (nullptr != xClass)
	{
		const std::vector<std::string>& xElementList = xClass->GetIDList();
		for (int i = 0; i < xElementList.size(); ++i)
		{
			const std::string& strConfigID = xElementList[i];
			NFINT64 nElementValue = GetPropertyInt(strConfigID, strPropertyName);
			if (nValue == nElementValue)
			{
				xList.push_back(strConfigID);
			}
		}
	}

	return xList;
}

const std::vector<std::string> NFElementModule::GetListByProperty(const std::string& strClassName, const std::string& strPropertyName, const std::string& nValue)
{
	std::vector<std::string> xList;

	NF_SHARE_PTR<NFIClass> xClass = m_pClassModule->GetElement(strClassName);
	if (nullptr != xClass)
	{
		const std::vector<std::string>& xElementList = xClass->GetIDList();
		for (int i = 0; i < xElementList.size(); ++i)
		{
			const std::string& strConfigID = xElementList[i];
			const std::string& strElementValue = GetPropertyString(strConfigID, strPropertyName);
			if (nValue == strElementValue)
			{
				xList.push_back(strConfigID);
			}
		}
	}

	return xList;
}

NF_SHARE_PTR<NFIProperty> NFElementModule::GetProperty(const std::string& strConfigName, const std::string& strPropertyName)
{
	NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
	if (pElementInfo)
	{
		return pElementInfo->GetPropertyManager()->GetElement(strPropertyName);
	}

	return NULL;
}

NF_SHARE_PTR<NFIPropertyManager> NFElementModule::GetPropertyManager(const std::string& strConfigName)
{
	NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
	if (pElementInfo)
	{
		return pElementInfo->GetPropertyManager();
	}

	return NULL;
}

NF_SHARE_PTR<NFIRecordManager> NFElementModule::GetRecordManager(const std::string& strConfigName)
{
	NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
	if (pElementInfo)
	{
		return pElementInfo->GetRecordManager();
	}
	return NULL;
}

bool NFElementModule::ExistElement(const std::string& strConfigName)
{
	NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
	if (pElementInfo)
	{
		return true;
	}

	return false;
}

bool NFElementModule::ExistElement(const std::string& strClassName, const std::string& strConfigName)
{
	NF_SHARE_PTR<ElementConfigInfo> pElementInfo = GetElement(strConfigName);
	if (!pElementInfo)
	{
		return false;
	}

	const std::string& strClass = pElementInfo->GetPropertyManager()->GetPropertyString("ClassName");
	if (strClass != strClassName)
	{
		return false;
	}

	return true;
}

bool NFElementModule::Load()
{
	if (mbLoaded)
	{
		return false;
	}

	NF_SHARE_PTR<NFIClass> pLogicClass = m_pClassModule->First();
	while (pLogicClass)
	{
		const std::string& strInstancePath = pLogicClass->GetInstancePath();
		if (strInstancePath.empty())
		{
			pLogicClass = m_pClassModule->Next();
			continue;
		}

		std::string strFile = pPluginManager->GetConfigPath() + strInstancePath;
		std::string strContent;
		pPluginManager->GetFileContent(strFile, strContent);

		rapidxml::xml_document<> xDoc;
		xDoc.parse<0>((char*)strContent.c_str());

		//support for unlimited layer class inherits
		rapidxml::xml_node<>* root = xDoc.first_node();
		for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
		{
			Load(attrNode, pLogicClass);
		}

		mbLoaded = true;
		pLogicClass = m_pClassModule->Next();
	}

	return true;
}

bool NFElementModule::Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pLogicClass)
{
	//attrNode is the node of a object
	std::string strConfigID = attrNode->first_attribute("Id")->value();
	if (strConfigID.empty())
	{
		NFASSERT(0, strConfigID, __FILE__, __FUNCTION__);
		return false;
	}

	if (ExistElement(strConfigID))
	{
		NFASSERT(0, strConfigID, __FILE__, __FUNCTION__);
		return false;
	}

	NF_SHARE_PTR<ElementConfigInfo> pElementInfo(NF_NEW ElementConfigInfo());
	AddElement(strConfigID, pElementInfo);

	//can find all configid by class name
	pLogicClass->AddId(strConfigID);

	//ElementConfigInfo* pElementInfo = CreateElement( strConfigID, pElementInfo );
	NF_SHARE_PTR<NFIPropertyManager> pElementPropertyManager = pElementInfo->GetPropertyManager();
	NF_SHARE_PTR<NFIRecordManager> pElementRecordManager = pElementInfo->GetRecordManager();

	//1.add property
	//2.set the default value  of them
	NF_SHARE_PTR<NFIPropertyManager> pClassPropertyManager = pLogicClass->GetPropertyManager();
	NF_SHARE_PTR<NFIRecordManager> pClassRecordManager = pLogicClass->GetRecordManager();
	if (pClassPropertyManager && pClassRecordManager)
	{
		NF_SHARE_PTR<NFIProperty> pProperty = pClassPropertyManager->First();
		while (pProperty)
		{
			pElementPropertyManager->AddProperty(NFGUID(), pProperty);

			pProperty = pClassPropertyManager->Next();
		}

		NF_SHARE_PTR<NFIRecord> pRecord = pClassRecordManager->First();
		while (pRecord)
		{
			NF_SHARE_PTR<NFRecord> xRecord = dynamic_pointer_cast<NFRecord>(
				pElementRecordManager->AddRecord(
					NFGUID(),
					pRecord->GetName(),
					pRecord->GetColData(),
					pRecord->GetMaxRows()));

			if (xRecord != nullptr)
			{
				xRecord->SetPublic(pRecord->GetPublic());
				xRecord->SetPrivate(pRecord->GetPrivate());
				xRecord->SetSave(pRecord->GetSave());
			}

			pRecord = pClassRecordManager->Next();
		}
	}

	//3.set the config value to them
	for (rapidxml::xml_attribute<>* pAttribute = attrNode->first_attribute(); pAttribute; pAttribute = pAttribute->next_attribute())
	{
		const char* pstrConfigName = pAttribute->name();
		const char* pstrConfigValue = pAttribute->value();

		NF_SHARE_PTR<NFIProperty> temProperty = pElementPropertyManager->GetElement(pstrConfigName);
		if (!temProperty)
		{
			continue;
		}

		NFData var;
		const NFDATA_TYPE eType = temProperty->GetType();
		switch (eType)
		{
		case TDATA_INT:
		{
			if (!LegalNumber(pstrConfigValue))
			{
				NFASSERT(0, temProperty->GetName(), __FILE__, __FUNCTION__);
			}
			var.SetInt(lexical_cast<NFINT64>(pstrConfigValue));
		}
		break;
		case TDATA_FLOAT:
		{
			if (strlen(pstrConfigValue) <= 0 || !LegalFloat(pstrConfigValue))
			{
				NFASSERT(0, temProperty->GetName(), __FILE__, __FUNCTION__);
			}
			var.SetFloat((double)atof(pstrConfigValue));
		}
		break;
		case TDATA_STRING:
		{
			var.SetString(pstrConfigValue);
		}
		break;
		case TDATA_OBJECT:
		{
			if (strlen(pstrConfigValue) <= 0)
			{
				NFASSERT(0, temProperty->GetName(), __FILE__, __FUNCTION__);
			}
			var.SetObject(NFGUID());
		}
		break;
		default:
			NFASSERT(0, temProperty->GetName(), __FILE__, __FUNCTION__);
			break;
		}
		temProperty->SetValue(var);
	}

	NFData xData;
	xData.SetString(pLogicClass->GetClassName());
	pElementPropertyManager->SetProperty("ClassName", xData);

	return true;
}

bool NFElementModule::LegalNumber(const char* str)
{
	int nLen = int(strlen(str));
	if (nLen <= 0)
	{
		return false;
	}

	int nStart = 0;
	if ('-' == str[0])
	{
		nStart = 1;
	}

	for (int i = nStart; i < nLen; ++i)
	{
		if (!isdigit(str[i]))
		{
			return false;
		}
	}

	return true;
}

bool NFElementModule::LegalFloat(const char* str)
{
	int nLen = int(strlen(str));
	if (nLen <= 0)
	{
		return false;
	}

	int nStart = 0;
	int nEnd = nLen;
	if ('-' == str[0])
	{
		nStart = 1;
	}
	if ('f' == std::tolower(str[nEnd - 1]))
	{
		nEnd--;
	}

	if (nEnd <= nStart)
	{
		return false;
	}

	int pointNum = 0;
	for (int i = nStart; i < nEnd; ++i)
	{
		if ('.' == str[i])
		{
			pointNum++;
		}

		if (!isdigit(str[i]) && '.' != str[i])
		{
			return false;
		}
	}

	if (pointNum > 1)
	{
		return false;
	}

	return true;
}

bool NFElementModule::AfterInit()
{
	return true;
}

bool NFElementModule::BeforeShut()
{
	return true;
}

bool NFElementModule::Execute()
{
	return true;
}

bool NFElementModule::Clear()
{
	ClearAll();

	NF_SHARE_PTR<NFIClass> pLogicClass = m_pClassModule->First();
	while (pLogicClass != nullptr)
	{
		pLogicClass->ClearId();
		pLogicClass = m_pClassModule->Next();
	}

	mbLoaded = false;
	return true;
}