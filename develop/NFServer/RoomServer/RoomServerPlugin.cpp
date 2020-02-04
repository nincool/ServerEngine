///--------------------------------------------------------------------
/// 文件名:		RoomServerPlugin.cpp
/// 内  容:		房间服插件
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "RoomServerPlugin.h"
#include "RoomKernel.h"
#include "RoomServer.h"
#include "RoomServerState.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, RoomServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, RoomServerPlugin)
};

NF_EXPORT int DllGetVersion(NFIPluginManager* pm)
{
	return IROOM_LOGIC_VERSION;
}

#endif

const int RoomServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string RoomServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(RoomServerPlugin);
}

void RoomServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, IRoomKernel, CRoomKernel);
	REGISTER_MODULE(pPluginManager, CRoomServer, CRoomServer);
	REGISTER_MODULE(pPluginManager, CRoomServerState, CRoomServerState);
}

void RoomServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, IRoomKernel, CRoomKernel);
	UNREGISTER_MODULE(pPluginManager, CRoomServer, CRoomServer);
	UNREGISTER_MODULE(pPluginManager, CRoomServerState, CRoomServerState);
}

