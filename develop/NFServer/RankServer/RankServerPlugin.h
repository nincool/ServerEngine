///--------------------------------------------------------------------
/// 文件名:		RankServerPlugin.h
/// 内  容:		排行榜逻辑插件
/// 说  明:		
/// 创建日期:	2019年9月16日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NF_RANK_LOGIC_PLUGIN_H__
#define __NF_RANK_LOGIC_PLUGIN_H__

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class RankServerPlugin : public NFIPlugin
{
public:
	RankServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif // __NF_RANK_LOGIC_PLUGIN_H__
