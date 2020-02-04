///--------------------------------------------------------------------
/// �ļ���:		NetPlugin.h
/// ��  ��:		�������
/// ˵  ��:		
/// ��������:	2019��11��12��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NetPlugin : public NFIPlugin
{
public:
	NetPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
