///--------------------------------------------------------------------
/// 文件名:		PlatformServerPlugin.h
/// 内  容:		平台服逻辑插件
/// 说  明:		
/// 创建日期:	2019年10月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_PLATFORMSERVERLOGIC_PLUGIN_H
#define NF_PLATFORMSERVERLOGIC_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class PlatformServerPlugin : public NFIPlugin
{
public:
	PlatformServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
