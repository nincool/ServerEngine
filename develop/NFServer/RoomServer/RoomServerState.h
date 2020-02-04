///--------------------------------------------------------------------
/// 文件名:		RoomServerState.h
/// 内  容:		房间服状态机
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _ROOM_SERVER_STATE_H
#define _ROOM_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CRoomServer;
class CRoomServerState : public NFIModule
{
public:
	enum ROOM_STATE_ENUM
	{
		ROOMSTATE_UNKNOWN,	// 初始状态
		ROOMSTATE_WAITINGOPEND,// 等待开启状态
		ROOMSTATE_OPENED,		// 服务开启状态
		ROOMSTATE_CLOSING,	// 服务准备关闭
		ROOMSTATE_CLOSED,		// 服务关闭状态
	};

	enum ROOM_INPUT_ENUM
	{
		ROOMINPUT_TIMER,		// 定时器刷新
		ROOMINPUT_WAITINGOPEN,// 等待开启
		ROOMINPUT_OPEN,		// 开启服务器
		ROOMINPUT_CLOSE,		// 关闭服务器
	};
public:
	CRoomServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetRoomState() { return m_nRoomState; }
	int RoomStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int RoomStateUnknown(int input, const char* msg, const uint32_t nLen);
	int RoomStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int RoomStateOpened(int input, const char* msg, const uint32_t nLen);
	int RoomStateClosed(int input, const char* msg, const uint32_t nLen);
	int RoomStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nRoomState = ROOMSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CRoomServer* m_pRoomServer = nullptr;
};

#endif
