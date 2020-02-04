///--------------------------------------------------------------------
/// 文件名:		ProxyServerPlugin.cpp
/// 内  容:		代理服务器插件
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "ProxyServerPlugin.h"
#include "ProxyClientLogicModule.h"
#include "ProxyServerLogicModule.h"
#include "ProxyServer.h"
#include "ProxyServerState.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, ProxyServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, ProxyServerPlugin)
};

#endif

const int ProxyServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string ProxyServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(ProxyServerPlugin);
}

void ProxyServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, CProxyClientLogicModule, CProxyClientLogicModule);
	REGISTER_MODULE(pPluginManager, CProxyServerLogicModule, CProxyServerLogicModule);
	REGISTER_MODULE(pPluginManager, CProxyServer, CProxyServer);
	REGISTER_MODULE(pPluginManager, CProxyServerState, CProxyServerState);
}

void ProxyServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, CProxyClientLogicModule, CProxyClientLogicModule);
	UNREGISTER_MODULE(pPluginManager, CProxyServerLogicModule, CProxyServerLogicModule);
	UNREGISTER_MODULE(pPluginManager, CProxyServer, CProxyServer);
	UNREGISTER_MODULE(pPluginManager, CProxyServerState, CProxyServerState);
}
