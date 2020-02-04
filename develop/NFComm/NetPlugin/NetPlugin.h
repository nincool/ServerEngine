///--------------------------------------------------------------------
/// 文件名:		NetPlugin.h
/// 内  容:		网络库插件
/// 说  明:		
/// 创建日期:	2019年11月12日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NetPlugin : public NFIPlugin
{
public:
	NetPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
