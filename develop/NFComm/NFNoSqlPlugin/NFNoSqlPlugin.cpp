///--------------------------------------------------------------------
/// 文件名:		NFNoSqlPlugin.cpp
/// 内  容:		数据库插件
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFNoSqlPlugin.h"
#include "NFNoSqlModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFNoSqlPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFNoSqlPlugin)
};

#endif

const int NFNoSqlPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFNoSqlPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFNoSqlPlugin);
}

void NFNoSqlPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, NFINoSqlModule, NFNoSqlModule)
}

void NFNoSqlPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, NFINoSqlModule, NFNoSqlModule)
}