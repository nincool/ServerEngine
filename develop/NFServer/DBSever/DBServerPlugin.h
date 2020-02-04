///--------------------------------------------------------------------
/// �ļ���:		DBServerPlugin.h
/// ��  ��:		���ݿ���
/// ˵  ��:		
/// ��������:	2019��8��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class DBServerPlugin : public NFIPlugin
{
public:
	DBServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};