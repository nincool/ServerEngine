///--------------------------------------------------------------------
/// �ļ���:		MasterServerPlugin.h
/// ��  ��:		Master��Ϊ���������
/// ˵  ��:		
/// ��������:	2019��9��6��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFMASTERNET_SERVERPLUGIN_EXPORTS
#define NFMASTERNET_SERVERPLUGIN_API __declspec(dllexport)
#else
#define NFMASTERNET_SERVERPLUGIN_API __declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////
class MasterServerPlugin : public NFIPlugin
{
public:
	MasterServerPlugin(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual const int GetPluginVersion();

    virtual const std::string GetPluginName();

    virtual void Install();

    virtual void Uninstall();
};

