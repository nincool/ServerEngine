///--------------------------------------------------------------------
/// �ļ���:		RankServerPlugin.h
/// ��  ��:		���а��߼����
/// ˵  ��:		
/// ��������:	2019��9��16��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "RankServerPlugin.h"
#include "RankModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, RankServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, RankServerPlugin)
};

#endif
//////////////////////////////////////////////////////////////////////////

const int RankServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string RankServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(RankServerPlugin);
}

void RankServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, RankModule, RankModule);
}

void RankServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, RankModule, RankModule);
}

