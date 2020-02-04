///--------------------------------------------------------------------
/// �ļ���:		RankDBServerPlugin.h
/// ��  ��:		���а����ݿ���
/// ˵  ��:		
/// ��������:	2019��12��10��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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