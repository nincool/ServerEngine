///--------------------------------------------------------------------
/// 文件名:		NFLuaScriptModule.cpp
/// 内  容:		lua
/// 说  明:		
/// 创建日期:	2019年10月25日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <assert.h>
#include "NFLuaScriptModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFServer/PlatformServer/PlatformKernel.h"

bool NFLuaScriptModule::Awake()
{
	mnTime = pPluginManager->GetNowTime();

	m_pKernelModule = dynamic_cast<NFKernelModule*>(pPluginManager->FindModule<NFIKernelModule>());
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pEventModule = pPluginManager->FindModule<NFIEventModule>();
    m_pNetClientModule = pPluginManager->FindModule<NFINetClientModule>();
    m_pNetModule = pPluginManager->FindModule<NFINetModule>();
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();

    //Register();
	

	//TRY_RUN_GLOBAL_SCRIPT_FUN0("module_awake");

	return true;
}

bool NFLuaScriptModule::Init()
{
   // TRY_RUN_GLOBAL_SCRIPT_FUN0("module_init");

    return true;
}

bool NFLuaScriptModule::AfterInit()
{
	//TRY_RUN_GLOBAL_SCRIPT_FUN0("module_after_init");

    return true;
}

bool NFLuaScriptModule::Shut()
{
	//TRY_RUN_GLOBAL_SCRIPT_FUN0("module_shut");

    return true;
}

bool NFLuaScriptModule::ReadyExecute()
{
	//TRY_RUN_GLOBAL_SCRIPT_FUN0("module_ready_execute");

	return true;
}

bool NFLuaScriptModule::Execute()
{
    //1分钟reload一次
#ifdef NF_DEBUG_MODE
    if (pPluginManager->GetNowTime() - mnTime > 10)
#else
    if (pPluginManager->GetNowTime() - mnTime > 60)
#endif
    {
        mnTime = pPluginManager->GetNowTime();

        OnScriptReload();
    }

    return true;
}

bool NFLuaScriptModule::BeforeShut()
{
   // TRY_RUN_GLOBAL_SCRIPT_FUN0("module_before_shut");

    return true;
}

//bool NFLuaScriptModule::LoadScriptFile(const std::string& filePath)
//{
//	//TRY_LOAD_SCRIPT_FLE(filePath.c_str());
//	return true;
//}
//
//bool NFLuaScriptModule::RunScript(void* pf)
//{
//	//TRY_RUN_GLOBAL_SCRIPT_FUN1("init_script_system", this);
//	return true;
//}

void NFLuaScriptModule::RegisterModule(const std::string & tableName, const LuaIntf::LuaRef & luatbl)
{
	mxTableName[tableName] = luatbl;
}

NFGUID NFLuaScriptModule::CreateObject(const NFGUID & self, const int nSceneID, const int nGroupID, const std::string & strClassName, const std::string & strIndex, const NFDataList & arg)
{
	NF_SHARE_PTR<NFIObject> xObject = m_pKernelModule->CreateObject(self, strClassName, strIndex, arg);
	if (xObject)
	{
		return xObject->Self();

	}

	return NFGUID();
}

bool NFLuaScriptModule::ExistObject(const NFGUID & self)
{
	return m_pKernelModule->ExistObject(self);
}

bool NFLuaScriptModule::DestroyObject(const NFGUID & self)
{
	return m_pKernelModule->DestroyObject(self);
}

bool NFLuaScriptModule::DoEvent(const NFGUID & self, const int nEventID, const NFDataList & arg)
{
	m_pEventModule->DoEvent(self, (NFEventDefine)nEventID, arg);

	return true;
}

bool NFLuaScriptModule::FindProperty(const NFGUID & self, const std::string & strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->FindProperty(strPropertyName);
}

bool NFLuaScriptModule::SetPropertyInt(const NFGUID & self, const std::string & strPropertyName, const NFINT64 nValue)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyInt(strPropertyName, nValue);
}

bool NFLuaScriptModule::SetPropertyFloat(const NFGUID & self, const std::string & strPropertyName, const double dValue)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyFloat(strPropertyName, dValue);
}

bool NFLuaScriptModule::SetPropertyString(const NFGUID & self, const std::string & strPropertyName, const std::string & strValue)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyString(strPropertyName, strValue);
}

bool NFLuaScriptModule::SetPropertyObject(const NFGUID & self, const std::string & strPropertyName, const NFGUID & objectValue)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return false;
	}

	return pObj->SetPropertyObject(strPropertyName, objectValue);
}

NFINT64 NFLuaScriptModule::GetPropertyInt(const NFGUID & self, const std::string & strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_INT;
	}

	return pObj->GetPropertyInt(strPropertyName);
}

double NFLuaScriptModule::GetPropertyFloat(const NFGUID & self, const std::string & strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_FLOAT;
	}

	return pObj->GetPropertyFloat(strPropertyName);
}

std::string NFLuaScriptModule::GetPropertyString(const NFGUID & self, const std::string & strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_STR;
	}

	return pObj->GetPropertyString(strPropertyName);
}

NFGUID NFLuaScriptModule::GetPropertyObject(const NFGUID & self, const std::string & strPropertyName)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return NULL_OBJECT;
	}

	return pObj->GetPropertyObject(strPropertyName);
}

bool NFLuaScriptModule::AddClassCallBack(std::string& className, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc)
{
	auto funcNameList = mxClassEventFuncMap.GetElement(className);
	if (!funcNameList)
	{
		funcNameList = new NFList<string>();
		mxClassEventFuncMap.AddElement(className, funcNameList);

		m_pKernelModule->AddEventCallBack(className, EVENT_OnDefault, METHOD_ARGS(NFLuaScriptModule::OnClassEventCB));
	}
	
	std::string strfuncName = FindFuncName(luatbl, luaFunc);
	if (!strfuncName.empty())
	{
		if (!funcNameList->Find(strfuncName))
		{
			funcNameList->Add(strfuncName);

			return true;
		}
	}

	return false;
}

int NFLuaScriptModule::OnClassEventCB(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	int nType = args.Int(0);
	const std::string& strClassName = args.String(1);
	NFDataList var;
	var.Append(args, 2, args.GetCount() - 2);

    auto funcNameList = mxClassEventFuncMap.GetElement(strClassName);
    if (funcNameList)
    {
		std::string strFuncNme;
		bool ret = funcNameList->First(strFuncNme);
		while (ret)
		{
			/*try
			{
				LuaIntf::LuaRef func(mLuaContext, strFuncNme.c_str());
				func.call(self, strClassName, nType, var);
			}
			catch (LuaIntf::LuaException& e)
			{
				cout << e.what() << endl;
				return 0;
			}
			catch (...)
			{
				return 0;
			}*/

			ret = funcNameList->Next(strFuncNme);
		}
    }

	return -1;
}

void NFLuaScriptModule::OnScriptReload()
{
    NFINT64 nAppType = APPType();
    std::string strRootFileh = "";
    switch ((NF_SERVER_TYPES)(nAppType))
    {
        case NF_SERVER_TYPES::NF_ST_GAME:
        {
            strRootFileh = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/game/script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_LOGIN:
        {
            strRootFileh = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/login/script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_WORLD:
        {
            strRootFileh = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/world/script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_PROXY:
        {
            strRootFileh = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/proxy/script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_MASTER:
        {
            strRootFileh = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/master/script_reload.lua";
        }
        break;
        default:
        break;
    }
    
    if (!strRootFileh.empty())
    {
		//TRY_LOAD_SCRIPT_FLE(strRootFileh.c_str());
    }
}

bool NFLuaScriptModule::AddPropertyCallBack(const NFGUID& self, std::string& strPropertyName, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luatbl, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaPropertyCallBackFuncMap, self, strPropertyName, luaFuncName))
		{
			m_pKernelModule->AddPropertyCallBack(self, strPropertyName, this, &NFLuaScriptModule::OnLuaPropertyCB);
		}

		return true;
	}
    return false;
}

int NFLuaScriptModule::OnLuaPropertyCB(const NFGUID& self, const std::string& strPropertyName, const NFData& oldVar, const NFData& newVar)
{
    return CallLuaFuncFromMap(mxLuaPropertyCallBackFuncMap, strPropertyName, self, strPropertyName, oldVar, newVar);
}

bool NFLuaScriptModule::AddRecordCallBack(const NFGUID& self, std::string& strRecordName, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luatbl, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaRecordCallBackFuncMap, self, strRecordName, luaFuncName))
		{
			m_pKernelModule->AddRecordCallBack(self, strRecordName, this, &NFLuaScriptModule::OnLuaRecordCB);
		}
		return true;
	}

	return false;
}

int NFLuaScriptModule::OnLuaRecordCB(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar)
{
    return CallLuaFuncFromMap(mxLuaRecordCallBackFuncMap, xEventData.strRecordName, self, xEventData.strRecordName, xEventData.nOpType, xEventData.nRow, xEventData.nCol, oldVar, newVar);
}

bool NFLuaScriptModule::AddEventCallBack(const NFGUID& self, const NFEventDefine nEventID, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luatbl, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaEventCallBackFuncMap, self, (int)nEventID, luaFuncName))
		{
			m_pEventModule->AddEventCallBack(self, nEventID, this, &NFLuaScriptModule::OnLuaEventCB);
		}

		return true;
	}

	return false;
}

int NFLuaScriptModule::OnLuaEventCB(const NFGUID& self, const NFEventDefine nEventID, const NFDataList& argVar)
{
    return CallLuaFuncFromMap(mxLuaEventCallBackFuncMap, (int)nEventID, self, nEventID, (NFDataList&)argVar);
}

int NFLuaScriptModule::AddRow(const NFGUID& self, std::string& strRecordName, const NFDataList& var)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		return -1;
	}

    NF_SHARE_PTR<NFIRecord> pRecord = pObj->GetRecord(strRecordName);
    if (nullptr == pRecord)
    {
        return -1;
    }

    return pRecord->AddRow(var);
}

bool NFLuaScriptModule::RemRow(const NFGUID & self, std::string & strRecordName, const int nRow)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
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

NFINT64 NFLuaScriptModule::GetNowTime()
{
	return pPluginManager->GetNowTime();
}

NFGUID NFLuaScriptModule::CreateID()
{
	return m_pKernelModule->CreateGUID();
}

NFINT64 NFLuaScriptModule::APPID()
{
	return pPluginManager->GetAppID();
}

NFINT64 NFLuaScriptModule::APPType()
{
    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement(LC::Server::ThisName());
    if (xLogicClass)
    {
		const std::vector<std::string>& strIdList = xLogicClass->GetIDList();
		for (int i = 0; i < strIdList.size(); ++i)
		{
			const std::string& strId = strIdList[i];

            const int nServerType = m_pElementModule->GetPropertyInt(strId, LC::Server::Type());
            const int nServerID = m_pElementModule->GetPropertyInt(strId, LC::Server::ServerID());
            if (pPluginManager->GetAppID() == nServerID)
            {
                return nServerType;
            }
        }
    }
    
	return 0;
}

bool NFLuaScriptModule::ExistElementObject(const std::string & strConfigName)
{
	return m_pElementModule->ExistElement(strConfigName);
}

std::vector<std::string> NFLuaScriptModule::GetEleList(const std::string& strClassName)
{
    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement(LC::Server::ThisName());
	if (xLogicClass)
	{
		return xLogicClass->GetIDList();
    }

    return std::vector<std::string>();
}

NFINT64 NFLuaScriptModule::GetElePropertyInt(const std::string & strConfigName, const std::string & strPropertyName)
{
	return m_pElementModule->GetPropertyInt(strConfigName, strPropertyName);
}

double NFLuaScriptModule::GetElePropertyFloat(const std::string & strConfigName, const std::string & strPropertyName)
{
	return m_pElementModule->GetPropertyFloat(strConfigName, strPropertyName);
}

std::string NFLuaScriptModule::GetElePropertyString(const std::string & strConfigName, const std::string & strPropertyName)
{
	return m_pElementModule->GetPropertyString(strConfigName, strPropertyName);
}

template<typename T>
bool NFLuaScriptModule::AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, const NFGUID& self, T key, string& luaFunc)
{
    auto funcList = funcMap.GetElement(key);
    if (!funcList)
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList = new NFMap<NFGUID, NFList<string>>;
        funcList->AddElement(self, funcNameList);
        funcMap.AddElement(key, funcList);
        return true;
    }

    if (!funcList->GetElement(self))
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList->AddElement(self, funcNameList);
        return true;
    }
    else
    {
        auto funcNameList = funcList->GetElement(self);
        if (!funcNameList->Find(luaFunc))
        {
            funcNameList->Add(luaFunc);
            return true;
        }
        else
        {
            return false;
        }
    }

}


template<typename T>
bool NFLuaScriptModule::AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, T key, string& luaFunc)
{
    auto funcList = funcMap.GetElement(key);
    if (!funcList)
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList = new NFMap<NFGUID, NFList<string>>;
        funcList->AddElement(NFGUID(), funcNameList);
        funcMap.AddElement(key, funcList);
        return true;
    }

    if (!funcList->GetElement(NFGUID()))
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList->AddElement(NFGUID(), funcNameList);
        return true;
    }
    else
    {
        auto funcNameList = funcList->GetElement(NFGUID());
        if (!funcNameList->Find(luaFunc))
        {
            funcNameList->Add(luaFunc);
            return true;
        }
        else
        {
            return false;
        }
    }

}

template<typename T1, typename ...T2>
bool NFLuaScriptModule::CallLuaFuncFromMap(NFMap<T1, NFMap<NFGUID, NFList<string>>>& funcMap, T1 key, const NFGUID& self, T2 ... arg)
{
    auto funcList = funcMap.GetElement(key);
    if (funcList)
    {
        auto funcNameList = funcList->GetElement(self);
        if (funcNameList)
        {
            string funcName;
            auto Ret = funcNameList->First(funcName);
            while (Ret)
            {
               /* try
                {
                    LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
                    func.call(self, key, arg ...);
					return true;
                }
                catch (LuaIntf::LuaException& e)
                {
                    cout << e.what() << endl;
                    return false;
                }
				catch (...)
				{
                    return false;
				}*/

                Ret = funcNameList->Next(funcName);
            }
        }
    }

    return true;
}

template<typename T1, typename ...T2>
bool NFLuaScriptModule::CallLuaFuncFromMap(NFMap<T1, NFMap<NFGUID, NFList<string>>>& funcMap, T1 key, T2 ... arg)
{
    auto funcList = funcMap.GetElement(key);
    if (funcList)
    {
        auto funcNameList = funcList->GetElement(NFGUID());
        if (funcNameList)
        {
            string funcName;
            auto Ret = funcNameList->First(funcName);
            while (Ret)
            {
               /* try
                {
                    LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
                    func.call(key, arg...);
					return true;
                }
                catch (LuaIntf::LuaException& e)
                {
                    cout << e.what() << endl;
                    return false;
                }
				catch (...)
				{
                    return false;
				}*/

                Ret = funcNameList->Next(funcName);
            }
        }
    }

    return true;
}

void NFLuaScriptModule::RemoveClientMsgCallBack(const int nMsgID)
{
	m_pNetModule->RemoveReceiveCallBack(nMsgID);
}

void NFLuaScriptModule::AddClientMsgCallBack(const int nMsgID, const LuaIntf::LuaRef & luatbl, const LuaIntf::LuaRef & luaFunc)
{
	//m_pNetModule->AddEventCallBack
}

void NFLuaScriptModule::RemoveServerMsgCallBack(const int nServerType, const int nMsgID)
{
	m_pNetClientModule->RemoveReceiveCallBack((NF_SERVER_TYPES)nServerType, nMsgID);
}

void NFLuaScriptModule::AddServerMsgCallBack(const int nServerType, const int nMsgID, const LuaIntf::LuaRef & luatbl, const LuaIntf::LuaRef & luaFunc)
{
	//m_pNetClientModule->AddReceiveCallBack((NF_SERVER_TYPES)nServerType, )
}

void NFLuaScriptModule::RemoveHttpCallBack(const std::string & path)
{
}

void NFLuaScriptModule::AddHttpCallBack(const std::string & path, const int httpType, const LuaIntf::LuaRef & luatbl, const LuaIntf::LuaRef & luaFunc)
{
}

void NFLuaScriptModule::LogInfo(const std::string& strData)
{
	m_pLogModule->LogInfo(strData);
}

void NFLuaScriptModule::LogError(const std::string& strData)
{
	m_pLogModule->LogError(strData);
}

void NFLuaScriptModule::LogWarning(const std::string& strData)
{
	m_pLogModule->LogWarning(strData);
}

void NFLuaScriptModule::LogDebug(const std::string& strData)
{
	m_pLogModule->LogDebug(strData);
}

void NFLuaScriptModule::SetVersionCode(const std::string& strData)
{
    strVersionCode = strData;
}

const std::string&  NFLuaScriptModule::GetVersionCode()
{
    return strVersionCode;
}

void NFLuaScriptModule::NxMessageBox(const std::string& strData)
{
	::MessageBox(NULL, strData.c_str(), "lua extension", MB_OK);
}

bool NFLuaScriptModule::OnRegisterLua11(LuaIntf::LuaContext& mLuaContext)
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<NFGUID>("NFGUID")
		.addConstructor(LUA_ARGS())
		.addProperty("data", &NFGUID::GetData, &NFGUID::SetData)
		.addProperty("head", &NFGUID::GetHead, &NFGUID::SetHead)
		.addFunction("tostring", &NFGUID::ToString)
		.addFunction("fromstring", &NFGUID::FromString)
		.endClass();
	return true;
}

bool NFLuaScriptModule::OnRegisterLua(LuaIntf::LuaContext& mLuaContext)
{
	/*LuaIntf::LuaBinding(mLuaContext).beginClass<NFGUID>("NFGUID")
		.addConstructor(LUA_ARGS())
		.addProperty("data", &NFGUID::GetData, &NFGUID::SetData)
		.addProperty("head", &NFGUID::GetHead, &NFGUID::SetHead)
		.addFunction("tostring", &NFGUID::ToString)
		.addFunction("fromstring", &NFGUID::FromString)
		.endClass();
*/
	/*LuaIntf::LuaBinding(mLuaContext).beginClass<NFIPluginManager>("NFIPluginManager")
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFDataList>("NFDataList")
		.addConstructor(LUA_ARGS())
		.addFunction("empty", &NFDataList::IsEmpty)
		.addFunction("count", &NFDataList::GetCount)
		.addFunction("tye", &NFDataList::Type)

		.addFunction("add_int", &NFDataList::AddInt)
		.addFunction("add_float", &NFDataList::AddFloat)
		.addFunction("add_string", &NFDataList::AddStringFromChar)
		.addFunction("add_object", &NFDataList::AddObject)

		.addFunction("set_int", &NFDataList::SetInt)
		.addFunction("set_float", &NFDataList::SetFloat)
		.addFunction("set_string", &NFDataList::SetString)
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
		.addFunction("set_string", &NFData::SetString)
		.endClass();*/

	//LuaIntf::LuaBinding(mLuaContext).beginClass<NFLuaScriptModule>("NFLuaScriptModule")
	//	.addConstructor(LUA_ARGS(NFIPluginManager*))
	//	.addFunction("register_module", &NFLuaScriptModule::RegisterModule)
	//	.addFunction("create_object", &NFLuaScriptModule::CreateObject)
	//	.addFunction("exist_object", &NFLuaScriptModule::ExistObject)
	//	.addFunction("destroy_object", &NFLuaScriptModule::DestroyObject)
	//	.addFunction("do_event", &NFLuaScriptModule::DoEvent)

	//	.addFunction("set_prop_int", &NFLuaScriptModule::SetPropertyInt)
	//	.addFunction("set_prop_float", &NFLuaScriptModule::SetPropertyFloat)
	//	.addFunction("set_prop_string", &NFLuaScriptModule::SetPropertyString)
	//	.addFunction("set_prop_object", &NFLuaScriptModule::SetPropertyObject)

	//	.addFunction("get_prop_int", &NFLuaScriptModule::GetPropertyInt)
	//	.addFunction("get_prop_float", &NFLuaScriptModule::GetPropertyFloat)
	//	.addFunction("get_prop_string", &NFLuaScriptModule::GetPropertyString)
	//	.addFunction("get_prop_object", &NFLuaScriptModule::GetPropertyObject)

	//	.addFunction("add_prop_cb", &NFLuaScriptModule::AddPropertyCallBack)
	//	.addFunction("add_record_cb", &NFLuaScriptModule::AddRecordCallBack)
	//	.addFunction("add_event_cb", &NFLuaScriptModule::AddEventCallBack)
	//	.addFunction("add_class_cb", &NFLuaScriptModule::AddClassCallBack)
	//	.addFunction("do_event", &NFLuaScriptModule::DoEvent)
	//	.addFunction("add_row", &NFLuaScriptModule::AddRow)
	//	.addFunction("rem_row", &NFLuaScriptModule::RemRow)

	//

	//	.addFunction("exist_ele", &NFLuaScriptModule::ExistElementObject)
	//	.addFunction("get_ele_list", &NFLuaScriptModule::GetEleList)
	//	.addFunction("get_ele_int", &NFLuaScriptModule::GetElePropertyInt)
	//	.addFunction("get_ele_float", &NFLuaScriptModule::GetElePropertyFloat)
	//	.addFunction("get_ele_string", &NFLuaScriptModule::GetElePropertyString)

	//	.addFunction("remove_cli_msg_cb", &NFLuaScriptModule::RemoveClientMsgCallBack)//as server
	//	.addFunction("add_cli_msg_cb", &NFLuaScriptModule::AddClientMsgCallBack)//as server
	//	.addFunction("remove_svr_msg_cb", &NFLuaScriptModule::RemoveServerMsgCallBack)//as client
	//	.addFunction("add_svr_msg_cb", &NFLuaScriptModule::AddServerMsgCallBack)//as client

	//	.addFunction("remove_http_cb", &NFLuaScriptModule::RemoveHttpCallBack)
	//	.addFunction("add_http_cb", &NFLuaScriptModule::AddHttpCallBack)

	//	.addFunction("log_info", &NFLuaScriptModule::LogInfo)
	//	.addFunction("log_error", &NFLuaScriptModule::LogError)
	//	.addFunction("log_warning", &NFLuaScriptModule::LogWarning)
	//	.addFunction("log_debug", &NFLuaScriptModule::LogDebug)

	//	.addFunction("get_version_code", &NFLuaScriptModule::GetVersionCode)
	//	.addFunction("set_version_code", &NFLuaScriptModule::SetVersionCode)

	//	.addFunction("nx_msgbox", &NFLuaScriptModule::NxMessageBox)

	//	.endClass();

    return true;
}

std::string NFLuaScriptModule::FindFuncName(const LuaIntf::LuaRef & luatbl, const LuaIntf::LuaRef & luaFunc)
{
	if (luatbl.isTable() && luaFunc.isFunction())
	{
		std::string strLuaTableName = "";
		std::map<std::string, LuaIntf::LuaRef>::iterator it = mxTableName.begin();
		for (it; it != mxTableName.end(); ++it)
		{
			if (it->second == luatbl)
			{
				strLuaTableName = it->first;
			}
		}
		
		if (!strLuaTableName.empty())
		{
			for (auto itr = luatbl.begin(); itr != luatbl.end(); ++itr)
			{
				const LuaIntf::LuaRef& key = itr.key();

				const std::string& sKey = key.toValue<std::string>();
				const LuaIntf::LuaRef& val = itr.value();
				if (val.isFunction() && luaFunc.isFunction() && val == luaFunc)
				{
					strLuaTableName.append(".");
					strLuaTableName.append(sKey);
					return strLuaTableName;
				}
			}
		}
	}

	return NULL_STR;
}
