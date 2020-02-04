///--------------------------------------------------------------------
/// 文件名:		NFLogPlugin.h
/// 内  容:		日志插件
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFLogPlugin : public NFIPlugin
{
public:
    NFLogPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
