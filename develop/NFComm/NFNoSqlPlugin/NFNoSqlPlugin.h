///--------------------------------------------------------------------
/// 文件名:		NFNoSqlPlugin.h
/// 内  容:		数据库插件
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFDATABASEPLUGIN_EXPORTS
#define NFDATABASEPLUGIN_API __declspec(dllexport)
#else
#define NFDATABASEPLUGIN_API __declspec(dllimport)
#endif

class NFNoSqlPlugin : public NFIPlugin
{
public:
    NFNoSqlPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};