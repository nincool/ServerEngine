///--------------------------------------------------------------------
/// 文件名:		ProxyServerPlugin.h
/// 内  容:		代理服务器插件
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef NF_PROXYSERVERPLUGIN_H
#define NF_PROXYSERVERPLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class ProxyServerPlugin : public NFIPlugin
{
public:
	ProxyServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif