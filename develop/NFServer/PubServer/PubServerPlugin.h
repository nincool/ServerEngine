///--------------------------------------------------------------------
/// 文件名:		PubServerPlugin.h
/// 内  容:		公共逻辑插件
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_PUBLOGIC_PLUGIN_H
#define NF_PUBLOGIC_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class PubServerPlugin : public NFIPlugin
{
public:
	PubServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
