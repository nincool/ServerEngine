///--------------------------------------------------------------------
/// 文件名:		PlatformServerPlugin.cpp
/// 内  容:		平台服逻辑插件
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PlatformServerPlugin.h"
#include "PlatformKernel.h"
#include "PlatformServer.h"
#include "PlatformServerState.h"
#include "NFComm/NFKernelPlugin/NFLuaScriptModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, PlatformServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, PlatformServerPlugin)
};

NF_EXPORT int DllGetVersion(NFIPluginManager* pm)
{
	return IPLATFORM_LOGIC_VERSION;
}

#endif

const int PlatformServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string PlatformServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(PlatformServerPlugin);
}

void PlatformServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, IPlatformKernel, CPlatformKernel);
	REGISTER_MODULE(pPluginManager, CPlatformServer, CPlatformServer);
	REGISTER_MODULE(pPluginManager, CPlatformServerState, CPlatformServerState);
}

void PlatformServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, IPlatformKernel, CPlatformKernel);
	UNREGISTER_MODULE(pPluginManager, CPlatformServer, CPlatformServer);
	UNREGISTER_MODULE(pPluginManager, CPlatformServerState, CPlatformServerState);
}

