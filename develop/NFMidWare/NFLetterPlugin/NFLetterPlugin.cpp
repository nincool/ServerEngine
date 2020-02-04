///---------------------------------------------------------
/// 文件名: NFLetterPlugin.cpp
/// 内 容:  
/// 说 明:       
/// 创建日期: 2019/08/26
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
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