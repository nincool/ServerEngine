///--------------------------------------------------------------------
/// �ļ���:		WorldServerPlugin.cpp
/// ��  ��:		������������
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "WorldPlayers.h"
#include "WorldClientLogicModule.h"
#include "WorldServerPlugin.h"
#include "WorldServerLogicModule.h"
#include "WorldServer.h"
#include "WorldServerState.h"
#include "MaintainModule.h"

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, WorldServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, WorldServerPlugin)
};

const int WorldServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string WorldServerPlugin::GetPluginName()
{
    return GET_CLASS_NAME(WorldServerPlugin);
}

void WorldServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, CWorldPlayers, CWorldPlayers);
	REGISTER_MODULE(pPluginManager, CWorldClientLogicModule, CWorldClientLogicModule);
	REGISTER_MODULE(pPluginManager, CWorldServerLogicModule, CWorldServerLogicModule);
	REGISTER_MODULE(pPluginManager, CWorldServer, CWorldServer);
	REGISTER_MODULE(pPluginManager, CWorldServerState, CWorldServerState);
	REGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}

void WorldServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, CWorldPlayers, CWorldPlayers);
	UNREGISTER_MODULE(pPluginManager, NetCommandWSModule, NetCommandWSModule);
	UNREGISTER_MODULE(pPluginManager, CWorldClientLogicModule, CWorldClientLogicModule);
	UNREGISTER_MODULE(pPluginManager, CWorldServerLogicModule, CWorldServerLogicModule);
	UNREGISTER_MODULE(pPluginManager, CWorldServer, CWorldServer);
	UNREGISTER_MODULE(pPluginManager, CWorldServerState, CWorldServerState);
	UNREGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}
