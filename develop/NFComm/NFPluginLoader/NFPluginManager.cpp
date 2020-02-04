///--------------------------------------------------------------------
/// 文件名:		NFPluginManager.cpp
/// 内  容:		插件管理器
/// 说  明:		
/// 创建日期:	2019年10月31日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFPluginManager.h"
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "Dependencies/RapidXML/rapidxml_iterators.hpp"
#include "Dependencies/RapidXML/rapidxml_print.hpp"
#include "Dependencies/RapidXML/rapidxml_utils.hpp"
#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFUtils/IniFile.h"
#include "../NFPluginModule/KConstDefine.h"

#pragma comment( lib, "ws2_32.lib" )

NFPluginManager::NFPluginManager() : NFIPluginManager()
{
	mnAppID = 0;
	mCurrentPlugin = nullptr;
	mCurrenModule = nullptr;
	mnInitTime = time(NULL);
	mnNowTime = mnInitTime;

	mstrConfigPath = "../";
}

NFPluginManager::~NFPluginManager()
{

}

bool NFPluginManager::LoadPlugin()
{
	PluginNameMap::iterator it = mPluginNameMap.begin();
	for (; it != mPluginNameMap.end(); ++it)
	{
		LoadPluginLibrary(it->first);
	}

	return true;
}

bool NFPluginManager::Awake()
{
	PluginInstanceMap::iterator itAfterInstance = mPluginInstanceMap.begin();
	for (; itAfterInstance != mPluginInstanceMap.end(); itAfterInstance++)
	{
		SetCurrentPlugin(itAfterInstance->second);
		itAfterInstance->second->Awake();
	}

	return true;
}

inline bool NFPluginManager::Init()
{
	PluginInstanceMap::iterator itInstance = mPluginInstanceMap.begin();
	for (; itInstance != mPluginInstanceMap.end(); itInstance++)
	{
		SetCurrentPlugin(itInstance->second);
		itInstance->second->Init();
	}

	return true;
}

void NFPluginManager::Registered(NFIPlugin* plugin)
{
	const std::string& strPluginName = plugin->GetPluginName();
	if (!FindPlugin(strPluginName))
	{
		mPluginInstanceMap.insert(PluginInstanceMap::value_type(strPluginName, plugin));
		plugin->Install();
	}
	else
	{
		std::cout << strPluginName << std::endl;
		assert(0);
	}
}

void NFPluginManager::UnRegistered(NFIPlugin* plugin)
{
	PluginInstanceMap::iterator it = mPluginInstanceMap.find(plugin->GetPluginName());
	if (it != mPluginInstanceMap.end())
	{
		it->second->Uninstall();
		delete it->second;
		it->second = NULL;
		mPluginInstanceMap.erase(it);
	}
}

bool NFPluginManager::ReLoadPlugin(const std::string& strPluginDLLName)
{
	//1.shut all module of this plugin
	//2.unload this plugin
	//3.load new plugin
	//4.init new module
	//5.tell others who has been reloaded
	PluginInstanceMap::iterator itInstance = mPluginInstanceMap.find(strPluginDLLName);
	if (itInstance == mPluginInstanceMap.end())
	{
		return false;
	}

	//1
	NFIPlugin* pPlugin = itInstance->second;
	NFIModule* pModule = pPlugin->First();
	while (pModule)
	{
		pModule->BeforeShut();
		pModule->Shut();
		pModule->Finalize();

		pModule = pPlugin->Next();
	}

	//2
	PluginLibMap::iterator it = mPluginLibMap.find(strPluginDLLName);
	if (it != mPluginLibMap.end())
	{
		NFDynLib* pLib = it->second;
		DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pLib->GetSymbol("DllStopPlugin");

		if (pFunc)
		{
			pFunc(this);
		}

		pLib->UnLoad();

		delete pLib;
		pLib = NULL;
		mPluginLibMap.erase(it);
	}

	//3
	NFDynLib* pLib = new NFDynLib(strPluginDLLName);
	bool bLoad = pLib->Load();
	if (bLoad)
	{
		mPluginLibMap.insert(PluginLibMap::value_type(strPluginDLLName, pLib));

		DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
		if (!pFunc)
		{
			std::cout << "Reload Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
			assert(0);
			return false;
		}

		pFunc(this);
	}
	else
	{
#if NF_PLATFORM == NF_PLATFORM_LINUX
		char* error = dlerror();
		if (error)
		{
			std::cout << stderr << " Reload shared lib[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
			std::cout << "Reload [" << pLib->GetName() << "] failed" << std::endl;
			assert(0);
			return false;
		}
#elif NF_PLATFORM == NF_PLATFORM_WIN
		std::cout << stderr << " Reload DLL[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
		std::cout << "Reload [" << pLib->GetName() << "] failed" << std::endl;
		assert(0);
		return false;
#endif // NF_PLATFORM
	}

	//4
	PluginInstanceMap::iterator itReloadInstance = mPluginInstanceMap.begin();
	for (; itReloadInstance != mPluginInstanceMap.end(); itReloadInstance++)
	{
		if (strPluginDLLName != itReloadInstance->first)
		{
			itReloadInstance->second->OnReloadPlugin();
		}
	}

	return true;
}

NFIPlugin* NFPluginManager::FindPlugin(const std::string& strPluginName)
{
	PluginInstanceMap::iterator it = mPluginInstanceMap.find(strPluginName);
	if (it != mPluginInstanceMap.end())
	{
		return it->second;
	}

	return NULL;
}

bool NFPluginManager::Execute()
{
	mnNowTime = time(NULL);

	bool bRet = true;

	PluginInstanceMap::iterator it = mPluginInstanceMap.begin();
	for (; it != mPluginInstanceMap.end(); ++it)
	{
		bool tembRet = it->second->Execute();
		bRet = bRet && tembRet;
	}

	return bRet;
}

inline int NFPluginManager::GetDistrictID() const
{
	return mnDistrictID;
}

inline void NFPluginManager::SetDistrictID(const int nDistrictID)
{
	if (nDistrictID <= 0)
	{
		std::cout << "Error District id" << nDistrictID << std::endl;
		assert(0);
		return;
	}

	mnDistrictID = nDistrictID;
}

inline int NFPluginManager::GetAppID() const
{
	return mnAppID;
}

inline void NFPluginManager::SetAppID(const int nAppID)
{
	if (nAppID <= 0)
	{
		std::cout << "Error APP id" << nAppID << std::endl;
		assert(0);
		return;
	}

	mnAppID = nAppID;
}

inline int NFPluginManager::GetServerType() const
{
	return mnServerType;
}
inline void NFPluginManager::SetServerType(const int nServerType)
{
	if (nServerType <= 0)
	{
		std::cout << "Error Server Type" << nServerType << std::endl;
		assert(0);
		return;
	}

	mnServerType = nServerType;
}

inline int NFPluginManager::GetServerID() const
{
	return mnServerID;
}

inline void NFPluginManager::SetServerID(const int nServerID)
{
	if (nServerID <= 0)
	{
		std::cout << "Error Server id" << nServerID << std::endl;
		assert(0);
		return;
	}

	mnServerID = nServerID;
}

inline NFINT64 NFPluginManager::GetInitTime() const
{
	return mnInitTime;
}

inline NFINT64 NFPluginManager::GetNowTime() const
{
	return mnNowTime;
}

inline const std::string& NFPluginManager::GetConfigPath() const
{
	return mstrConfigPath;
}

inline void NFPluginManager::SetConfigPath(const std::string& strPath)
{
	mstrConfigPath = strPath;
}

void NFPluginManager::SetConfigName(const std::string& strAppName)
{
	if (strAppName.empty())
	{
		return;
	}

	mstrConfigName = strAppName;
}

const std::string& NFPluginManager::GetConfigName() const
{
	return mstrConfigName;
}

const std::string& NFPluginManager::GetAppName() const
{
	return mstrAppName;
}

void NFPluginManager::SetAppName(const std::string& strAppName)
{
	if (!mstrAppName.empty())
	{
		return;
	}

	mstrAppName = strAppName;
}

const std::string& NFPluginManager::GetLuaName() const
{
	return mstrLuaName;
}

void NFPluginManager::SetLuaName(const std::string& strLuaName)
{
	if (!mstrLuaName.empty())
	{
		return;
	}

	mstrLuaName = strLuaName;
}

const std::string& NFPluginManager::GetMetricsInfo() const
{
	return mstrMetricsInfo;
}

void NFPluginManager::SetMetricsInfo(const std::string& strMetricsInfo)
{
	if (!mstrMetricsInfo.empty())
	{
		return;
	}

	mstrMetricsInfo = strMetricsInfo;
}

NF_SHARE_PTR<prometheus::Registry> NFPluginManager::GetMetricsRegistry()
{
	return pMetricsRegistry;
}

void NFPluginManager::SetMetricsRegistry(NF_SHARE_PTR<prometheus::Registry> pRegistry)
{
	pMetricsRegistry = pRegistry;
}

const std::string& NFPluginManager::GetTitleName()
{
	return mstrTitleName;
}

prometheus::Family<prometheus::Counter>& NFPluginManager::GetMetricsCounter(int nTempIndex)
{
	if (pMetricsRegistry == nullptr)
	{
		std::string errorInfo = "GetMetricsCounter pMetricsRegistry is NULL";
		throw std::invalid_argument(errorInfo);
	}

	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.find(nTempIndex);
	if (itFind == mMetricsMap.end())
	{
		std::string errorInfo = "GetMetricsCounter not find Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}
		
	if (itFind->second.GetType() != (int)prometheus::MetricType::Counter)
	{
		std::string errorInfo = "GetMetricsCounter not Counter Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	return prometheus::BuildCounter()
				.Name(itFind->second.GetName())
				.Help(itFind->second.GetHelp())
				.Labels(itFind->second.GetLables())
				.Register(*pMetricsRegistry);
}

prometheus::Family<prometheus::Gauge>& NFPluginManager::GetMetricsGauge(int nTempIndex)
{
	if (pMetricsRegistry == nullptr)
	{
		std::string errorInfo = "GetMetricsGauge pMetricsRegistry is NULL";
		throw std::invalid_argument(errorInfo);
	}

	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.find(nTempIndex);
	if (itFind == mMetricsMap.end())
	{
		std::string errorInfo = "GetMetricsGauge not find Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	if (itFind->second.GetType() != (int)prometheus::MetricType::Gauge)
	{
		std::string errorInfo = "GetMetricsGauge not Gauge Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	return prometheus::BuildGauge()
		.Name(itFind->second.GetName())
		.Help(itFind->second.GetHelp())
		.Labels(itFind->second.GetLables())
		.Register(*pMetricsRegistry);
}

prometheus::Family<prometheus::Histogram>& NFPluginManager::GetMetricsHistogram(int nTempIndex)
{
	if (pMetricsRegistry == nullptr)
	{
		std::string errorInfo = "GetMetricsHistogram pMetricsRegistry is NULL";
		throw std::invalid_argument(errorInfo);
	}

	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.find(nTempIndex);
	if (itFind == mMetricsMap.end())
	{
		std::string errorInfo = "GetMetricsHistogram not find Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	if (itFind->second.GetType() != (int)prometheus::MetricType::Histogram)
	{
		std::string errorInfo = "GetMetricsHistogram not Histogram Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	return prometheus::BuildHistogram()
		.Name(itFind->second.GetName())
		.Help(itFind->second.GetHelp())
		.Labels(itFind->second.GetLables())
		.Register(*pMetricsRegistry);
}

prometheus::Family<prometheus::Summary>& NFPluginManager::GetMetricsSummary(int nTempIndex)
{
	if (pMetricsRegistry == nullptr)
	{
		std::string errorInfo = "GetMetricsSummary pMetricsRegistry is NULL";
		throw std::invalid_argument(errorInfo);
	}

	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.find(nTempIndex);
	if (itFind == mMetricsMap.end())
	{
		std::string errorInfo = "GetMetricsSummary not find Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	if (itFind->second.GetType() != (int)prometheus::MetricType::Summary)
	{
		std::string errorInfo = "GetMetricsSummary not Summary Index:" + lexical_cast<std::string>(nTempIndex);
		throw std::invalid_argument(errorInfo);
	}

	return prometheus::BuildSummary()
		.Name(itFind->second.GetName())
		.Help(itFind->second.GetHelp())
		.Labels(itFind->second.GetLables())
		.Register(*pMetricsRegistry);
}

bool NFPluginManager::GetHistogramConfig(int nTempIndex, std::vector<double>& vecValue)
{
	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.find(nTempIndex);
	if (itFind == mMetricsMap.end())
	{
		return false;
	}

	if (itFind->second.GetType() != (int)prometheus::MetricType::Histogram)
	{
		return false;
	}

	vecValue = itFind->second.GetBuckets();
	return true;
}

bool NFPluginManager::GetSummaryConfig(int nTempIndex, std::vector<QuantileInfo>& vecValue)
{
	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.find(nTempIndex);
	if (itFind == mMetricsMap.end())
	{
		return false;
	}

	if (itFind->second.GetType() != (int)prometheus::MetricType::Summary)
	{
		return false;
	}

	std::map<double, double>::iterator iterFind = itFind->second.GetQuantiles().begin();
	while (iterFind != itFind->second.GetQuantiles().end())
	{
		QuantileInfo info{ iterFind->first, iterFind->second };
		vecValue.push_back(info);
		++iterFind;
	}

	return true;
}

std::string NFPluginManager::GetNodeValue(rapidxml::xml_attribute<>* pAttr)
{
	return (pAttr == nullptr || pAttr->value() == nullptr) ? "" : pAttr->value();
}

void NFPluginManager::LoadMetricsConfig()
{
	std::string strFile = GetConfigPath() + "NFDataCfg/MetricsDefine.xml";
	std::string strContent;
	if (!GetFileContent(strFile, strContent))
	{
		std::string errorInfo = "MetricsDefine.xml Not find:" + strFile;
		throw std::invalid_argument(errorInfo);
	}

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());

	rapidxml::xml_node<>* root = xDoc.first_node();
	for (rapidxml::xml_node<>* attrNode = root->first_node(); attrNode; attrNode = attrNode->next_sibling())
	{
		Metrics MetricsData;
		std::string nIDIndex = GetNodeValue(attrNode->first_attribute("Id"));
		if (nIDIndex.empty())
		{
			throw std::invalid_argument("MetricsDefine.xml Id empty");
		}

		//获取metrics类型
		std::string strMetricsType = GetNodeValue(attrNode->first_attribute("Type"));
		if (strMetricsType.empty())
		{
			std::string errorInfo = "MetricsDefine.xml Type empty Index:" + nIDIndex;
			throw std::invalid_argument(errorInfo);
		}

		int nMetricsType = GetMetricsType(strMetricsType);
		if (nMetricsType < 0)
		{
			std::string errorInfo = "MetricsDefine.xml Type not find Index:" + nIDIndex;
			throw std::invalid_argument(errorInfo);
		}
		MetricsData.SetType(nMetricsType);

		//获取名字
		std::string strMetricsName = GetNodeValue(attrNode->first_attribute("Name"));
		if (strMetricsName.empty())
		{
			std::string errorInfo = "MetricsDefine.xml name empty Index:" + nIDIndex;
			throw std::invalid_argument(errorInfo);
		}
		MetricsData.SetName(strMetricsName);

		//获取Help
		std::string strMetricsHelp = GetNodeValue(attrNode->first_attribute("Help"));
		if (strMetricsHelp.empty())
		{
			std::string errorInfo = "MetricsDefine.xml help empty Index:" + nIDIndex;
			throw std::invalid_argument(errorInfo);
		}
		MetricsData.SetHelp(strMetricsHelp);

		//获取Lables
		std::string strMetricsLables = GetNodeValue(attrNode->first_attribute("Lables"));
		std::map<std::string, std::string> map_Temp;
		map_Temp.insert(make_pair("Name", mstrAppName));
		if (!strMetricsLables.empty())
		{
			std::vector<std::string> vec_Lables = StringUtil::SplitString(strMetricsLables, ",");
			for (int i = 0; i < vec_Lables.size(); ++i)
			{
				std::vector<std::string> vec_value = StringUtil::SplitString(vec_Lables[i], ":");
				if (vec_value.size() == 2)
				{
					map_Temp.insert(make_pair(vec_value[0], vec_value[1]));
				}
			}	
		}
		MetricsData.SetLables(move(map_Temp));

		//获取BucketBoundaries
		std::string strMetricsBucketBoundaries = GetNodeValue(attrNode->first_attribute("BucketBoundaries"));
		if (!strMetricsBucketBoundaries.empty())
		{
			std::vector<double> vec_temp;
			std::vector<std::string> vec_BucketBoundaries = StringUtil::SplitString(strMetricsBucketBoundaries, ":");
			for (int i = 0; i < vec_BucketBoundaries.size(); ++i)
			{
				vec_temp.push_back(StringUtil::StringAsFloat(vec_BucketBoundaries[i].c_str()));
			}
			MetricsData.SetBuckets(move(vec_temp));
		}

		std::string strMetricsQuantiles = GetNodeValue(attrNode->first_attribute("Quantiles"));
		if (!strMetricsQuantiles.empty())
		{
			std::map<double, double> vec_Temp;
			std::vector<std::string> vec_Quantiles = StringUtil::SplitString(strMetricsQuantiles, ",");
			for (int i = 0; i < vec_Quantiles.size(); ++i)
			{
				std::vector<std::string> vec_value = StringUtil::SplitString(vec_Quantiles[i], ":");
				if (vec_value.size() == 2)
				{
					vec_Temp.insert(make_pair(StringUtil::StringAsFloat(vec_value[0].c_str()),
						StringUtil::StringAsFloat(vec_value[1].c_str())));
				}
			}
			MetricsData.SetQuantiles(move(vec_Temp));
		}

		mMetricsMap.insert(make_pair(lexical_cast<int>(nIDIndex), MetricsData));
	}
}

void NFPluginManager::InitExporter()
{
	std::unordered_map<int, Metrics>::iterator itFind = mMetricsMap.begin();
	while (itFind != mMetricsMap.end())
	{
		if (itFind->second.GetType() == (int)prometheus::MetricType::Counter)
		{
			prometheus::BuildCounter()
				.Name(itFind->second.GetName())
				.Help(itFind->second.GetHelp())
				.Labels(itFind->second.GetLables())
				.Register(*pMetricsRegistry);
		}
		else if (itFind->second.GetType() == (int)prometheus::MetricType::Gauge)
		{
			prometheus::BuildGauge()
				.Name(itFind->second.GetName())
				.Help(itFind->second.GetHelp())
				.Labels(itFind->second.GetLables())
				.Register(*pMetricsRegistry);
		}
		else if (itFind->second.GetType() == (int)prometheus::MetricType::Summary)
		{
			prometheus::BuildSummary()
				.Name(itFind->second.GetName())
				.Help(itFind->second.GetHelp())
				.Labels(itFind->second.GetLables())
				.Register(*pMetricsRegistry);
		}
		else if (itFind->second.GetType() == (int)prometheus::MetricType::Histogram)
		{
			prometheus::BuildHistogram()
				.Name(itFind->second.GetName())
				.Help(itFind->second.GetHelp())
				.Labels(itFind->second.GetLables())
				.Register(*pMetricsRegistry);
		}

		++itFind;
	}
}

void NFPluginManager::SetTitleName(const std::string& strTitleName)
{
	if (!mstrTitleName.empty())
	{
		return;
	}

	mstrTitleName = strTitleName;
}

const std::string& NFPluginManager::GetLogConfigName() const
{
	return mstrLogConfigName;
}

void NFPluginManager::SetLogConfigName(const std::string& strName)
{
	mstrLogConfigName = strName;
}

NFIPlugin* NFPluginManager::GetCurrentPlugin()
{
	return mCurrentPlugin;
}

NFIModule* NFPluginManager::GetCurrenModule()
{
	return mCurrenModule;
}

void NFPluginManager::SetCurrentPlugin(NFIPlugin* pPlugin)
{
	mCurrentPlugin = pPlugin;
}

void NFPluginManager::SetCurrenModule(NFIModule* pModule)
{
	mCurrenModule = pModule;
}

bool NFPluginManager::GetFileContent(const std::string& strFileName, std::string& strContent)
{
	FILE* fp = fopen(strFileName.c_str(), "rb");
	if (!fp)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	const long filelength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	strContent.resize(filelength);
	fread((void*)strContent.data(), filelength, 1, fp);
	fclose(fp);

	return true;
}

void NFPluginManager::AddModule(const std::string& strModuleName, NFIModule* pModule)
{
    if (!FindModule(strModuleName))
    {
        mModuleInstanceMap.insert(ModuleInstanceMap::value_type(strModuleName, pModule));
    }
}

void NFPluginManager::RemoveModule(const std::string& strModuleName)
{
	ModuleInstanceMap::iterator it = mModuleInstanceMap.find(strModuleName);
	if (it != mModuleInstanceMap.end())
	{
		mModuleInstanceMap.erase(it);
	}
}

NFIModule* NFPluginManager::FindModule(const std::string& strModuleName)
{
	std::string strSubModuleName = strModuleName;

	std::size_t position = strSubModuleName.find(" ");
	if (string::npos != position)
	{
		strSubModuleName = strSubModuleName.substr(position + 1, strSubModuleName.length());
	}

	ModuleInstanceMap::iterator it = mModuleInstanceMap.find(strSubModuleName);
	if (it != mModuleInstanceMap.end())
	{
		return it->second;
	}

	if (this->GetCurrenModule())
	{
		std::cout << this->GetCurrenModule()->strName << " want to find module: " << strModuleName << " but got null_ptr!!!" << std::endl;
	}

	return NULL;
}

std::list<NFIModule*> NFPluginManager::Modules()
{
	std::list<NFIModule*> xModules;

	ModuleInstanceMap::iterator itCheckInstance = mModuleInstanceMap.begin();
	for (; itCheckInstance != mModuleInstanceMap.end(); itCheckInstance++)
	{
		xModules.push_back(itCheckInstance->second);
	}

	return xModules;
}

bool NFPluginManager::InitLua()
{
	PluginInstanceMap::iterator itCheckInstance = mPluginInstanceMap.begin();
	for (; itCheckInstance != mPluginInstanceMap.end(); itCheckInstance++)
	{
		SetCurrentPlugin(itCheckInstance->second);
		itCheckInstance->second->InitLua();
	}

	return true;
}

bool NFPluginManager::AfterInit()
{
	PluginInstanceMap::iterator itAfterInstance = mPluginInstanceMap.begin();
	for (; itAfterInstance != mPluginInstanceMap.end(); itAfterInstance++)
	{
		SetCurrentPlugin(itAfterInstance->second);
		itAfterInstance->second->AfterInit();
	}

	return true;
}

bool NFPluginManager::InitNetServer()
{
	PluginInstanceMap::iterator itAfterInstance = mPluginInstanceMap.begin();
	for (; itAfterInstance != mPluginInstanceMap.end(); itAfterInstance++)
	{
		SetCurrentPlugin(itAfterInstance->second);
		itAfterInstance->second->InitNetServer();
	}

	return true;
}

bool NFPluginManager::InitNetClient()
{
	PluginInstanceMap::iterator itAfterInstance = mPluginInstanceMap.begin();
	for (; itAfterInstance != mPluginInstanceMap.end(); itAfterInstance++)
	{
		SetCurrentPlugin(itAfterInstance->second);
		itAfterInstance->second->InitNetClient();
	}

	return true;
}

bool NFPluginManager::ReadyExecute()
{
	PluginInstanceMap::iterator itCheckInstance = mPluginInstanceMap.begin();
	for (; itCheckInstance != mPluginInstanceMap.end(); itCheckInstance++)
	{
		SetCurrentPlugin(itCheckInstance->second);
		itCheckInstance->second->ReadyExecute();
	}

	return true;
}

bool NFPluginManager::BeforeShut()
{
	PluginInstanceMap::iterator itBeforeInstance = mPluginInstanceMap.begin();
	for (; itBeforeInstance != mPluginInstanceMap.end(); itBeforeInstance++)
	{
		SetCurrentPlugin(itBeforeInstance->second);
		itBeforeInstance->second->BeforeShut();
	}

	return true;
}

bool NFPluginManager::Shut()
{
	PluginInstanceMap::iterator itInstance = mPluginInstanceMap.begin();
	for (; itInstance != mPluginInstanceMap.end(); ++itInstance)
	{
		SetCurrentPlugin(itInstance->second);
		itInstance->second->Shut();
	}

	return true;
}

bool NFPluginManager::Finalize()
{
	for (auto& it : mPluginInstanceMap)
	{
		SetCurrentPlugin(it.second);
		it.second->Finalize();
	}

	// 停止所有lib
	for (auto& it : mPluginLibMap)
	{
		NFDynLib* pLib = it.second;
		DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pLib->GetSymbol("DllStopPlugin");
		if (pFunc)
		{
			pFunc(this);
		}
	}

	// 卸载所有lib
	for (auto& it : mPluginLibMap)
	{
		NFDynLib* pLib = it.second;
		pLib->UnLoad();
		delete pLib;
		pLib = NULL;
	}

	mPluginLibMap.clear();
	mPluginInstanceMap.clear();
	mPluginNameMap.clear();

	return true;
}

bool NFPluginManager::CommandShut()
{
	PluginInstanceMap::iterator itInstance = mPluginInstanceMap.begin();
	for (; itInstance != mPluginInstanceMap.end(); ++itInstance)
	{
		SetCurrentPlugin(itInstance->second);
		itInstance->second->CommandShut();
	}

	return true;
}

bool NFPluginManager::LoadPluginLibrary(const std::string& strPluginDLLName)
{
	PluginLibMap::iterator it = mPluginLibMap.find(strPluginDLLName);
	if (it == mPluginLibMap.end())
	{
		NFDynLib* pLib = new NFDynLib(strPluginDLLName);
		bool bLoad = pLib->Load();

		if (bLoad)
		{
			mPluginLibMap.insert(PluginLibMap::value_type(strPluginDLLName, pLib));

			//查看开启的版本设置
			DLL_GET_VERSION_FUNC pFunc_GetVersion = (DLL_GET_VERSION_FUNC)pLib->GetSymbol("DllGetVersion");
			if (pFunc_GetVersion)
			{
				if (mnVersion == 0)
				{
					mnVersion = pFunc_GetVersion(this);

					std::cout << strPluginDLLName << " Version:" << mnVersion << std::endl;
				}
				else
				{
					int nOtherVersion = pFunc_GetVersion(this);
					if (nOtherVersion != mnVersion)
					{
						std::cout << "Dll Version Not Same:" << nOtherVersion << "," << mnVersion << std::endl;
						assert(0);
						return false;
					}	
					std::cout << strPluginDLLName << " Version:" << nOtherVersion << std::endl;
				}
			}

			DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
			if (!pFunc)
			{
				std::cout << "Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
				assert(0);
				return false;
			}

			pFunc(this);

			return true;
		}
		else
		{
			std::cout << stderr << " Load DLL[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
			std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
			assert(0);
			return false;
		}
	}

	return false;
}

bool NFPluginManager::UnLoadPluginLibrary(const std::string& strPluginDLLName)
{
	PluginLibMap::iterator it = mPluginLibMap.find(strPluginDLLName);
	if (it != mPluginLibMap.end())
	{
		NFDynLib* pLib = it->second;
		DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pLib->GetSymbol("DllStopPlugin");

		if (pFunc)
		{
			pFunc(this);
		}

		pLib->UnLoad();

		delete pLib;
		pLib = NULL;
		mPluginLibMap.erase(it);

		return true;
	}

	return false;
}

int NFPluginManager::GetFileConfig()
{
	std::string strFilePath = GetConfigPath() + "Lua/" + mstrConfigName + ".ini";
	CIniFile ini(strFilePath.c_str());
	if (!ini.LoadFromFile())
	{
		std::cout << "Load File Error:" << mstrConfigName << std::endl;
		assert(0);
		return 0;
	}

	mnServerID = ini.ReadInteger("Main", "ServerID", 1);
	mnDistrictID = ini.ReadInteger("Main", "DistrictID");
	mstrAppName = ini.ReadString("System", "AppName");
	mstrLuaName = ini.ReadString("System", "LuaName");

	mnAppID = ini.ReadInteger("Main", "AppID");
	if (mnAppID == 0)
	{
		//从代码直接设置
		std::map<std::string, NF_SERVER_TYPES>::iterator itFind = g_ServerMap.find(mstrAppName);
		if (itFind != g_ServerMap.end())
		{
			mnAppID = itFind->second;
		}
	}

	mnServerType = ini.ReadInteger("Main", "ServerType");
	if (mnServerType == 0)
	{
		//从代码直接设置
		std::map<std::string, NF_SERVER_TYPES>::iterator itFind = g_ServerMap.find(mstrAppName);
		if (itFind != g_ServerMap.end())
		{
			mnServerType = itFind->second;
		}
	}

	bool bMetrics = ini.ReadBool("Metrics", "Inuse");
	if (bMetrics)
	{
		std::string strMetricsIP = ini.ReadString("Metrics", "Addr");
		int nMetricsPort = ini.ReadInteger("Metrics", "Port");
		mstrMetricsInfo = strMetricsIP + ":" + lexical_cast<std::string>(nMetricsPort);
	}

	int nSection_Index = ini.FindSection("Module");
	int nSection_Count = ini.GetSectItemCount(nSection_Index);
	for (int i = 0; i < nSection_Count; ++i)
	{
		std::string strPluginName = ini.GetSectionDataKey(nSection_Index, i);
		mPluginNameMap.insert(PluginNameMap::value_type(strPluginName.c_str(), true));
	}

	return 0;
}

void NFPluginManager::SetExit(bool bExit)
{
	m_bExitApp = bExit;
}

bool NFPluginManager::GetExit() const
{
	return m_bExitApp;
}

int NFPluginManager::GetMetricsType(std::string& value)
{
	if (stricmp(value.c_str(), "Counter") == 0)
	{
		return (int)prometheus::MetricType::Counter;
	}
	else if (stricmp(value.c_str(), "Gauge") == 0)
	{
		return (int)prometheus::MetricType::Gauge;
	}
	else if (stricmp(value.c_str(), "Summary") == 0)
	{
		return (int)prometheus::MetricType::Summary;
	}
	else if (stricmp(value.c_str(), "Histogram") == 0)
	{
		return (int)prometheus::MetricType::Histogram;
	}
	else
	{
		return -1;
	}
}