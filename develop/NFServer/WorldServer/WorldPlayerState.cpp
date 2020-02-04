///--------------------------------------------------------------------
/// 文件名:		WorldPlayerState.h
/// 内  容:		世界服务器玩家状态机
/// 说  明:		
/// 创建日期:	2019年10月12日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "WorldPlayers.h"

// 保存等待最大秒 (以后可以外部配置)
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
	//case PLAYERSTATE_UNKNOWN:	// 初始状态
	//	PlayerStateUnknown(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_QUEUING:	// 正在排队
	//	PlayerStateQueuing(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_GETTING:	// 正在获得角色信息
	//	PlayerStateGetting(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_LOGINED:	// 登陆成功
	//	PlayerStateLogined(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_CHOOSING:	// 选择并加载角色数据
	//	PlayerStateChooseing(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_CREATING:	// 创建角色
	//	PlayerStateCreating(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_CREATE_CHECKING:	// 创建角色验证
	//	PlayerStateCreateChecking(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_DELETING:	// 删除角色
	//	PlayerStateDeleting(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_DELETING:	// 删除角色
	//	PlayerStateDeleting(player, nInput, msg, nLen);
	//	break;
	//case PLAYERSTATE_REVIVING:	// 恢复角色
	//	PlayerStateReviving(player, nInput, msg, nLen);
	//	break;
	case PLAYERSTATE_ONLINE:	// 已进入游戏
		PlayerStateOnline(player, nInput, msg, nLen);
		break;
	case PLAYERSTATE_OFFLINE:	// 已断线
		PlayerStateOffline(player, nInput, msg, nLen);
		break;
	case PLAYERSTATE_SAVING:	// 正在保存角色数据
		PlayerStateSaving(player, nInput, msg, nLen);
		break;
	//case PLAYERSTATE_LEAVING:	// 正在离开游戏
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
		// 不做处理
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
		// 设置正在保存状态
		player.nSavingSec = 0;
		SetPlayerState(player, PLAYERSTATE_SAVING);

		// 断开客户端
		BreakClient(player);

		// 同步离线状态
		CommandPlayerOffline(player);

		// 删除游戏服角色
		CommandPlayerLeaveGame(player);
	}
	else if (nInput == PLAYERINPUT_USER_BREAK)
	{
		// 延迟删除时间
		player.nRemoveSec = 120;
		// 进入离线状态
		SetPlayerState(player, PLAYERSTATE_OFFLINE);

		// 断开客户端
		BreakClient(player);

		// 同步离线状态
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
			// 设置正在保存状态
			player.nSavingSec = 0;
			SetPlayerState(player, PLAYERSTATE_SAVING);

			// 删除游戏服角色
			CommandPlayerLeaveGame(player);
		}
	}
	else if (nInput == PLAYERINPUT_SERVER_CLOSE 
		|| nInput == PLAYERINPUT_LEAVE_GAME)
	{
		// 设置正在保存状态
		player.nSavingSec = 0;
		SetPlayerState(player, PLAYERSTATE_SAVING);

		// 断开客户端
		BreakClient(player);

		// 删除游戏服角色
		CommandPlayerLeaveGame(player);
	}
	else if (nInput == PLAYERINPUT_USER_BREAK)
	{
		// 断开客户端
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
		// 断开客户端
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
