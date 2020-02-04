///--------------------------------------------------------------------
/// �ļ���:		NFMysqlPlugin.h
/// ��  ��:		mysql���ݿ�������
/// ˵  ��:		
/// ��������:	2019��9��24��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

//////////////////////////////////////////////////////////////////////////
class NFMysqlPlugin : public NFIPlugin
{
public:
    NFMysqlPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};