///--------------------------------------------------------------------
/// 文件名:		NFSecurityPlugin.h
/// 内  容:		加密插件
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_SECURITY_PLUGIN_H
#define NF_SECURITY_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFSecurityPlugin : public NFIPlugin
{
public:
	NFSecurityPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif