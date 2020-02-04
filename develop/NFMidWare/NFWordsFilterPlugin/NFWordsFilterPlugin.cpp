///--------------------------------------------------------------------
/// 文件名:		NFWordsFilterPlugin.cpp
/// 内  容:		敏感词屏蔽插件
/// 说  明:		
/// 创建日期:	2019年9月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
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
