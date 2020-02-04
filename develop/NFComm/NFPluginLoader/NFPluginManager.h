///--------------------------------------------------------------------
/// 文件名:		NFPluginManager.h
/// 内  容:		插件管理器
/// 说  明:		
/// 创建日期:	2019年10月31日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_PLUGIN_MANAGER_H
#define NF_PLUGIN_MANAGER_H

#include <map>
#include <string>
#include <time.h>
#include <thread>
#include "NFDynLib.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "prometheus/detail/builder.h"
#include "prometheus/counter.h"
#include "../NFPluginModule/MetricsDefine.h"
#include "RapidXML/rapidxml.hpp"
#include "../NFUtils/StringUtil.h"

class NFPluginManager : public NFIPluginManager
{
public:
    NFPluginManager();
    virtual ~NFPluginManager();

    virtual bool LoadPlugin() override;
    virtual bool Awake() override;
    virtual bool Init() override;
	virtual bool InitLua() override;
    virtual bool AfterInit() override;
    virtual bool ReadyExecute() override;
	virtual bool InitNetServer() override;
	virtual bool InitNetClient() override;
    virtual bool BeforeShut() override;
    virtual bool Shut() override;
	virtual bool Finalize() override;
	virtual bool CommandShut() override;

    virtual void Registered(NFIPlugin* pPlugin) override;
    virtual void UnRegistered(NFIPlugin* pPlugin) override;

    virtual bool ReLoadPlugin(const std::string& strPluginDLLName) override;
    virtual NFIPlugin* FindPlugin(const std::string& strPluginName) override;

    virtual void AddModule(const std::string& strModuleName, NFIModule* pModule) override;
    virtual void RemoveModule(const std::string& strModuleName) override;
    virtual NFIModule* FindModule(const std::string& strModuleName) override;
    virtual std::list<NFIModule*> Modules() override;

    virtual bool Execute() override;

	virtual int GetDistrictID() const override;
	virtual void SetDistrictID(const int nDistrictID) override;

    virtual int GetAppID() const override;
    virtual void SetAppID(const int nAppID) override;

    virtual int GetServerType() const override;
    virtual void SetServerType(const int nServerType) override;

	virtual int GetServerID() const override;
	virtual void SetServerID(const int nServerID) override;

    virtual NFINT64 GetInitTime() const override;
    virtual NFINT64 GetNowTime() const override;

    virtual const std::string& GetConfigPath() const override;
    virtual void SetConfigPath(const std::string & strPath) override;

    virtual void SetConfigName(const std::string& strAppName) override;
    virtual const std::string& GetConfigName() const override;

    virtual const std::string& GetAppName() const override;
    virtual void SetAppName(const std::string& strAppName) override;

	virtual const std::string& GetLuaName() const override;
	virtual void SetLuaName(const std::string& strLuaName) override;

	virtual const std::string& GetMetricsInfo() const override;
	virtual void SetMetricsInfo(const std::string& mstrMetricsInfo) override;

	virtual NF_SHARE_PTR<prometheus::Registry> GetMetricsRegistry() override;
	virtual void SetMetricsRegistry(NF_SHARE_PTR<prometheus::Registry> pMetricsRegistry) override;

	virtual prometheus::Family<prometheus::Counter>& GetMetricsCounter(int nTempIndex) override;
	virtual prometheus::Family<prometheus::Gauge>& GetMetricsGauge(int nTempIndex) override;
	virtual prometheus::Family<prometheus::Histogram>& GetMetricsHistogram(int nTempIndex) override;
	virtual prometheus::Family<prometheus::Summary>& GetMetricsSummary(int nTempIndex) override;

    virtual bool GetHistogramConfig(int nTempIndex, std::vector<double>& vecValue) override;
	virtual bool GetSummaryConfig(int nTempIndex, std::vector<QuantileInfo>& vecValue) override;

    void LoadMetricsConfig();
    void InitExporter();

	// 进程窗口标题
	virtual const std::string& GetTitleName();
	virtual void SetTitleName(const std::string& strTitleName);

    virtual const std::string& GetLogConfigName() const override;
    virtual void SetLogConfigName(const std::string& strName) override;

    virtual NFIPlugin* GetCurrentPlugin() override;
	virtual void SetCurrentPlugin(NFIPlugin* pPlugin) override;

	virtual NFIModule* GetCurrenModule() override;
    virtual void SetCurrenModule(NFIModule* pModule) override;

	virtual int GetFileConfig() override;
    virtual bool GetFileContent(const std::string &strFileName, std::string &strContent) override;

	virtual void SetExit(bool bExit) override;
	virtual bool GetExit() const override;
protected:
    bool LoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnLoadPluginLibrary(const std::string& strPluginDLLName);
private:
    std::string GetNodeValue(rapidxml::xml_attribute<>* pAttr);
    int GetMetricsType(std::string& value);

private:
	bool m_bExitApp = false;
    int mnAppID = 0;
	int mnServerID = 0;
	int mnDistrictID = 0;
    int mnServerType = 0;
    int mnVersion = 0;
    NFINT64 mnInitTime = 0;
    NFINT64 mnNowTime = 0;
    std::string mstrConfigPath = "";
    std::string mstrConfigName = "";
    std::string mstrAppName = "";
	std::string mstrLuaName = "";
	std::string mstrTitleName = "";
    std::string mstrLogConfigName = "";
    std::string mstrMetricsInfo = "";
    NF_SHARE_PTR<prometheus::Registry> pMetricsRegistry = nullptr;
    std::unordered_map<int, Metrics> mMetricsMap;

    NFIPlugin* mCurrentPlugin;
    NFIModule* mCurrenModule;

    typedef std::map<std::string, bool> PluginNameMap;
    typedef std::map<std::string, NFDynLib*> PluginLibMap;
    typedef std::map<std::string, NFIPlugin*> PluginInstanceMap;
    typedef std::map<std::string, NFIModule*> ModuleInstanceMap;

    typedef void(* DLL_START_PLUGIN_FUNC)(NFIPluginManager* pm);
    typedef void(* DLL_STOP_PLUGIN_FUNC)(NFIPluginManager* pm);
    typedef int(*DLL_GET_VERSION_FUNC)(NFIPluginManager* pm);

    PluginNameMap mPluginNameMap;
    PluginLibMap mPluginLibMap;
    PluginInstanceMap mPluginInstanceMap;
    ModuleInstanceMap mModuleInstanceMap;
};

#endif
