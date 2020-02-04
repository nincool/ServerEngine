///--------------------------------------------------------------------
/// �ļ���:		WorldServerPlugin.h
/// ��  ��:		������������
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef NF_WORLDLOGICPLUGIN_MODULE_H
#define NF_WORLDLOGICPLUGIN_MODULE_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFWORLDLOGICPLUGIN_EXPORTS
#define NFWORLDLOGICPLUGIN_API __declspec(dllexport)
#else
#define NFWORLDLOGICPLUGIN_API __declspec(dllimport)
#endif

class WorldServerPlugin : public NFIPlugin
{
public:
	WorldServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif