///--------------------------------------------------------------------
/// 文件名:		PlatformServerState.h
/// 内  容:		平台服状态机
/// 说  明:		
/// 创建日期:	2019年9月29日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PLATFORM_SERVER_STATE_H
#define _PLATFORM_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/IPlatformKernel.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPlatformServer;
class CPlatformServerState : public NFIModule
{
public:
	enum PLATFORM_STATE_ENUM
	{
		PLATFORMSTATE_UNKNOWN,	// 初始状态
		PLATFORMSTATE_WAITINGALLNET,	// 等待ROOM连接
		PLATFORMSTATE_LOADINGDB,		// 服务加载db
		PLATFORMSTATE_WAITINGOPEN,		// 服务等待开启
		PLATFORMSTATE_OPENED,		// 服务开启状态
		PLATFORMSTATE_CLOSING,	// 服务准备关闭
		PLATFORMSTATE_CLOSED,		// 服务关闭状态
	};

	enum PLATFORM_INPUT_ENUM
	{
		PLATFORMINPUT_TIMER,		// 定时器刷新
		PLATFORMINPUT_WAITINGALLNET,	// 等待ROOM连接状态
		PLATFORMINPUT_ROOMNET,		// ROOM网络完成
		PLATFORMINPUT_LOADINGDB,		// 服务加载db状态
		PLATFORMINPUT_WAITINGOPEN,		// 服务等待开启状态
		PLATFORMINPUT_ROOMOPEN,		// ROOM开启状态
		PLATFORMINPUT_CLOSE,		// 关闭服务器
	};
public:
	CPlatformServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetPlatformState() { return m_nPlatformState; }
	int PlatformStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int PlatformStateUnknown(int input, const char* msg, const uint32_t nLen);
	int PlatformStateWaitingAllNet(int input, const char* msg, const uint32_t nLen);
	int PlatformStateLoadingDB(int input, const char* msg, const uint32_t nLen);
	int PlatformStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int PlatformStateOpened(int input, const char* msg, const uint32_t nLen);
	int PlatformStateClosed(int input, const char* msg, const uint32_t nLen);
	int PlatformStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nPlatformState = PLATFORMSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	CPlatformServer* m_pPlatformServer = nullptr;
	IPlatformKernel* m_pPlatformKernel = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
