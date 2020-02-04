///--------------------------------------------------------------------
/// �ļ���:		ChatServerPlugin.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��8��30��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#ifdef NFLOGINNET_SERVERPLUGIN_EXPORTS
#define NFLOGINNET_SERVERPLUGIN_API __declspec(dllexport)
#else
#define NFLOGINNET_SERVERPLUGIN_API __declspec(dllimport)
#endif

class ChatServerPlugin : public NFIPlugin
{
public:
	ChatServerPlugin(NFIPluginManager* p);

	virtual const int GetPluginVersion();

	virtual const std::string GetPluginName();

	virtual void Install();

	virtual void Uninstall();
};