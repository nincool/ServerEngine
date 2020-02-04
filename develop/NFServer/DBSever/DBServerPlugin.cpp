///--------------------------------------------------------------------
/// 文件名:		DBServerPlugin.cpp
/// 内  容:		数据库插件
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "DBServerPlugin.h"
#include "PlayerRedis.h"
#include "CommonRedis.h"
#include "AccountRedis.h"
#include "PubRedis.h"
#include "LetterRedis.h"
#include "LogToDB.h"
#include "DBServerState.h"
#include "DBServer.h"
#include "PlatformRedis.h"
#include "ColdData.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, DBServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, DBServerPlugin)
};

#endif

const int DBServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string DBServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(DBServerPlugin);
}

void DBServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, CPubRedis, CPubRedis);
	REGISTER_MODULE(pPluginManager, CPlayerRedis, CPlayerRedis);
	REGISTER_MODULE(pPluginManager, ICommonRedis, CCommonRedis);
	REGISTER_MODULE(pPluginManager, IAccountRedis, CAccountRedis);
	REGISTER_MODULE(pPluginManager, ILetterRedis, CLetterRedis);
	REGISTER_MODULE(pPluginManager, LogToDBModule, LogToDBModule);
	REGISTER_MODULE(pPluginManager, CDBServerState, CDBServerState);
	REGISTER_MODULE(pPluginManager, CDBServer, CDBServer);
	REGISTER_MODULE(pPluginManager, CPlatformRedis, CPlatformRedis); 
	REGISTER_MODULE(pPluginManager, CColdData, CColdData);
}

void DBServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, CPubRedis, CPubRedis);
	UNREGISTER_MODULE(pPluginManager, CPlayerRedis, CPlayerRedis);
	UNREGISTER_MODULE(pPluginManager, ICommonRedis, CCommonRedis);
	UNREGISTER_MODULE(pPluginManager, IAccountRedis, CAccountRedis);
	UNREGISTER_MODULE(pPluginManager, ILetterRedis, CLetterRedis);
	UNREGISTER_MODULE(pPluginManager, LogToDBModule, LogToDBModule);
	UNREGISTER_MODULE(pPluginManager, CDBServerState, CDBServerState);
	UNREGISTER_MODULE(pPluginManager, CDBServer, CDBServer);
	UNREGISTER_MODULE(pPluginManager, CPlatformRedis, CPlatformRedis);
	UNREGISTER_MODULE(pPluginManager, CColdData, CColdData);
}
