///--------------------------------------------------------------------
/// �ļ���:		RoomServerPlugin.cpp
/// ��  ��:		��������
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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

