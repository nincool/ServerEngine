///--------------------------------------------------------------------
/// �ļ���:		NFConfigPlugin.cpp
/// ��  ��:		���ò��
/// ˵  ��:		
/// ��������:	2019��11��12��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFConfigPlugin.h"
#include "NFClassModule.h"
#include "NFElementModule.h"
#include "NFResModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFConfigPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFConfigPlugin)
};

#endif

NFConfigPlugin::NFConfigPlugin(NFIPluginManager* p)
{
    pPluginManager = p;
}

const int NFConfigPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFConfigPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFConfigPlugin);
}

void NFConfigPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, NFIClassModule, NFClassModule)
	REGISTER_MODULE(pPluginManager, NFIElementModule, NFElementModule)
	REGISTER_MODULE(pPluginManager, NFIResModule, NFResModule)
}

void NFConfigPlugin::Uninstall()
{
    UNREGISTER_MODULE(pPluginManager, NFIElementModule, NFElementModule)
    UNREGISTER_MODULE(pPluginManager, NFIClassModule, NFClassModule)
	UNREGISTER_MODULE(pPluginManager, NFIResModule, NFResModule)
}
