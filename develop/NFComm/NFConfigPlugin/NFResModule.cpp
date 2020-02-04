///--------------------------------------------------------------------
/// 文件名:		NFResModule.cpp
/// 内  容:		配置文件管理
/// 说  明:		加载proto二进制形式的配置文件
/// 创建日期:	2019年8月15日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFResModule.h"
#include "../NFUtils/QLOG.h"

// 资源配置命名空间(以后需要挪到配置中)
const std::string RES_NAMESPACE = "Garena";

NFResModule::NFResModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

NFResModule::~NFResModule()
{
	for (auto& it : m_mapRes)
	{
		delete (it.second);
	}

	m_mapRes.clear();
}

bool NFResModule::Init()
{
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();

	return true;
}

bool NFResModule::AfterInit()
{
	return true;
}

google::protobuf::Message* NFResModule::InnerGet(const char* szPath)
{
	if (szPath == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " path empty";
		return nullptr;
	}

	auto it = m_mapRes.find(szPath);
	if (it == m_mapRes.end())
	{
		return nullptr;
	}

	return it->second;
}

bool NFResModule::InnerLoad(
	google::protobuf::Message* pRes, 
	const char* szPath, 
	bool bReload/* = false*/)
{
	if (pRes == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pRes == NULL";
		return false;
	}

	if (szPath == nullptr || szPath[0] == '\0')
	{
		QLOG_WARING << __FUNCTION__ << " path empty";
		return false;
	}

	FILE* fp = fopen(szPath, "rb");
	if (fp == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " open failed path:" << szPath;
		return false;
	}

	fseek(fp, 0, SEEK_END);
	const long filelength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	std::string strContent = "";
	strContent.resize(filelength);
	fread((void*)strContent.data(), filelength, 1, fp);
	fclose(fp);

	pRes->Clear();
	if (!pRes->ParseFromString(strContent))
	{
		QLOG_WARING << __FUNCTION__ << " parse failed path:" << szPath;
		return false;
	}

	m_mapRes[szPath] = pRes;

	// 添加逻辑类数据
	m_pElementModule->AddClassData(*pRes, std::string(szPath), bReload);

	return true;
}

bool NFResModule::InnerAddReloadHook(const char* szPath, const google::protobuf::Message* pRes, ResReloadFunction pfReload)
{
	if (pfReload == nullptr)
	{
		return false;
	}

	m_mapHook[szPath].push_back(pfReload);
	pfReload(szPath, pRes);

	return true;
}

const google::protobuf::Message* NFResModule::Load(const char* szPath, ResReloadFunction pfReload /*= nullptr*/)
{
	auto* pRes = InnerGet(szPath);
	if (pRes != nullptr)
	{
		return pRes;
	}

	std::string strName = szPath;
	size_t posBegin1 = strName.rfind('/');
	size_t posBegin2 = strName.rfind('\\');
	size_t posBegin = 0;
	if (posBegin1 != std::string::npos)
	{
		posBegin = posBegin1 + 1;
	}
	if (posBegin2 != std::string::npos)
	{
		posBegin = (std::max)(posBegin1, posBegin2) + 1;
	}
	
	size_t posEnd = strName.rfind('.');
	int nCount = posEnd - posBegin;
	if (nCount <= 0)
	{
		QLOG_WARING << __FUNCTION__ << " get res name failed:" << szPath;
		return false;
	}

	strName = strName.substr(posBegin, nCount);
	std::string strArray = RES_NAMESPACE + "." + strName + "_ARRAY";
	auto* pDesc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(strArray);
	if (pDesc == nullptr)
	{
		std::string strMap = strName + "_MAP";
		pDesc = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(strMap);
		if (pDesc == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " pDesc == NULL name:" << strName << 
				" path:" << szPath;
			return nullptr;
		}
	}

	auto pPrototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(pDesc);
	if (pPrototype == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pPrototype == NULL name:" << strName 
			<< " path:" << szPath;
		return nullptr;
	}

	pRes = pPrototype->New();
	if (pRes == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " New Descriptor failed name:" << strName 
			<< " path:" << szPath;
		return nullptr;
	}

	if (!InnerLoad(pRes, szPath, false))
	{
		return nullptr;
	}

	InnerAddReloadHook(szPath, pRes, pfReload);

	return pRes;
}

bool NFResModule::Reload(const char* szPath)
{
	auto pRes = InnerGet(szPath);
	if (pRes == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " res not load, path:" << szPath;
		return false;
	}

	if (!InnerLoad(pRes, szPath, true))
	{
		return false;
	}

	for (auto& it : m_mapHook[szPath])
	{
		it(szPath, pRes);
	}

	return true;
}

bool NFResModule::ReloadAll()
{
	m_pElementModule->Clear();
	m_pElementModule->Load();

	for (auto& it : m_mapRes)
	{
		InnerLoad(it.second, it.first.c_str());

		for (auto& itHook : m_mapHook[it.first])
		{
			itHook(it.first, it.second);
		}
	}

	return true;
}

bool NFResModule::ConvertMapData(
	const google::protobuf::Message* pRes, 
	ResMapStrData& mapData, 
	const std::string& strKeyName)
{
	mapData.clear();

	if (pRes == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pRes == NULL";
		return false;
	}

	auto& msg = *pRes;
	auto* pDesc = msg.GetDescriptor();
	if (pDesc == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pDesc == NULL";
		return false;
	}
	
	auto* pReflection = msg.GetReflection();
	if (pReflection == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pReflection == NULL";
		return false;
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
				NFData xKeyData;
				MapRowData xRowData;
				if (!GetMapRowData(pRepeatedMsg, strKeyName, xKeyData, xRowData))
				{
					QLOG_WARING << __FUNCTION__ << " failed parse row data";
					continue;
				}

				if (strKeyName.empty())
				{
					mapData[::lexical_cast<std::string>(j)] = xRowData;
				}
				else
				{
					mapData[xKeyData.GetString()] = xRowData;
				}
			}
		}
	}

	return true;
}

bool NFResModule::ConvertMapData(
	const google::protobuf::Message* pRes,
	ResMapIntData& mapData,
	const std::string& strKeyName)
{
	mapData.clear();

	if (pRes == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pRes == NULL";
		return false;
	}

	auto& msg = *pRes;

	auto* pDesc = msg.GetDescriptor();
	if (pDesc == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pDesc == NULL";
		return false;
	}

	auto* pReflection = msg.GetReflection();
	if (pReflection == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pReflection == NULL";
		return false;
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
				NFData xKeyData;
				MapRowData xRowData;
				if (!GetMapRowData(pRepeatedMsg, strKeyName, xKeyData, xRowData))
				{
					QLOG_WARING << __FUNCTION__ << " failed parse row data";
					continue;
				}

				if (strKeyName.empty())
				{
					mapData[j] = xRowData;
				}
				else
				{
					mapData[xKeyData.GetInt()] = xRowData;
				}
			}
		}
	}

	return true;
}

// 解析一行数据
bool NFResModule::GetMapRowData(
	const google::protobuf::Message& msg,
	const std::string& strKeyName, 
	NFData& xKeyData, 
	MapRowData& mapRow)
{
	auto* pDesc = msg.GetDescriptor();
	if (pDesc == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pDesc == NULL";
		return false;
	}

	auto* pReflection = msg.GetReflection();
	if (pReflection == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pReflection == NULL";
		return false;
	}

	if (!strKeyName.empty())
	{
		auto* pKeyName = pDesc->FindFieldByName(strKeyName);
		if (pKeyName == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " not find key:" << strKeyName;
			return false;
		}

		auto nType = pKeyName->type();
		switch (nType)
		{
		case google::protobuf::FieldDescriptor::Type::TYPE_INT32:
			xKeyData.SetInt(pReflection->GetInt32(msg, pKeyName));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_STRING:
		case google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
			xKeyData.SetString(pReflection->GetString(msg, pKeyName));
			break;
		}
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

		auto nType = pField->type();
		switch (nType)
		{
		case google::protobuf::FieldDescriptor::Type::TYPE_DOUBLE:
			mapRow[strName].SetFloat(pReflection->GetDouble(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_FLOAT:
			mapRow[strName].SetFloat(pReflection->GetFloat(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_INT64:
		case google::protobuf::FieldDescriptor::Type::TYPE_SFIXED64:
		case google::protobuf::FieldDescriptor::Type::TYPE_SINT64:
			mapRow[strName].SetInt(pReflection->GetInt64(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_UINT64:
		case google::protobuf::FieldDescriptor::Type::TYPE_FIXED64:
			mapRow[strName].SetInt(pReflection->GetUInt64(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_INT32:
		case google::protobuf::FieldDescriptor::Type::TYPE_SFIXED32:
		case google::protobuf::FieldDescriptor::Type::TYPE_SINT32:
			mapRow[strName].SetInt(pReflection->GetInt32(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_UINT32:
		case google::protobuf::FieldDescriptor::Type::TYPE_FIXED32:
			mapRow[strName].SetInt(pReflection->GetUInt32(msg, pField));
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_BOOL:
			mapRow[strName].SetInt(pReflection->GetBool(msg, pField) ? 1 : 0);
			break;
		case google::protobuf::FieldDescriptor::Type::TYPE_STRING:
		case google::protobuf::FieldDescriptor::Type::TYPE_BYTES:
			mapRow[strName].SetString(pReflection->GetString(msg, pField));
			break;
		}
	}

	return true;
}
