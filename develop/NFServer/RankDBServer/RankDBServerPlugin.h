///--------------------------------------------------------------------
/// 文件名:		RankDBServerPlugin.h
/// 内  容:		排行榜数据库插件
/// 说  明:		
/// 创建日期:	2019年12月10日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __RANK_DB_SERVER_PLUGIN_H__
#define __RANK_DB_SERVER_PLUGIN_H__

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class RankDBServerPlugin : public NFIPlugin
{
public:
	RankDBServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif // __RANK_DB_SERVER_PLUGIN_H__