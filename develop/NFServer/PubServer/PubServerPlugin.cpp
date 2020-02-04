///--------------------------------------------------------------------
/// �ļ���:		PubServerPlugin.cpp
/// ��  ��:		�����߼����
/// ˵  ��:		
/// ��������:	2019��8��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "PubServerPlugin.h"
#include "PubKernel.h"
#include "PubInfoUpdateModule.h"
#include "PubServer.h"
#include "PubServerState.h"
#include "MaintainModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, PubServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, PubServerPlugin)
};

NF_EXPORT int DllGetVersion(NFIPluginManager* pm)
{
	return IPUB_LOGIC_VERSION;
}

#endif

const int PubServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string PubServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(PubServerPlugin);
}

void PubServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, IPubKernel, CPubKernel);
	REGISTER_MODULE(pPluginManager, CPubInfoUpdateModule, CPubInfoUpdateModule);
	REGISTER_MODULE(pPluginManager, CPubServer, CPubServer);
	REGISTER_MODULE(pPluginManager, CPubServerState, CPubServerState);
	REGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}

void PubServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, IPubKernel, CPubKernel);
	UNREGISTER_MODULE(pPluginManager, CPubInfoUpdateModule, CPubInfoUpdateModule);
	UNREGISTER_MODULE(pPluginManager, CPubServer, CPubServer);
	UNREGISTER_MODULE(pPluginManager, CPubServerState, CPubServerState);
	UNREGISTER_MODULE(pPluginManager, MaintainModule, MaintainModule);
}

