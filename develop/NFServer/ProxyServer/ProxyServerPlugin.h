///--------------------------------------------------------------------
/// �ļ���:		ProxyServerPlugin.h
/// ��  ��:		������������
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef NF_PROXYSERVERPLUGIN_H
#define NF_PROXYSERVERPLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class ProxyServerPlugin : public NFIPlugin
{
public:
	ProxyServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

#endif