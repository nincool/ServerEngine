///--------------------------------------------------------------------
/// �ļ���:		NFSecurityPlugin.h
/// ��  ��:		���ܲ��
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_SECURITY_PLUGIN_H
#define NF_SECURITY_PLUGIN_H

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFSecurityPlugin : public NFIPlugin
{
public:
	NFSecurityPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
#endif