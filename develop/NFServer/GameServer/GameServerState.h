///--------------------------------------------------------------------
/// 文件名:		GameServerState.h
/// 内  容:		游戏状态机
/// 说  明:		
/// 创建日期:	2019年9月28日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _GAME_SERVER_STATE_H
#define _GAME_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CGameServer;
class CGameServerState : public NFIModule
{
public:
	enum GAME_STATE_ENUM
	{
		GAMESTATE_UNKNOWN,	// 初始状态
		GAMESTATE_WAITINGOPEND,// 等待开启状态
		GAMESTATE_LOADING,		// 服务加载状态
		GAMESTATE_OPENED,		// 服务开启状态
		GAMESTATE_CLOSING,	// 服务准备关闭
		GAMESTATE_CLOSED,		// 服务关闭状态
	};

	enum GAME_INPUT_ENUM
	{
		GAMEINPUT_TIMER,		// 定时器刷新
		GAMEINPUT_WAITINGOPEN,// 等待开启
		GAMEINPUT_LOADING,		// 服务加载状态
		GAMEINPUT_PUBLOAD,		// pub同步完成
		GAMEINPUT_GUILDLOAD,	// guild同步完成
		GAMEINPUT_GUIDNAMELOAD, // 所有玩家GUID与Name映射同步完成
		GAMEINPUT_OPEN,		// 开启服务器
		GAMEINPUT_CLOSE,		// 关闭服务器
	};
public:
	CGameServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetGameState() { return m_nGameState; }
	int GameStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int GameStateUnknown(int input, const char* msg, const uint32_t nLen);
	int GameStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int GameStateLoading(int input, const char* msg, const uint32_t nLen);
	int GameStateOpened(int input, const char* msg, const uint32_t nLen);
	int GameStateClosed(int input, const char* msg, const uint32_t nLen);
	int GameStateClosing(int input, const char* msg, const uint32_t nLen);

	int StateCheckForOpen();
private:
	NFINT64 m_nLastTime = 0;
	int m_nGameState = GAMESTATE_UNKNOWN;
	bool bCompletePub = false;
	bool bCompleteGuild = false;
	bool bCompleteGuidName = false;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
};

#endif
