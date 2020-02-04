///--------------------------------------------------------------------
/// 文件名:		LuaExt.hpp
/// 内  容:		lua公用函数
/// 说  明:		
/// 创建日期:	2019.10.29
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#define TRY_RUN_GLOBAL_SCRIPT_FUN0(strFuncName)   try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(); }   catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_RUN_GLOBAL_SCRIPT_FUN1(strFuncName, arg1)  try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(arg1); }catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_RUN_GLOBAL_SCRIPT_FUN2(strFuncName, arg1, arg2)  try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(arg1, arg2); }catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_LOAD_SCRIPT_FLE(strFileName)  try{mLuaContext.doFile(strFileName);} catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }

extern LuaIntf::LuaContext mLuaContext;

#ifndef LUA_EXT_H
#define LUA_EXT_H

#include "LuaIntf/LuaRef.h"
#include "../NFPluginModule/NFGUID.h"
#include "../NFCore/NFDataList.hpp"
#include "../NFPluginModule/NFILogModule.h"
#include "../NFPluginModule/NFIKernelModule.h"
#include "IniFile.h"
#include "../NFPluginModule/ConnectData.h"
#include "../NFPluginModule/NetData.h"
#include "StringUtil.h"
#include "NFServer/WorldServer/WhiteIP.h"
#include "../NFPluginModule/NotifyData.h"
#include "QuickLog.h"

LuaIntf::LuaContext mLuaContext;
NFIPluginManager* p = nullptr;
NFIKernelModule* kernel = nullptr;
NFILogModule* plog = nullptr;

void NxMessageBox(const std::string& strData)
{
	::MessageBox(NULL, strData.c_str(), "lua extension", MB_OK);
}

NFINT64 NxTime()
{
	return p->GetNowTime();
}

int NxAppID()
{
	return p->GetAppID();
}

const std::string& NxAppName()
{
	return p->GetAppName();
}

const std::string& NxConfigName()
{
	return p->GetConfigName();
}

const std::string& NxPath()
{
	return p->GetConfigPath();
}

bool NxBool(const std::string& strData)
{
	if (::stricmp(strData.c_str(), "true") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SetPropertyInt(const NFGUID& self, const std::string& strPropertyName, const NFINT64 nValue)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyInt(strPropertyName, nValue);
}

bool SetPropertyFloat(const NFGUID& self, const std::string& strPropertyName, const double dValue)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyFloat(strPropertyName, dValue);
}

bool SetPropertyString(const NFGUID& self, const std::string& strPropertyName, const std::string& strValue)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyString(strPropertyName, strValue);
}

bool SetPropertyObject(const NFGUID& self, const std::string& strPropertyName, const NFGUID& objectValue)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyObject(strPropertyName, objectValue);
}

NFINT64 GetPropertyInt(const NFGUID& self, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_INT;
	}

	return pObj->GetPropertyInt(strPropertyName);
}

double GetPropertyFloat(const NFGUID& self, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_FLOAT;
	}

	return pObj->GetPropertyFloat(strPropertyName);
}

std::string GetPropertyString(const NFGUID& self, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_STR;
	}

	return pObj->GetPropertyString(strPropertyName);
}

NFGUID GetPropertyObject(const NFGUID& self, const std::string& strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_OBJECT;
	}

	return pObj->GetPropertyObject(strPropertyName);
}

int AddRow(const NFGUID& self, std::string& strRecordName, const NFDataList& var)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return -1;
	}

	NF_SHARE_PTR<NFIRecord> pRecord = pObj->GetRecord(strRecordName);
	if (nullptr == pRecord)
	{
		return -1;
	}

	return pRecord->AddRow(-1, var);
}

bool RemoveRow(const NFGUID& self, std::string& strRecordName, const int nRow)
{
	NF_SHARE_PTR<NFIObject> pObj = kernel->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	NF_SHARE_PTR<NFIRecord> pRecord = pObj->GetRecord(strRecordName);
	if (nullptr == pRecord)
	{
		return false;
	}

	return pRecord->Remove(nRow);
}

void LogInfo(const std::string& strData)
{
	plog->LogInfo(strData);
}

void LogError(const std::string& strData)
{
	plog->LogError(strData);
}

void LogWarning(const std::string& strData)
{
	plog->LogWarning(strData);
}

void LogDebug(const std::string& strData)
{
	plog->LogDebug(strData);
}

bool IsDebug()
{
#ifdef NF_DEBUG_MODE
	return true;
#endif
	return false;
}

void SetLogLauncher(const std::string& strData)
{
	plog->SetLogLauncher(strData);
}

void SetLogGroupID(int nGroupID)
{
	plog->StartLogModule(nGroupID);
}

void SetLogLevel(int nLevel)
{
	QuickLog::GetRef().SetMaxLevel(LogLevel(nLevel));
}

void SetLogEchoLevel(int nLevel)
{
	QuickLog::GetRef().SetEchoMaxLevel(LogLevel(nLevel));
}

void OnLuaExt(NFIPluginManager* pPluginManager, NFIKernelModule* m_pKernelModule,
	NFILogModule* m_pLogModule, LuaIntf::LuaContext& mLuaContext)
{
	p = pPluginManager;
	kernel = m_pKernelModule;
	plog = m_pLogModule;

	LuaIntf::LuaBinding(mLuaContext).beginClass<NotifyData>("NotifyData")
		.addConstructor(LUA_ARGS())
		.addProperty("ip", &NotifyData::GetIP, &NotifyData::SetIP)
		.addProperty("port", &NotifyData::GetPort, &NotifyData::SetPort)
		.addProperty("webport", &NotifyData::GetWebPort, &NotifyData::SetWebPort)
		.addProperty("name", &NotifyData::GetName, &NotifyData::SetName)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NetData>("NetData")
		.addConstructor(LUA_ARGS())
		.addProperty("ip", &NetData::GetIP, &NetData::SetIP)
		.addProperty("port", &NetData::GetPort, &NetData::SetPort)
		.addProperty("buffer_size", &NetData::GetBufferSize, &NetData::SetBufferSize)
		.addProperty("read_buffer_size", &NetData::GetReadBufferSize, &NetData::SetReadBufferSize)
		.addProperty("max_connect", &NetData::GetMaxConnect, &NetData::SetMaxConnect)
		.addProperty("net_type", &NetData::GetNetType, &NetData::SetNetType)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<ConnectCfg>("ConnectData")
		.addConstructor(LUA_ARGS())
		.addProperty("app_type", &ConnectCfg::GetServerType, &ConnectCfg::SetServerType)
		.addProperty("ip", &ConnectCfg::GetIP, &ConnectCfg::SetIP)
		.addProperty("port", &ConnectCfg::GetPort, &ConnectCfg::SetPort)
		.addProperty("is_weak_link", &ConnectCfg::GetIsWeakLink, &ConnectCfg::SetIsWeakLink)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFGUID>("NFGUID")
		.addConstructor(LUA_ARGS())
		.addProperty("data", &NFGUID::GetData, &NFGUID::SetData)
		.addProperty("head", &NFGUID::GetHead, &NFGUID::SetHead)
		.addFunction("to_string", &NFGUID::ToString)
		.addFunction("from_string", &NFGUID::FromString)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFDataList>("NFDataList")
		.addConstructor(LUA_ARGS())
		.addFunction("empty", &NFDataList::IsEmpty)
		.addFunction("count", &NFDataList::GetCount)
		.addFunction("nx_type", &NFDataList::Type)

		.addFunction("add_int", &NFDataList::AddInt)
		.addFunction("add_float", &NFDataList::AddFloat)
		.addFunction("add_string", &NFDataList::AddStringFromChar)
		.addFunction("add_object", &NFDataList::AddObject)

		.addFunction("set_int", &NFDataList::SetInt)
		.addFunction("set_float", &NFDataList::SetFloat)
		.addFunction("set_string", (bool (NFDataList:: *)(const int, const std::string&))&NFDataList::SetString)
		.addFunction("set_object", &NFDataList::SetObject)

		.addFunction("int", &NFDataList::Int)
		.addFunction("float", &NFDataList::Float)
		.addFunction("string", &NFDataList::String)
		.addFunction("object", &NFDataList::Object)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFData>("TData")
		.addConstructor(LUA_ARGS())
		.addFunction("float", &NFData::GetFloat)
		.addFunction("int", &NFData::GetInt)
		.addFunction("object", &NFData::GetObject)
		.addFunction("string", &NFData::GetString)

		.addFunction("type", &NFData::GetType)
		.addFunction("is_null", &NFData::IsNullValue)

		.addFunction("set_float", &NFData::SetFloat)
		.addFunction("set_int", &NFData::SetInt)
		.addFunction("set_object", &NFData::SetObject)
		.addFunction("set_string", (void (NFData::*)(const std::string&))&NFData::SetString)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<CIniFile>("IniFile")
		.addConstructor(LUA_ARGS())
		.addProperty("file_name", &CIniFile::GetFileName, &CIniFile::SetFileName)
		.addFunction("load_file", &CIniFile::LoadFromFile)
		.addFunction("read_int", &CIniFile::ReadInteger)
		.addFunction("read_float", &CIniFile::ReadFloat)
		.addFunction("read_string", &CIniFile::ReadString)
		.addFunction("read_bool", &CIniFile::ReadBool)
		.addFunction("find_section", &CIniFile::FindSection)
		.addFunction("section_count", &CIniFile::GetSectItemCount)
		.addFunction("section_data", &CIniFile::GetSectionDataKey)
		.addFunction("section_value", &CIniFile::GetSectionDataValue)
		.addFunction("destroy", &CIniFile::Release)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFIPluginManager>("NFIPluginManager")
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginModule("kernel")
		.addFunction("nx_time", &NxTime)
		.addFunction("nx_appid", &NxAppID)
		.addFunction("nx_appname", &NxAppName)
		.addFunction("nx_configname", &NxConfigName)
		.addFunction("nx_path", &NxPath)
		.addFunction("nx_msgbox", &NxMessageBox)
		.addFunction("nx_bool", &NxBool)

		.addFunction("set_prop_int", &SetPropertyInt)
		.addFunction("set_prop_float", &SetPropertyFloat)
		.addFunction("set_prop_string", &SetPropertyString)
		.addFunction("set_prop_object", &SetPropertyObject)

		.addFunction("get_prop_int", &GetPropertyInt)
		.addFunction("get_prop_float", &GetPropertyFloat)
		.addFunction("get_prop_string", &GetPropertyString)
		.addFunction("get_prop_object", &GetPropertyObject)

		.addFunction("add_row", &AddRow)
		.addFunction("remove_row", &RemoveRow)

		.addFunction("log_info", &LogInfo)
		.addFunction("log_error", &LogError)
		.addFunction("log_warning", &LogWarning)
		.addFunction("log_debug", &LogDebug)
		.addFunction("set_log_launcher", &SetLogLauncher)
		.addFunction("set_log_groupid", &SetLogGroupID)
		.addFunction("is_debug", &IsDebug)
		.addFunction("set_log_level", &SetLogLevel)
		.addFunction("set_log_echo_level", &SetLogEchoLevel)
		.endModule();
}

#endif
