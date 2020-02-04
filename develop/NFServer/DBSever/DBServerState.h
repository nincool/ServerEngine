///--------------------------------------------------------------------
/// 文件名:		DBServerState.h
/// 内  容:		数据库状态机
/// 说  明:		
/// 创建日期:	2019年9月26日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _DB_SERVER_STATE_H
#define _DB_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CDBServer;
class CDBServerState : public NFIModule
{
public:
	enum DB_STATE_ENUM
	{
		DBSTATE_UNKNOWN,	// 初始状态
		DBSTATE_WAITINGOPEND,// 等待开启状态
		DBSTATE_OPENED,		// 服务开启状态
		DBSTATE_CLOSING,	// 服务准备关闭
		DBSTATE_CLOSED,		// 服务关闭状态
	};

	enum DB_INPUT_ENUM
	{
		DBINPUT_TIMER,		// 定时器刷新
		DBINPUT_WAITINGOPEN,// 等待开启
		DBINPUT_OPEN,		// 开启服务器
		DBINPUT_CLOSE,		// 关闭服务器
	};
public:
	CDBServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	const int GetStateEnum() const;
	int DBStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int DBStateUnknown(int input, const char* msg, const uint32_t nLen);
	int DBStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int DBStateOpened(int input, const char* msg, const uint32_t nLen);
	int DBStateClosed(int input, const char* msg, const uint32_t nLen);
	int DBStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nDBState = DBSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CDBServer* m_pDBServer = nullptr;
};

#endif
