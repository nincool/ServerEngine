///--------------------------------------------------------------------
/// �ļ���:		WorldServerState.h
/// ��  ��:		�����״̬��
/// ˵  ��:		
/// ��������:	2019��9��29��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
		WORLDSTATE_UNKNOWN,	// ��ʼ״̬
		WORLDSTATE_WAITINGALLNET,// �ȴ������������
		WORLDSTATE_WAITINGOPEN,	// �ȴ�����
		WORLDSTATE_OPENED,		// ������״̬
		WORLDSTATE_CLOSEPROXY,	// �ر�Proxy
		WORLDSTATE_CLOSEGAME,	// �ر�Game
		WORLDSTATE_CLOSEPUB,	// �ر�Pub
		WORLDSTATE_CLOSEDB,		// �ر�DB
		WORLDSTATE_CLOSED,		// ����ر�״̬
		WORLDSTATE_CLEAR_PLAYER, // ������
	};

	enum WORLD_INPUT_ENUM
	{
		WORLDINPUT_TIMER,		// ��ʱ��ˢ��
		WORLDINPUT_WAITINGALLNET, // �ȴ�����
		WORLDINPUT_DBNET,		// DB�������
		WORLDINPUT_GAMENET,		// GAME�������
		WORLDINPUT_PUBNET,		// PUB�������
		WORLDINPUT_PROXYNET,	// PROXY�������
		WORLDINPUT_WAITINGOPEN,// �ȴ�����
		WORLDINPUT_DBOPEN,		// DB open
		WORLDINPUT_GAMEOPEN,	// GAME open
		WORLDINPUT_PUBOPEN,		// PUB open
		WORLDINPUT_PROXYOPEN,	// PROXY open
		WORLDINPUT_OPEN,		// ����������
		WORLDINPUT_CLOSEPROXY,  // �ر�Proxy
		WORLDINPUT_CLOSEGAME,	// �ر�Game
		WORLDINPUT_CLOSEPUB,	// �ر�Pub
		WORLDINPUT_CLOSEDB,		// �ر�DB
		WORLDINPUT_CLOSE,		// �رշ�����
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
