///--------------------------------------------------------------------
/// 文件名:		CommandModule.h
/// 内  容:		命令控制
/// 说  明:		
/// 创建日期:	2019年12月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_CommandModule_H__
#define __H_CommandModule_H__
#include "NFIModule.h"
#include "NetClient.h"
#include <map>
#include <functional>

class CommandModule : public NFIModule
{
public:
	CommandModule();
	~CommandModule();
	bool Init();
	bool Connect(const char* strIP, const unsigned short nPort);
	bool Execute();
	bool Run(std::string& command);
private:
	void OnSocketEvent(const __int64 nSockIndex, const NET_EVENT eEvent);
	void OnMsg(const __int64 nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	void RunConnect(std::string& command);
	void RunNetCommand(std::string& command);
	void RunSendMsg(std::string& command);
private:
	NetCallback mNetCallback;
	NetClient* m_pNetClient = nullptr;
	bool mConnectIng = false;

	std::map<std::string, std::function<void(std::string&)>> mConnectFunMap;
};


#endif //__H_CommandModule_H__