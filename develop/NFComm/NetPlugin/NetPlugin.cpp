///--------------------------------------------------------------------
/// 文件名:		NetPlugin.cpp
/// 内  容:		网络库插件
/// 说  明:		
/// 创建日期:	2019年11月12日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NetPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NetClientModule.h"
#include "NetServerInsideModule.h"
#include "NetServerOuterModule.h"
#include "NetServerMaintainModule.h"
#include "NFHttpServerModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NetPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NetPlugin)
};

#endif

const int NetPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NetPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NetPlugin);
}

void NetPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, NFIHttpServerModule, NFHttpServerModule)
	REGISTER_MODULE(pPluginManager, NetClientModule, NetClientModule)
	REGISTER_MODULE(pPluginManager, NetServerInsideModule, NetServerInsideModule)
	REGISTER_MODULE(pPluginManager, NetServerOuterModule, NetServerOuterModule)
	REGISTER_MODULE(pPluginManager, NetServerMaintainModule, NetServerMaintainModule)
}

void NetPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, NFIHttpServerModule, NFHttpServerModule)
	UNREGISTER_MODULE(pPluginManager, NetClientModule, NetClientModule)
	UNREGISTER_MODULE(pPluginManager, NetServerInsideModule, NetServerInsideModule)
	UNREGISTER_MODULE(pPluginManager, NetServerOuterModule, NetServerOuterModule)
	UNREGISTER_MODULE(pPluginManager, NetServerMaintainModule, NetServerMaintainModule)
}