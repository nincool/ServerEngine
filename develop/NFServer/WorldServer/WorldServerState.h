///--------------------------------------------------------------------
/// 文件名:		WorldServerState.h
/// 内  容:		世界服状态机
/// 说  明:		
/// 创建日期:	2019年9月29日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _WORLD_SERVER_STATE_H
#define _WORLD_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "WorldPlayers.h"
#include "WorldServer.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CWorldServerState : public NFIModule
{
public:
	enum WORLD_STATE_ENUM
	{
		WORLDSTATE_UNKNOWN,	// 初始状态
		WORLDSTATE_WAITINGALLNET,// 等待所有网络完成
		WORLDSTATE_WAITINGOPEN,	// 等待开启
		WORLDSTATE_OPENED,		// 服务开启状态
		WORLDSTATE_CLOSEPROXY,	// 关闭Proxy
		WORLDSTATE_CLOSEGAME,	// 关闭Game
		WORLDSTATE_CLOSEPUB,	// 关闭Pub
		WORLDSTATE_CLOSEDB,		// 关闭DB
		WORLDSTATE_CLOSED,		// 服务关闭状态
		WORLDSTATE_CLEAR_PLAYER, // 清空玩家
	};

	enum WORLD_INPUT_ENUM
	{
		WORLDINPUT_TIMER,		// 定时器刷新
		WORLDINPUT_WAITINGALLNET, // 等待网络
		WORLDINPUT_DBNET,		// DB网络完成
		WORLDINPUT_GAMENET,		// GAME网络完成
		WORLDINPUT_PUBNET,		// PUB网络完成
		WORLDINPUT_PROXYNET,	// PROXY网络完成
		WORLDINPUT_WAITINGOPEN,// 等待开启
		WORLDINPUT_DBOPEN,		// DB open
		WORLDINPUT_GAMEOPEN,	// GAME open
		WORLDINPUT_PUBOPEN,		// PUB open
		WORLDINPUT_PROXYOPEN,	// PROXY open
		WORLDINPUT_OPEN,		// 开启服务器
		WORLDINPUT_CLOSEPROXY,  // 关闭Proxy
		WORLDINPUT_CLOSEGAME,	// 关闭Game
		WORLDINPUT_CLOSEPUB,	// 关闭Pub
		WORLDINPUT_CLOSEDB,		// 关闭DB
		WORLDINPUT_CLOSE,		// 关闭服务器
	};

	vector<int> vServerList = { NF_ST_GAME, NF_ST_PUB, NF_ST_DB, NF_ST_PROXY };
public:
	CWorldServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetWorldState() { return m_nWorldState; }
	int WorldStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int WorldStateUnknown(int input, const char* msg, const uint32_t nLen);
	int WorldStateWaitingAllNet(int input, const char* msg, const uint32_t nLen);
	int WorldStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int WorldStateOpened(int input, const char* msg, const uint32_t nLen);
	int WorldStateClosed(int input, const char* msg, const uint32_t nLen);
	int WorldStateClearPlayer(int input, const char* msg, const uint32_t nLen);
	int WorldStateCloseProxy(int input, const char* msg, const uint32_t nLen);
	int WorldStateCloseGame(int input, const char* msg, const uint32_t nLen);
	int WorldStateClosePub(int input, const char* msg, const uint32_t nLen);
	int WorldStateCloseDB(int input, const char* msg, const uint32_t nLen);
	int CheckForAllNetComplete();
	int CheckForAllServerOpen();
private:
	NFINT64 m_nLastTime = 0;
	int m_nWorldState = WORLDSTATE_UNKNOWN;

	bool bDBNetComplete = false;
	bool bGameNetComplete = false;
	bool bPubNetComplete = false;
	bool bProxyNetComplete = false;

	bool bDBServerOpen = false;
	bool bGameServerOpen = false;
	bool bPubServerOpen = false;
	bool bProxyServerOpen = false;

	NetClientModule* m_pClientNet = nullptr;
	CWorldPlayers* m_pWorldPlayers = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CWorldServer* m_pWorldServer = nullptr;
};

#endif
