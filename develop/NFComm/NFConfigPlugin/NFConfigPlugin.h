///--------------------------------------------------------------------
/// �ļ���:		NFConfigPlugin.h
/// ��  ��:		���ò��
/// ˵  ��:		
/// ��������:	2019��11��12��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFConfigPlugin : public NFIPlugin
{
public:
    NFConfigPlugin(NFIPluginManager* p);

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
