///--------------------------------------------------------------------
/// �ļ���:		WorldPlayerState.h
/// ��  ��:		������������״̬��
/// ˵  ��:		
/// ��������:	2019��10��12��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "WorldPlayers.h"

// ����ȴ������ (�Ժ�����ⲿ����)
const int SAVING_SEC_MAX = 300;

int CWorldPlayers::PlayerStateMachine(const NFGUID& self, int nInput, const char* msg, const uint32_t nLen)
{
	auto it = m_mapPlayers.find(self);
	if (it == m_mapPlayers.end())
	{
		QLOG_WARING << __FUNCTION__ << " not find player input:" << nInput << " player:" << self;
		return 0;
	}

	PlayerData& player = it->second;
	switch (player.nState)
	{
	//case PLAYERSTATE_UNKNOWN:	// ��ʼ״̬
	//	PlayerStateUnknown(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_QUEUING:	// �����Ŷ�
	//	PlayerStateQueuing(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_GETTING:	// ���ڻ�ý�ɫ��Ϣ
	//	PlayerStateGetting(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_LOGINED:	// ��½�ɹ�
	//	PlayerStateLogined(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_CHOOSING:	// ѡ�񲢼��ؽ�ɫ����
	//	PlayerStateChooseing(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_CREATING:	// ������ɫ
	//	PlayerStateCreating(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_CREATE_CHECKING:	// ������ɫ��֤
	//	PlayerStateCreateChecking(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_DELETING:	// ɾ����ɫ
	//	PlayerStateDeleting(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_DELETING:	// ɾ����ɫ
	//	PlayerStateDeleting(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_REVIVING:	// �ָ���ɫ
	//	PlayerStateReviving(player, nInput, msg, nLen);
	//	break;
	case PLAYERSTATE_ONLINE:	// �ѽ�����Ϸ
		PlayerStateOnline(player, nInput, msg, nLen);
		break;
	case PLAYERSTATE_OFFLINE:	// �Ѷ���
		PlayerStateOffline(player, nInput, msg, nLen);
		break;
	case PLAYERSTATE_SAVING:	// ���ڱ����ɫ����
		PlayerStateSaving(player, nInput, msg, nLen);
		break;
	//case PLAYERSTATE_LEAVING:	// �����뿪��Ϸ
	//	PlayerStateLeaving(player, nInput, msg, nLen);
	//	break;
	default:
		QLOG_WARING << __FUNCTION__ << " error player state:" << player.nState << " player:" << self;
		break;
	}

	return 0;
}

bool CWorldPlayers::SetPlayerState(PlayerData& player, int nState)
{
	player.nState = nState;

	return true;
}

int CWorldPlayers::PlayerStateAny(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
{
	if (nInput == PLAYERINPUT_TIMER)
	{
		// ��������
	}
	else
	{
		return 0;
	}

	return 1;
}

//int CWorldPlayers::PlayerStateUnknown(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (nInput == PLAYERINPUT_CLIENT_LOGIN)
//	{
//		SetPlayerState(player, PLAYERSTATE_LOGINING);
//	}
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateQueuing(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateGetting(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateLogined(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateChooseing(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateCreating(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateCreateChecking(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateDeleting(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateReviving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	}
//
//	return 1;
//}
//
//int CWorldPlayers::PlayerStateReviving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}

int CWorldPlayers::PlayerStateOnline(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
{
	if (nInput == PLAYERINPUT_SERVER_CLOSE
		|| nInput == PLAYERINPUT_LEAVE_GAME)
	{
		// �������ڱ���״̬
		player.nSavingSec = 0;
		SetPlayerState(player, PLAYERSTATE_SAVING);

		// �Ͽ��ͻ���
		BreakClient(player);

		// ͬ������״̬
		CommandPlayerOffline(player);

		// ɾ����Ϸ����ɫ
		CommandPlayerLeaveGame(player);
	}
	else if (nInput == PLAYERINPUT_USER_BREAK)
	{
		// �ӳ�ɾ��ʱ��
		player.nRemoveSec = 120;
		// ��������״̬
		SetPlayerState(player, PLAYERSTATE_OFFLINE);

		// �Ͽ��ͻ���
		BreakClient(player);

		// ͬ������״̬
		CommandPlayerOffline(player);
	}
	else if (PlayerStateAny(player, nInput, msg, nLen) > 0)
	{
		return 1;
	}
	else
	{
		QLOG_WARING << __FUNCTION__ << " error player input:" << nInput << " player:" << player.self;
		return 0;
	}

	return 1;
}

int CWorldPlayers::PlayerStateOffline(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
{
	if (nInput == PLAYERINPUT_TIMER)
	{
		if (--player.nRemoveSec < 0)
		{
			// �������ڱ���״̬
			player.nSavingSec = 0;
			SetPlayerState(player, PLAYERSTATE_SAVING);

			// ɾ����Ϸ����ɫ
			CommandPlayerLeaveGame(player);
		}
	}
	else if (nInput == PLAYERINPUT_SERVER_CLOSE 
		|| nInput == PLAYERINPUT_LEAVE_GAME)
	{
		// �������ڱ���״̬
		player.nSavingSec = 0;
		SetPlayerState(player, PLAYERSTATE_SAVING);

		// �Ͽ��ͻ���
		BreakClient(player);

		// ɾ����Ϸ����ɫ
		CommandPlayerLeaveGame(player);
	}
	else if (nInput == PLAYERINPUT_USER_BREAK)
	{
		// �Ͽ��ͻ���
		BreakClient(player);
	}
	else if (PlayerStateAny(player, nInput, msg, nLen) > 0)
	{
		return 1;
	}
	else
	{
		QLOG_WARING << __FUNCTION__ << " error player input:" << nInput << " player:" << player.self;
		return 0;
	}

	return 1;
}

int CWorldPlayers::PlayerStateSaving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
{
	if (nInput == PLAYERINPUT_STORE_SAVED)
	{
		RemovePlayer(player.self);
	}
	else if (nInput == PLAYERINPUT_TIMER)
	{
		if (++player.nSavingSec > SAVING_SEC_MAX)
		{
			QLOG_ERROR << __FUNCTION__ << " save player timeout player:" << player.self;

			RemovePlayer(player.self);
		}
	}
	else if (nInput == PLAYERINPUT_USER_BREAK)
	{
		// �Ͽ��ͻ���
		BreakClient(player);
	}
	else if (PlayerStateAny(player, nInput, msg, nLen) > 0)
	{
		return 1;
	}
	else
	{
		QLOG_WARING << __FUNCTION__ << " error player input:" << nInput << " player:" << player.self;
		return 0;
	}

	return 1;
}

//int CWorldPlayers::PlayerStateLeaving(PlayerData& player, int nInput, const char* msg, const uint32_t nLen)
//{
//	if (PlayerStateAny(player, nInput, msg, nLen) > 0)
//	{
//		return 1;
//	}
//	else
//	{
//		std::ostringstream str;
//		str << "error player input:" << nInput;
//		m_pLogModule->LogWarning(player.self, str, __FUNCTION__, __LINE__);
//		return 0;
//	}
//
//	return 1;
//}
