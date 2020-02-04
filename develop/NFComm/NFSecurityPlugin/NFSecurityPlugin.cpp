///--------------------------------------------------------------------
/// �ļ���:		NFSecurityPlugin.cpp
/// ��  ��:		���ܲ��
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFSecurityModule.h"
#include "NFSecurityPlugin.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{

    CREATE_PLUGIN(pm, NFSecurityPlugin)

};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFSecurityPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int NFSecurityPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFSecurityPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFSecurityPlugin);
}

void NFSecurityPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, NFISecurityModule, NFSecurityModule)

}

void NFSecurityPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, NFISecurityModule, NFSecurityModule)
}