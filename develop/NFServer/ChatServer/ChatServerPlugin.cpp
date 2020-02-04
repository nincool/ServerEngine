///--------------------------------------------------------------------
/// �ļ���:		ChatServerPlugin.cpp
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��8��30��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
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

//��������ģ��
void ChatServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, ChatModule, ChatModule)
	REGISTER_MODULE(pPluginManager, ChatServerInit, ChatServerInit)
	REGISTER_MODULE(pPluginManager, ChatNetMsgModule, ChatNetMsgModule)

}

//ж������ģ��
void ChatServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, ChatModule, ChatModule)
	UNREGISTER_MODULE(pPluginManager, ChatServerInit, ChatServerInit)
	UNREGISTER_MODULE(pPluginManager, ChatNetMsgModule, ChatNetMsgModule)
}