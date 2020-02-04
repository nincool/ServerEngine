///--------------------------------------------------------------------
/// 文件名:		MasterServerPlugin.h
/// 内  容:		Master作为服务器插件
/// 说  明:		
/// 创建日期:	2019年9月6日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFMASTERNET_SERVERPLUGIN_EXPORTS
#define NFMASTERNET_SERVERPLUGIN_API __declspec(dllexport)
#else
#define NFMASTERNET_SERVERPLUGIN_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
class MasterServerPlugin : public NFIPlugin
{
public:
	MasterServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

