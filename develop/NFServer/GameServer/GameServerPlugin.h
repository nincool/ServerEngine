///--------------------------------------------------------------------
/// �ļ���:		GameServerPlugin.h
/// ��  ��:		GameServerPlugin
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_GAMELOGICLUGIN_H
#define NF_GAMELOGICLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class GameServerPlugin : public NFIPlugin
{
public:
	GameServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif
