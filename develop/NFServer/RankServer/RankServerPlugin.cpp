///--------------------------------------------------------------------
/// 文件名:		RankServerPlugin.h
/// 内  容:		排行榜逻辑插件
/// 说  明:		
/// 创建日期:	2019年9月16日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
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

