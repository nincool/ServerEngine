///---------------------------------------------------------
/// 文件名: NFLetterPlugin.h
/// 内 容:  
/// 说 明:       
/// 创建日期: 2019/08/26
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFLetterPlugin : public NFIPlugin
{
public:
    NFLetterPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
