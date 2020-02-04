///--------------------------------------------------------------------
/// �ļ���:		NFMysqlPlugin.cpp
/// ��  ��:		mysql���ݿ�������
/// ˵  ��:		
/// ��������:	2019��9��24��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFMysqlPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFMysqlModule.h"

#ifdef NF_DYNAMIC_PLUGIN

NF_EXPORT void DllStartPlugin(NFIPluginManager* pm)
{
    CREATE_PLUGIN(pm, NFMysqlPlugin)

};

NF_EXPORT void DllStopPlugin(NFIPluginManager* pm)
{
    DESTROY_PLUGIN(pm, NFMysqlPlugin)
};

#endif


//////////////////////////////////////////////////////////////////////////

const int NFMysqlPlugin::GetPluginVersion()
{
    return 0;
}

const std::string NFMysqlPlugin::GetPluginName()
{
	return GET_CLASS_NAME(NFMysqlPlugin);
}

void NFMysqlPlugin::Install()
{
    REGISTER_MODULE(pPluginManager, NFMysqlModule, NFMysqlModule)
}

void NFMysqlPlugin::Uninstall()
{

    UNREGISTER_MODULE(pPluginManager, NFMysqlModule, NFMysqlModule)
}