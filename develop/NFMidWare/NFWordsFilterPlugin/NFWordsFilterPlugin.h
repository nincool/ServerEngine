///--------------------------------------------------------------------
/// �ļ���:		NFWordsFilterPlugin.h
/// ��  ��:		���д����β��
/// ˵  ��:		
/// ��������:	2019��9��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFWordsFilterPlugin : public NFIPlugin
{
public:
	NFWordsFilterPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};
