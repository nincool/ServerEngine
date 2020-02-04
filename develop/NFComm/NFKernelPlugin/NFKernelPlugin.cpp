///--------------------------------------------------------------------
/// 文件名:		NFKernelPlugin.cpp
/// 内  容:		核心插件
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFKernelPlugin.h"
#include "NFKernelModule.h"
#include "NFEventModule.h"
#include "NFHeartBeatModule.h"
#include "ExtraServerModule.h"
#include "PerformanceCountModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFKernelPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFKernelPlugin)
};

#endif

const int NFKernelPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFKernelPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFKernelPlugin);
}

void NFKernelPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, NFIKernelModule, NFKernelModule);
	REGISTER_MODULE(pPluginManager, NFIEventModule, NFEventModule);
	REGISTER_MODULE(pPluginManager, NFIHeartBeatModule, NFHeartBeatModule);
	REGISTER_MODULE(pPluginManager, CExtraServerModule, CExtraServerModule);
	REGISTER_MODULE(pPluginManager, PerformanceCountModule, PerformanceCountModule);
}

void NFKernelPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, NFIEventModule, NFEventModule);
	UNREGISTER_MODULE(pPluginManager, NFIKernelModule, NFKernelModule);
	UNREGISTER_MODULE(pPluginManager, NFIHeartBeatModule, NFHeartBeatModule);
	UNREGISTER_MODULE(pPluginManager, CExtraServerModule, CExtraServerModule);
	UNREGISTER_MODULE(pPluginManager, PerformanceCountModule, PerformanceCountModule);
}