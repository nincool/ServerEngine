///--------------------------------------------------------------------
/// 文件名:		WorldServerPlugin.h
/// 内  容:		世界服务器插件
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef NF_WORLDLOGICPLUGIN_MODULE_H
#define NF_WORLDLOGICPLUGIN_MODULE_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFWORLDLOGICPLUGIN_EXPORTS
#define NFWORLDLOGICPLUGIN_API __declspec(dllexport)
#else
#define NFWORLDLOGICPLUGIN_API __declspec(dllimport)
#endif

class WorldServerPlugin : public NFIPlugin
{
public:
	WorldServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif