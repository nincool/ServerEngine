///--------------------------------------------------------------------
/// �ļ���:		LoginServerPlugin.h
/// ��  ��:		LoginServerPlugin
/// ˵  ��:		
/// ��������:	2019��9��6��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef NF_LOGINLOGICPLUGIN_MODULE_H
#define NF_LOGINLOGICPLUGIN_MODULE_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFLOGINLOGICPLUGIN_EXPORTS
#define NFLOGINLOGICPLUGIN_API __declspec(dllexport)
#else
#define NFLOGINLOGICPLUGIN_API __declspec(dllimport)
#endif

class LoginServerPlugin : public NFIPlugin
{
public:
	LoginServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif