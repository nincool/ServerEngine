///--------------------------------------------------------------------
/// �ļ���:		NFWordsFilterPlugin.cpp
/// ��  ��:		���д����β��
/// ˵  ��:		
/// ��������:	2019��9��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFWordsFilterPlugin.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "WordsFilterModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFWordsFilterPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFWordsFilterPlugin)
};

#endif

const int NFWordsFilterPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFWordsFilterPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFWordsFilterPlugin);
}

void NFWordsFilterPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, IWordsFilterModule, WordsFilterModule)
}

void NFWordsFilterPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, IWordsFilterModule, WordsFilterModule)
}
