///---------------------------------------------------------
/// �ļ���: NFLetterPlugin.h
/// �� ��:  
/// ˵ ��:       
/// ��������: 2019/08/26
/// ������: �ڵ���
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFLetterPlugin : public NFIPlugin
{
public:
    NFLetterPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
