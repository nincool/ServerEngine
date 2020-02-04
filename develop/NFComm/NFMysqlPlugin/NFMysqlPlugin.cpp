///--------------------------------------------------------------------
/// 文件名:		NFMysqlPlugin.cpp
/// 内  容:		mysql数据库操作插件
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFMysqlPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFMysqlModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFMysqlPlugin)

};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFMysqlPlugin)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int NFMysqlPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFMysqlPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFMysqlPlugin);
}

void NFMysqlPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, NFMysqlModule, NFMysqlModule)
}

void NFMysqlPlugin::Uninstall()
{

    UNREGISTER_MODULE(pPluginManager, NFMysqlModule, NFMysqlModule)
}