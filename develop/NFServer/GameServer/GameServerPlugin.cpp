
///--------------------------------------------------------------------
/// 文件名:		GameServerPlugin.cpp
/// 内  容:		GameServerPlugin
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "GameServerPlugin.h"
#include "CreateRoleModule.h"
#include "UpdateVisInfoModule.h"
#include "UpdateViewportModule.h"
#include "GameKernel.h"
#include "PubDataModule.h"
#include "UpdatePubData.h"
#include "GameServer.h"
#include "GameServerState.h"
#include "MaintainModule.h"

#ifdef NF_DYNAMIC_PLUGIN
NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, GameServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, GameServerPlugin)
};

NF_EXPORT int DllGetVersion(NFIPluginManager* pm)
{
	return IGAME_LOGIC_VERSION;
}

#endif
const int GameServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string GameServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(GameServerPlugin);
}

void GameServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, ICreateRoleModule, CCreateRoleModule);
	REGISTER_MODULE(pPluginManager, CUpdateVisInfoModule, CUpdateVisInfoModule);
	REGISTER_MODULE(pPluginManager, CUpdateViewportModule, CUpdateViewportModule);
	REGISTER_MODULE(pPluginManager, IGameKernel, CGameKernel);
	REGISTER_MODULE(pPluginManager, PlayerRenameModule, PlayerRenameModule);
	REGISTER_MODULE(pPluginManager, CPubDataModule, CPubDataModule);
	REGISTER_MODULE(pPluginManager, CUpdatePubData, CUpdatePubData);
	REGISTER_MODULE(pPluginManager, CGameServer, CGameServer);
	REGISTER_MODULE(pPluginManager, CGameServerState, CGameServerState);
	REGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
	
}

void GameServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, ICreateRoleModule, CCreateRoleModule);
	UNREGISTER_MODULE(pPluginManager, CUpdateVisInfoModule, CUpdateVisInfoModule);
	UNREGISTER_MODULE(pPluginManager, CUpdateViewportModule, CUpdateViewportModule);
	UNREGISTER_MODULE(pPluginManager, IGameKernel, CGameKernel);
	UNREGISTER_MODULE(pPluginManager, PlayerRenameModule, PlayerRenameModule);	
	UNREGISTER_MODULE(pPluginManager, CPubDataModule, CPubDataModule);
	UNREGISTER_MODULE(pPluginManager, CUpdatePubData, CUpdatePubData);
	UNREGISTER_MODULE(pPluginManager, CGameServer, CGameServer);
	UNREGISTER_MODULE(pPluginManager, CGameServerState, CGameServerState);
	UNREGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}

