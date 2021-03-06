///--------------------------------------------------------------------
/// 文件名:		NFIPluginManager.h
/// 内  容:		插件管理接口
/// 说  明:		
/// 创建日期:	2019年8月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_PLUGIN_MANAGER_H
#define NFI_PLUGIN_MANAGER_H

#include <list>
#include "NFPlatform.h"
#include "Dependencies/prometheus/registry.h"
#include "Dependencies/prometheus/family.h"
#include "Dependencies/prometheus/counter.h"
#include "Dependencies/prometheus/gauge.h"
#include "Dependencies/prometheus/histogram.h"
#include "Dependencies/prometheus/summary.h"

class NFIPlugin;
class NFIModule;

typedef prometheus::detail::CKMSQuantiles::Quantile QuantileInfo;

template<typename DerivedType, typename BaseType>
class TIsDerived
{
public:
    static int AnyFunction(BaseType* base)
    {
        return 1;
    }

    static  char AnyFunction(void* t2)
    {
        return 0;
    }

    enum
    {
        Result = (sizeof(int) == sizeof(AnyFunction((DerivedType*)NULL))),
    };
};

#define FIND_MODULE(classBaseName, className)  \
	assert((TIsDerived<classBaseName, NFIModule>::Result));

class NFIPluginManager
{
public:
	NFIPluginManager()
	{
	}

	virtual bool LoadPlugin()
	{
		return true;
	}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Init()
	{
		return true;
	}

	virtual bool InitLua()
	{
		return true;
	}

	virtual bool AfterInit()
	{
		return true;
	}

	virtual bool InitNetServer()
	{
		return true;
	}

	virtual bool InitNetClient()
	{
		return true;
	}

	virtual bool ReadyExecute()
	{
		return true;
	}

	virtual bool Execute()
	{
		return true;
	}

	virtual bool BeforeShut()
	{
		return true;
	}

	virtual bool Shut()
	{
		return true;
	}

	virtual bool Finalize()
	{
		return true;
	}

	virtual bool CommandShut()
	{
		return true;
	}

	virtual bool OnReloadPlugin()
	{
		return true;
	}

	template <typename T>
	T* FindModule()
	{
		NFIModule* pLogicModule = FindModule(typeid(T).name());
		if (pLogicModule)
		{
			if (!TIsDerived<T, NFIModule>::Result)
			{
				return NULL;
			}
            //TODO OSX上dynamic_cast返回了NULL
#if NF_PLATFORM == NF_PLATFORM_APPLE
            T* pT = (T*)pLogicModule;
#else
			T* pT = dynamic_cast<T*>(pLogicModule);
#endif
			assert(NULL != pT);

			return pT;
		}
		assert(NULL);
		return NULL;
	}

	virtual bool ReLoadPlugin(const std::string& strPluginDLLName) = 0;
    virtual void Registered(NFIPlugin* plugin) = 0;
    virtual void UnRegistered(NFIPlugin* plugin) = 0;
    virtual NFIPlugin* FindPlugin(const std::string& strPluginName) = 0;

	virtual void AddModule(const std::string& strModuleName, NFIModule* pModule) = 0;
    virtual void RemoveModule(const std::string& strModuleName) = 0;
    virtual NFIModule* FindModule(const std::string& strModuleName) = 0;
	virtual std::list<NFIModule*> Modules() = 0;

	virtual int GetDistrictID() const = 0;
	virtual void SetDistrictID(const int nDistrictID) = 0;

    virtual int GetAppID() const = 0;
    virtual void SetAppID(const int nAppID) = 0;

	virtual int GetServerType() const = 0;
	virtual void SetServerType(const int nAppType) = 0;

	virtual int GetServerID() const = 0;
	virtual void SetServerID(const int nServerID) = 0;

    virtual NFINT64 GetInitTime() const = 0;
    virtual NFINT64 GetNowTime() const = 0;

	virtual const std::string& GetConfigPath() const = 0;
	virtual void SetConfigPath(const std::string & strPath) = 0;

	virtual void SetConfigName(const std::string& strAppName) = 0;	
	virtual const std::string& GetConfigName() const = 0;

	virtual const std::string& GetAppName() const = 0;
	virtual void SetAppName(const std::string& strAppName) = 0;

	virtual const std::string& GetLuaName() const = 0;
	virtual void SetLuaName(const std::string& strLuaName) = 0;

	virtual const std::string& GetMetricsInfo() const = 0;
	virtual void SetMetricsInfo(const std::string& mstrMetricsInfo) = 0;

	virtual NF_SHARE_PTR<prometheus::Registry> GetMetricsRegistry() = 0;
	virtual void SetMetricsRegistry(NF_SHARE_PTR<prometheus::Registry> pMetricsRegistry)  = 0;

	// prometheus数据采集
	virtual prometheus::Family<prometheus::Counter>& GetMetricsCounter(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Gauge>& GetMetricsGauge(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Histogram>& GetMetricsHistogram(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Summary>& GetMetricsSummary(int nTempIndex) = 0;

	virtual bool GetHistogramConfig(int nTempIndex, std::vector<double>& vecValue) = 0;
	virtual bool GetSummaryConfig(int nTempIndex, std::vector<QuantileInfo>& vecValue) = 0;

	virtual const std::string& GetTitleName() = 0;
	virtual void SetTitleName(const std::string& strTitleName) = 0;

	virtual const std::string& GetLogConfigName() const = 0;
	virtual void SetLogConfigName(const std::string& strName) = 0;

	virtual NFIPlugin* GetCurrentPlugin() = 0;
	virtual NFIModule* GetCurrenModule() = 0;

	virtual void SetCurrentPlugin(NFIPlugin* pPlugin) = 0;
	virtual void SetCurrenModule(NFIModule* pModule) = 0;

	virtual int GetFileConfig() = 0;
	virtual bool GetFileContent(const std::string &strFileName, std::string &strContent) = 0;

	virtual void SetExit(bool bExit) = 0;
	virtual bool GetExit() const = 0;
};

#endif
