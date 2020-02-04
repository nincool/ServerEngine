///--------------------------------------------------------------------
/// 文件名:		LogServerPlugin.cpp
/// 内  容:		异步共享日志
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "LogServerPlugin.h"
#include "LogServer.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, LogServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, LogServerPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int LogServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string LogServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(LogServerPlugin);
}

void LogServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, CLogServer, CLogServer)
}

void LogServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, CLogServer, CLogServer)
}