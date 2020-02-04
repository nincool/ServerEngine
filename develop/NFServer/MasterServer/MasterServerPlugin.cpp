///--------------------------------------------------------------------
/// �ļ���:		MasterServerPlugin.cpp
/// ��  ��:		Master��Ϊ���������
/// ˵  ��:		
/// ��������:	2019��9��6��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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