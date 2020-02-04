///--------------------------------------------------------------------
/// 文件名:		LoginServerPlugin.h
/// 内  容:		LoginServerPlugin
/// 说  明:		
/// 创建日期:	2019年9月6日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef NF_LOGINLOGICPLUGIN_MODULE_H
#define NF_LOGINLOGICPLUGIN_MODULE_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFLOGINLOGICPLUGIN_EXPORTS
#define NFLOGINLOGICPLUGIN_API __declspec(dllexport)
#else
#define NFLOGINLOGICPLUGIN_API __declspec(dllimport)
#endif

class LoginServerPlugin : public NFIPlugin
{
public:
	LoginServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif