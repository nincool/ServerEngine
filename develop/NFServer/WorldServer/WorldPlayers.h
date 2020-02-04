///--------------------------------------------------------------------
/// �ļ���:		WorldPlayers.h
/// ��  ��:		����������������
/// ˵  ��:		
/// ��������:	2019��10��12��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

// ��ǰ�����״̬������ö���кܶ�û����,ֻʵ�ּ����״̬�����Ժ������Ҫ���޸�����
// ����world�������н���֮��ͨ�ŵ���ת�����Բ���ͳ�����й��ܣ����½���Ƕ�һЩ����ֻ��������ǣ����ܸ�Ԥ
// ���Զ������״̬�������Ǹ����������̷�����������Ϣ����������Ӧ��״̬��ǣ�����״̬����Ҳ���������������־
// ��Ȼ��������ѯ���״̬���������̵���Ȼ�������õģ�����ط����̡�

#ifndef __WORLD_PLAYERS_H__
#define __WORLD_PLAYERS_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include <list>
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "WorldServer.h"

class NetServerInsideModule;

// ���״̬
enum PLAYER_STATE_ENUM
{
	PLAYERSTATE_UNKNOWN,	// ��ʼ״̬
	//PLAYERSTATE_LOGINING,	// ���ڵ�½
	//PLAYERSTATE_QUEUING,	// �����Ŷ�
	//PLAYERSTATE_GETTING,	// ���ڻ�ý�ɫ��Ϣ
	//PLAYERSTATE_LOGINED,	// ��½�ɹ�
	//PLAYERSTATE_CHOOSING,	// ѡ�񲢼��ؽ�ɫ����
	//PLAYERSTATE_CREATING,	// ������ɫ
	//PLAYERSTATE_CREATE_CHECKING,	// ������ɫ��֤
	//PLAYERSTATE_DELETING,	// ɾ����ɫ
	//PLAYERSTATE_REVIVING,	// �ָ���ɫ
	PLAYERSTATE_ONLINE,		// �ѽ�����Ϸ
	PLAYERSTATE_OFFLINE,	// �Ѷ���
	PLAYERSTATE_SAVING,		// ���ڱ����ɫ����
	//PLAYERSTATE_LEAVING,	// �����뿪��Ϸ
};

enum PLAYER_INPUT_ENUM
{
	PLAYERINPUT_TIMER,				// ��ʱ��ˢ��
	//PLAYERINPUT_REMOVE,				// ǿ��ɾ��
	PLAYERINPUT_USER_BREAK,			// �ͻ��˶���
	//PLAYERINPUT_CLIENT_LOGIN,		// �ͻ��˵�¼
	//PLAYERINPUT_CLIENT_CHOOSE_ROLE,	// �ͻ���ѡ���ɫ
	//PLAYERINPUT_CLIENT_CREATE_ROLE,	// �ͻ��˴�����ɫ
	//PLAYERINPUT_CLIENT_CREATE_ROLE_CHECK,// �ͻ��˴�����ɫ��֤
	//PLAYERINPUT_CLIENT_DELETE_ROLE,	// �ͻ���ɾ����ɫ
	//PLAYERINPUT_CLIENT_REVIVE_ROLE,	// �ͻ��˻ָ���ɫ
	//PLAYERINPUT_RETURN_NEW_ROLE,	// �����½�ɫ����
	//PLAYERINPUT_ACCOUNT_ROLES,		// �����ʺŵĽ�ɫ��Ϣ
	//PLAYERINPUT_STORE_LOAD,			// ���ؼ��ؽ�ɫ���ݽ��
	//PLAYERINPUT_STORE_CREATE,		// ���ش�����ɫ���
	//PLAYERINPUT_STORE_DELETE,		// ����ɾ����ɫ���
	//PLAYERINPUT_STORE_REVIVE,		// ���ػָ���ɫ���
	PLAYERINPUT_STORE_SAVED,		// ����������ݳɹ�
	//PLAYERINPUT_END_QUEUE,			// �����Ŷ�
	PLAYERINPUT_SERVER_CLOSE,		// �������ر�
	PLAYERINPUT_LEAVE_GAME,			// �����뿪��Ϸ
	//PLAYERINPUT_LOGIN_WITH_ROLE,	// ��½��ѡ���ɫ
};

// �������
struct PlayerData
{	
	NFGUID self;		// ��ɫID
	NFGUID client;		// �ͻ���ID
	int nGameID = 0;	// ��Ϸ��ID
	int nGateID = 0;	// �����ID
	int nState = PLAYERSTATE_UNKNOWN; // ״̬
	int nSavingSec = 0; // ����ȴ�����
	int nRemoveSec = 0; // ɾ���ȴ���
};

class CWorldPlayers : public NFIModule
{
public:
	CWorldPlayers(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool AfterInit();
	virtual bool Execute();

	// ��ȡ�������
	const PlayerData* GetPlayer(const NFGUID& self);
	// ������
	void ClearPlayers();
	// �������Ƿ����
	bool IsFinishClearPlayer();

	//��ǰ��������
	size_t GetPlayerCount();

private:
	PlayerData& AddPlayer(const NFGUID& self, const NFGUID& client, int nGameID, int nGateID);
	bool RemovePlayer(const NFGUID& self);

	// �Ͽ�����
	void BreakClient(const PlayerData& player);

private:

	//������ҽ�ɫ������Ϣ
	void OnOnlineProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//��ұ����������
	void OnPlayerSaved(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �ͻ��˶���
	void OnUserBreak(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �����뿪��Ϸ
	void OnLeaveGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ɫ�뿪��Ϸ
	void CommandPlayerLeaveGame(const PlayerData& player);
	// ͬ������״̬
	void CommandPlayerOffline(const PlayerData& player);

private: // ״̬��
	int PlayerStateMachine(const NFGUID& self, int nInput, const char* msg, const uint32_t nLen);
	bool SetPlayerState(PlayerData& player, int nState);

	int PlayerStateAny(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateUnknown(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateLogining(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateQueuing(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateGetting(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateLogined(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateChooseing(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateCreating(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateCreateChecking(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateDeleting(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateReviving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	int PlayerStateOnline(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	int PlayerStateOffline(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	int PlayerStateSaving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);
	//int PlayerStateLeaving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen);

private:
	NFILogModule* m_pLogModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CWorldServer* m_pWorldServer = nullptr;

	//��Ҷ�������
	std::map<NFGUID, PlayerData> m_mapPlayers;
	//������ɾ��������
	std::list<NFGUID> m_listRemovePlayers;

	// �ϴ�ʱ���
	int64_t m_dnLastTime = 0;
};

#endif //__WORLD_PLAYERS_H__