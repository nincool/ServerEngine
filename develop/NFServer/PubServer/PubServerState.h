///--------------------------------------------------------------------
/// 文件名:		PubServerState.h
/// 内  容:		Pub状态机
/// 说  明:		
/// 创建日期:	2019年9月29日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PUB_SERVER_STATE_H
#define _PUB_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/IPubKernel.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPubServer;
class CPubServerState : public NFIModule
{
public:
	enum PUB_STATE_ENUM
	{
		PUBSTATE_UNKNOWN,	// 初始状态
		PUBSTATE_WAITINGOPEND,// 等待开启状态
		PUBSTATE_LOADING,		// 服务加载状态
		PUBSTATE_OPENED,		// 服务开启状态
		PUBSTATE_CLOSING,	// 服务准备关闭
		PUBSTATE_CLOSED,		// 服务关闭状态
	};

	enum PUB_INPUT_ENUM
	{
		PUBINPUT_TIMER,		// 定时器刷新
		PUBINPUT_WAITINGOPEN,// 等待开启
		PUBINPUT_LOADING,		// 服务加载状态
		PUBINPUT_PUBLOAD,		// 公共域加载完成
		PUBINPUT_GUILDLOAD,	// 公会加载完成
		PUBINPUT_OPEN,		// 开启服务器
		PUBINPUT_CLOSESAVE,		// 保存公共域信息
		PUBINPUT_CLOSE,		// 关闭服务器
	};
public:
	CPubServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetPubState() { return m_nPubState; }
	int PubStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int PubStateUnknown(int input, const char* msg, const uint32_t nLen);
	int PubStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int PubStateLoading(int input, const char* msg, const uint32_t nLen);
	int PubStateOpened(int input, const char* msg, const uint32_t nLen);
	int PubStateClosed(int input, const char* msg, const uint32_t nLen);
	int PubStateClosing(int input, const char* msg, const uint32_t nLen);

	int StateCheckForOpen();
private:
	NFINT64 m_nLastTime = 0;
	int m_nPubState = PUBSTATE_UNKNOWN;
	bool bCompletePub = false;
	bool bCompleteGuild = false;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	IPubKernel* m_pPubKernel = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CPubServer* m_pPubServer = nullptr;
};

#endif
