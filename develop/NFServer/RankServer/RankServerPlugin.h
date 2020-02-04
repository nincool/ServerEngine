///--------------------------------------------------------------------
/// �ļ���:		RankServerPlugin.h
/// ��  ��:		���а��߼����
/// ˵  ��:		
/// ��������:	2019��9��16��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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
