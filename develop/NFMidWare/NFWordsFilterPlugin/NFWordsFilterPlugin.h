///--------------------------------------------------------------------
/// 文件名:		NFWordsFilterPlugin.h
/// 内  容:		敏感词屏蔽插件
/// 说  明:		
/// 创建日期:	2019年9月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFWordsFilterPlugin : public NFIPlugin
{
public:
	NFWordsFilterPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
