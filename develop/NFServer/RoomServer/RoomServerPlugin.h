///--------------------------------------------------------------------
/// �ļ���:		RoomServerPlugin.h
/// ��  ��:		��������
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_ROOMLOGIC_PLUGIN_H
#define NF_ROOMLOGIC_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class RoomServerPlugin : public NFIPlugin
{
public:
	RoomServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
