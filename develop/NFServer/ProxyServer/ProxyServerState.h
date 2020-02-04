///--------------------------------------------------------------------
/// 文件名:		ProxyServerState.h
/// 内  容:		代理服状态机
/// 说  明:		
/// 创建日期:	2019年9月29日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PROXY_SERVER_STATE_H
#define _PROXY_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CProxyServer;
class CProxyServerLogicModule;
class CProxyServerState : public NFIModule
{
public:
	enum PROXY_STATE_ENUM
	{
		PROXYSTATE_UNKNOWN,	// 初始状态
		PROXYSTATE_WAITINGOPEND,// 等待开启状态
		PROXYSTATE_PAUSED,		// 暂停状态
		PROXYSTATE_OPENED,		// 服务开启状态
		PROXYSTATE_CLOSING,	// 服务准备关闭
		PROXYSTATE_CLOSED,		// 服务关闭状态
	};

	enum PROXY_INPUT_ENUM
	{
		PROXYINPUT_TIMER,		// 定时器刷新
		PROXYINPUT_WAITINGOPEN,// 等待开启
		PROXYINPUT_PAUSED,		// 暂停进入
		PROXYINPUT_OPEN,		// 开启服务器
		PROXYINPUT_CLOSE,		// 关闭服务器
	};

	enum PROXY_CAN_OPEN
	{
		CANT_OPEN = 0,
		CAN_OPEN = 1,
	};
public:
	CProxyServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetProxyState() { return m_nProxyState; }
	int ProxyStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int ProxyStateUnknown(int input, const char* msg, const uint32_t nLen);
	int ProxyStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int ProxyStatePaused(int input, const char* msg, const uint32_t nLen);
	int ProxyStateOpened(int input, const char* msg, const uint32_t nLen);
	int ProxyStateClosed(int input, const char* msg, const uint32_t nLen);
	int ProxyStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nProxyState = PROXYSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	CProxyServerLogicModule* m_pProxyServerLogicModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CProxyServer* m_pProxyServer = nullptr;
};

#endif
