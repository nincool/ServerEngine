///--------------------------------------------------------------------
/// 文件名:		NFLogPlugin.cpp
/// 内  容:		日志插件
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFLogPlugin.h"
#include "NFLogModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFLogPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFLogPlugin)
};

#endif

const int NFLogPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFLogPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFLogPlugin);
}

void NFLogPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, NFILogModule, NFLogModule)
}

void NFLogPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, NFILogModule, NFLogModule)
}