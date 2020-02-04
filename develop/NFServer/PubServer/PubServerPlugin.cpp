///--------------------------------------------------------------------
/// 文件名:		PubServerPlugin.cpp
/// 内  容:		公共逻辑插件
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PubServerPlugin.h"
#include "PubKernel.h"
#include "PubInfoUpdateModule.h"
#include "PubServer.h"
#include "PubServerState.h"
#include "MaintainModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, PubServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, PubServerPlugin)
};

NF_EXPORT int DllGetVersion(NFIPluginManager* pm)
{
	return IPUB_LOGIC_VERSION;
}

#endif

const int PubServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string PubServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(PubServerPlugin);
}

void PubServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, IPubKernel, CPubKernel);
	REGISTER_MODULE(pPluginManager, CPubInfoUpdateModule, CPubInfoUpdateModule);
	REGISTER_MODULE(pPluginManager, CPubServer, CPubServer);
	REGISTER_MODULE(pPluginManager, CPubServerState, CPubServerState);
	REGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}

void PubServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, IPubKernel, CPubKernel);
	UNREGISTER_MODULE(pPluginManager, CPubInfoUpdateModule, CPubInfoUpdateModule);
	UNREGISTER_MODULE(pPluginManager, CPubServer, CPubServer);
	UNREGISTER_MODULE(pPluginManager, CPubServerState, CPubServerState);
	UNREGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}

