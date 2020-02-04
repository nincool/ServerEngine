///---------------------------------------------------------
/// �ļ���: NFLetterPlugin.cpp
/// �� ��:  
/// ˵ ��:       
/// ��������: 2019/08/26
/// ������: �ڵ���
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------

#include "NFLetterPlugin.h"
#include "NFLetterModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFLetterPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFLetterPlugin)
};

#endif

const int NFLetterPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFLetterPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFLetterPlugin);
}

void NFLetterPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, NFILetterModule, NFLetterModule);
}

void NFLetterPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, NFILetterModule, NFLetterModule);
}