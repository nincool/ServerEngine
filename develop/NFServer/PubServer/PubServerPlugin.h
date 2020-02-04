///--------------------------------------------------------------------
/// �ļ���:		PubServerPlugin.h
/// ��  ��:		�����߼����
/// ˵  ��:		
/// ��������:	2019��8��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_PUBLOGIC_PLUGIN_H
#define NF_PUBLOGIC_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class PubServerPlugin : public NFIPlugin
{
public:
	PubServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
