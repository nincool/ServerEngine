///--------------------------------------------------------------------
/// �ļ���:		LoginServerPlugin.cpp
/// ��  ��:		LoginServerPlugin
/// ˵  ��:		
/// ��������:	2019��9��6��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "LoginServerPlugin.h"
#include "LoginServer.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, LoginServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, LoginServerPlugin)
};

#endif

const int LoginServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string LoginServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(LoginServerPlugin);
}

void LoginServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, CLoginServer, CLoginServer);
}

void LoginServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, CLoginServer, CLoginServer);
}
