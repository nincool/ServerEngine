///--------------------------------------------------------------------
/// 文件名:		NFLuaScriptModule.h
/// 内  容:		lua
/// 说  明:		
/// 创建日期:	2019年10月25日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_LUA_SCRIPT_MODULE_H
#define NF_LUA_SCRIPT_MODULE_H

//just define it as 0 if you want to use luaintf with C
//#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0

#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIEventModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "../NFPluginModule/NFIModule.h"

class NFLuaScriptModule : public NFIModule
{
public:
    NFLuaScriptModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

	virtual bool Awake();
	virtual bool Init();
    virtual bool Shut();
	virtual bool ReadyExecute();
	virtual bool Execute();
    virtual bool AfterInit();
    virtual bool BeforeShut();
	virtual bool OnRegisterLua(LuaIntf::LuaContext& mLuaContext);
	static bool OnRegisterLua11(LuaIntf::LuaContext& mLuaContext);

	static void OnTest();

public:
	void RegisterModule(const std::string& tableName, const LuaIntf::LuaRef& luatbl);

	//FOR KERNEL MODULE
	NFGUID CreateObject(const NFGUID& self, const int nSceneID, const int nGroupID, const std::string& strClassName, const std::string& strIndex, const NFDataList& arg);
	bool ExistObject(const NFGUID& self);
	bool DestroyObject(const NFGUID & self);

	//return the group id
	bool DoEvent(const NFGUID& self, const int nEventID, const NFDataList& arg);

	bool FindProperty(const NFGUID& self, const std::string& strPropertyName);

	bool SetPropertyInt(const NFGUID& self, const std::string& strPropertyName, const NFINT64 nValue);
	bool SetPropertyFloat(const NFGUID& self, const std::string& strPropertyName, const double dValue);
	bool SetPropertyString(const NFGUID& self, const std::string& strPropertyName, const std::string& strValue);
	bool SetPropertyObject(const NFGUID& self, const std::string& strPropertyName, const NFGUID& objectValue);

	NFINT64 GetPropertyInt(const NFGUID& self, const std::string& strPropertyName);
	double GetPropertyFloat(const NFGUID& self, const std::string& strPropertyName);
	std::string GetPropertyString(const NFGUID& self, const std::string& strPropertyName);
	NFGUID GetPropertyObject(const NFGUID& self, const std::string& strPropertyName);

	bool AddPropertyCallBack(const NFGUID& self, std::string& strPropertyName, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);
    bool AddRecordCallBack(const NFGUID& self, std::string& strRecordName, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);
    bool AddEventCallBack(const NFGUID& self, const NFEventDefine nEventID, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);

	int AddRow(const NFGUID& self, std::string& strRecordName, const NFDataList& var);
	bool RemRow(const NFGUID& self, std::string& strRecordName, const int nRow);

	NFINT64 GetNowTime();
	NFGUID CreateID();
	NFINT64 APPID();
	NFINT64 APPType();

	//FOR ELEMENT MODULE
	bool ExistElementObject(const std::string& strConfigName);
	std::vector<std::string> GetEleList(const std::string& strClassName);

	NFINT64 GetElePropertyInt(const std::string& strConfigName, const std::string& strPropertyName);
	double GetElePropertyFloat(const std::string& strConfigName, const std::string& strPropertyName);
	std::string GetElePropertyString(const std::string& strConfigName, const std::string& strPropertyName);

	//FOR NET MODULE
	void RemoveClientMsgCallBack(const int nMsgID);
	void AddClientMsgCallBack(const int nMsgID, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);
	void RemoveServerMsgCallBack(const int nServerType, const int nMsgID);
	void AddServerMsgCallBack(const int nServerType, const int nMsgID, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);
	void RemoveHttpCallBack(const std::string& path);
	void AddHttpCallBack(const std::string& path, const int httpType, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);

	//for log
	void LogInfo(const std::string& strData);
	void LogError(const std::string& strData);
	void LogWarning(const std::string& strData);
	void LogDebug(const std::string& strData);

    //hot fix
	void SetVersionCode(const std::string& strData);
	const std::string& GetVersionCode();

	void NxMessageBox(const std::string& strData);

	//FOR CLASS MDOULE
    bool AddClassCallBack(std::string& className, const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);

protected:
    template<typename T>
    bool AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, const NFGUID& self, T key, std::string& luaFunc);

    template<typename T>
    bool AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, T key, std::string& luaFunc);

    template<typename T1, typename... T2>
    bool CallLuaFuncFromMap(NFMap<T1, NFMap<NFGUID, NFList<string>>>& funcMap, T1 key, const NFGUID& self, T2... arg);

    template<typename T1, typename... T2>
    bool CallLuaFuncFromMap(NFMap<T1, NFMap<NFGUID, NFList<string>>>& funcMap, T1 key, T2... arg);

    int OnLuaPropertyCB(const NFGUID& self, const std::string& strPropertyName, const NFData& oldVar, const NFData& newVar);
    int OnLuaRecordCB(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar);

    int OnLuaEventCB(const NFGUID& self, const NFEventDefine nEventID, const NFDataList& argVar);

	int OnClassEventCB(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
    
	void OnScriptReload();

protected:
    bool Register();
	std::string FindFuncName(const LuaIntf::LuaRef& luatbl, const LuaIntf::LuaRef& luaFunc);

protected:
    NFIElementModule* m_pElementModule;
    NFKernelModule* m_pKernelModule;
    NFIClassModule* m_pClassModule;
	NFIEventModule* m_pEventModule;
    NFILogModule* m_pLogModule;
	
protected:
    int64_t mnTime;
    std::string strVersionCode;

	std::map<std::string, LuaIntf::LuaRef> mxTableName;

    NFMap<std::string, NFMap<NFGUID, NFList<std::string>>> mxLuaPropertyCallBackFuncMap;
    NFMap<std::string, NFMap<NFGUID, NFList<std::string>>> mxLuaRecordCallBackFuncMap;
    NFMap<int, NFMap<NFGUID, NFList<std::string>>> mxLuaEventCallBackFuncMap;
    NFMap<std::string, NFMap<NFGUID, NFList<std::string>>> mxLuaHeartBeatCallBackFuncMap;

    NFMap<std::string, NFList<std::string>> mxClassEventFuncMap;
};

#endif
