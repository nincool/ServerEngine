///--------------------------------------------------------------------
/// 文件名:		MasterServerPlugin.cpp
/// 内  容:		Master作为服务器插件
/// 说  明:		
/// 创建日期:	2019年9月6日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "MasterServerPlugin.h"
#include "MasterServer.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, MasterServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, MasterServerPlugin)
};

#endif

const int MasterServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string MasterServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(MasterServerPlugin);
}

void MasterServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, CMasterServer, CMasterServer);
}

void MasterServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, CMasterServer, CMasterServer);
}