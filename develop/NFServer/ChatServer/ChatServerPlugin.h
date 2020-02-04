///--------------------------------------------------------------------
/// 文件名:		ChatServerPlugin.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年8月30日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
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