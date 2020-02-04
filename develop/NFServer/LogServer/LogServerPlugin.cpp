///--------------------------------------------------------------------
/// �ļ���:		LogServerPlugin.cpp
/// ��  ��:		�첽������־
/// ˵  ��:		
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "LogServerPlugin.h"
#include "LogServer.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, LogServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, LogServerPlugin)
};

#endif

//////////////////////////////////////////////////////////////////////////

const int LogServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string LogServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(LogServerPlugin);
}

void LogServerPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, CLogServer, CLogServer)
}

void LogServerPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, CLogServer, CLogServer)
}