///--------------------------------------------------------------------
/// 文件名:		NFConfigPlugin.h
/// 内  容:		配置插件
/// 说  明:		
/// 创建日期:	2019年11月12日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFConfigPlugin : public NFIPlugin
{
public:
    NFConfigPlugin(NFIPluginManager* p);

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
