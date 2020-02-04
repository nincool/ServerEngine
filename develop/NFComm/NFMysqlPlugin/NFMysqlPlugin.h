///--------------------------------------------------------------------
/// 文件名:		NFMysqlPlugin.h
/// 内  容:		mysql数据库操作插件
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

//////////////////////////////////////////////////////////////////////////
class NFMysqlPlugin : public NFIPlugin
{
public:
    NFMysqlPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};