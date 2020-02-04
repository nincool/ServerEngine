///--------------------------------------------------------------------
/// �ļ���:		PlatformServerPlugin.h
/// ��  ��:		ƽ̨���߼����
/// ˵  ��:		
/// ��������:	2019��10��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_PLATFORMSERVERLOGIC_PLUGIN_H
#define NF_PLATFORMSERVERLOGIC_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class PlatformServerPlugin : public NFIPlugin
{
public:
	PlatformServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
