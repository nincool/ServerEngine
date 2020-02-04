///--------------------------------------------------------------------
/// 文件名:		ChatServerPlugin.cpp
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年8月30日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ChatServerPlugin.h"
#include "ChatNetMsgModule.h"
#include "ChatServerInit.h"
#include "ChatModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
	CREATE_PLUGIN(pm, ChatServerPlugin)

};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
	DESTROY_PLUGIN(pm, ChatServerPlugin)
};

#endif


ChatServerPlugin::ChatServerPlugin(NFIPluginManager* p)
{
	pPluginManager = p;
}

const int ChatServerPlugin::GetPluginVersion()
{
	return 0;
}

const std::string ChatServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(ChatServerPlugin);
}

//启动共他模块
void ChatServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, ChatModule, ChatModule)
	REGISTER_MODULE(pPluginManager, ChatServerInit, ChatServerInit)
	REGISTER_MODULE(pPluginManager, ChatNetMsgModule, ChatNetMsgModule)

}

//卸载其他模块
void ChatServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, ChatModule, ChatModule)
	UNREGISTER_MODULE(pPluginManager, ChatServerInit, ChatServerInit)
	UNREGISTER_MODULE(pPluginManager, ChatNetMsgModule, ChatNetMsgModule)
}