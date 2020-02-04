///--------------------------------------------------------------------
/// �ļ���:		RankDBServerPlugin.cpp
/// ��  ��:		���а����ݿ���
/// ˵  ��:		
/// ��������:	2019��12��10��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "RankDBServerPlugin.h"
#include "RankRedis.h"
#include "RankDBServer.h"
#include "NFServer/DBSever/CommonRedis.h"
#include "NFServer/DBSever/ColdData.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, RankDBServerPlugin)
};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, RankDBServerPlugin)
};

#endif

const int RankDBServerPlugin::GetPluginVersion()
{
    return 0;
}

const std::string RankDBServerPlugin::GetPluginName()
{
	return GET_CLASS_NAME(DBServerPlugin);
}

void RankDBServerPlugin::Install()
{
	REGISTER_MODULE(pPluginManager, ICommonRedis, CCommonRedis);
	REGISTER_MODULE(pPluginManager, RankRedis, RankRedis);
	REGISTER_MODULE(pPluginManager, CRankDBServer, CRankDBServer);
	REGISTER_MODULE(pPluginManager, CColdData, CColdData);
}

void RankDBServerPlugin::Uninstall()
{
	UNREGISTER_MODULE(pPluginManager, ICommonRedis, CCommonRedis);
	UNREGISTER_MODULE(pPluginManager, RankRedis, RankRedis);
	UNREGISTER_MODULE(pPluginManager, CRankDBServer, CRankDBServer);
	UNREGISTER_MODULE(pPluginManager, CColdData, CColdData);
}
