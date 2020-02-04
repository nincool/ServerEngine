///--------------------------------------------------------------------
/// �ļ���:		GameServerState.h
/// ��  ��:		��Ϸ״̬��
/// ˵  ��:		
/// ��������:	2019��9��28��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
		GAMESTATE_UNKNOWN,	// ��ʼ״̬
		GAMESTATE_WAITINGOPEND,// �ȴ�����״̬
		GAMESTATE_LOADING,		// �������״̬
		GAMESTATE_OPENED,		// ������״̬
		GAMESTATE_CLOSING,	// ����׼���ر�
		GAMESTATE_CLOSED,		// ����ر�״̬
	};

	enum GAME_INPUT_ENUM
	{
		GAMEINPUT_TIMER,		// ��ʱ��ˢ��
		GAMEINPUT_WAITINGOPEN,// �ȴ�����
		GAMEINPUT_LOADING,		// �������״̬
		GAMEINPUT_PUBLOAD,		// pubͬ�����
		GAMEINPUT_GUILDLOAD,	// guildͬ�����
		GAMEINPUT_GUIDNAMELOAD, // �������GUID��Nameӳ��ͬ�����
		GAMEINPUT_OPEN,		// ����������
		GAMEINPUT_CLOSE,		// �رշ�����
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
