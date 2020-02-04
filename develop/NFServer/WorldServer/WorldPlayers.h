///--------------------------------------------------------------------
/// 文件名:		WorldPlayers.h
/// 内  容:		世界服务器玩管理器
/// 说  明:		
/// 创建日期:	2019年10月12日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

// 当前管理的状态和输入枚举有很多没有用,只实现极简的状态机，以后根据需要再修改扩充
// 由于world不是所有进程之间通信的中转，所以不能统管所有功能，导致结果是对一些流程只能做出标记，不能干预
// 所以对于玩家状态，基本是根据其他进程反馈过来的消息，来做出相应的状态标记，出现状态互斥也仅仅是输出错误日志
// 当然有主动查询玩家状态来控制流程的仍然是起作用的，比如关服流程。

#ifndef __WORLD_PLAYERS_H__
#define __WORLD_PLAYERS_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include <list>
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "WorldServer.h"

class NetServerInsideModule;

// 玩家状态
enum PLAYER_STATE_ENUM
{
	PLAYERSTATE_UNKNOWN,	// 初始状态
	//PLAYERSTATE_LOGINING,	// 正在登陆
	//PLAYERSTATE_QUEUING,	// 正在排队
	//PLAYERSTATE_GETTING,	// 正在获得角色信息
	//PLAYERSTATE_LOGINED,	// 登陆成功
	//PLAYERSTATE_CHOOSING,	// 选择并加载角色数据
	//PLAYERSTATE_CREATING,	// 创建角色
	//PLAYERSTATE_CREATE_CHECKING,	// 创建角色验证
	//PLAYERSTATE_DELETING,	// 删除角色
	//PLAYERSTATE_REVIVING,	// 恢复角色
	PLAYERSTATE_ONLINE,		// 已进入游戏
	PLAYERSTATE_OFFLINE,	// 已断线
	PLAYERSTATE_SAVING,		// 正在保存角色数据
	//PLAYERSTATE_LEAVING,	// 正在离开游戏
};

enum PLAYER_INPUT_ENUM
{
	PLAYERINPUT_TIMER,				// 定时器刷新
	//PLAYERINPUT_REMOVE,				// 强制删除
	PLAYERINPUT_USER_BREAK,			// 客户端断线
	//PLAYERINPUT_CLIENT_LOGIN,		// 客户端登录
	//PLAYERINPUT_CLIENT_CHOOSE_ROLE,	// 客户端选择角色
	//PLAYERINPUT_CLIENT_CREATE_ROLE,	// 客户端创建角色
	//PLAYERINPUT_CLIENT_CREATE_ROLE_CHECK,// 客户端创建角色验证
	//PLAYERINPUT_CLIENT_DELETE_ROLE,	// 客户端删除角色
	//PLAYERINPUT_CLIENT_REVIVE_ROLE,	// 客户端恢复角色
	//PLAYERINPUT_RETURN_NEW_ROLE,	// 返回新角色数据
	//PLAYERINPUT_ACCOUNT_ROLES,		// 返回帐号的角色信息
	//PLAYERINPUT_STORE_LOAD,			// 返回加载角色数据结果
	//PLAYERINPUT_STORE_CREATE,		// 返回创建角色结果
	//PLAYERINPUT_STORE_DELETE,		// 返回删除角色结果
	//PLAYERINPUT_STORE_REVIVE,		// 返回恢复角色结果
	PLAYERINPUT_STORE_SAVED,		// 保存玩家数据成功
	//PLAYERINPUT_END_QUEUE,			// 结束排队
	PLAYERINPUT_SERVER_CLOSE,		// 服务器关闭
	PLAYERINPUT_LEAVE_GAME,			// 请求离开游戏
	//PLAYERINPUT_LOGIN_WITH_ROLE,	// 登陆并选择角色
};

// 玩家数据
struct PlayerData
{	
	NFGUID self;		// 角色ID
	NFGUID client;		// 客户端ID
	int nGameID = 0;	// 游戏服ID
	int nGateID = 0;	// 接入服ID
	int nState = PLAYERSTATE_UNKNOWN; // 状态
	int nSavingSec = 0; // 保存等待秒数
	int nRemoveSec = 0; // 删除等待秒
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

	// 获取玩家数据
	const PlayerData* GetPlayer(const NFGUID& self);
	// 清空玩家
	void ClearPlayers();
	// 清楚玩家是否结束
	bool IsFinishClearPlayer();

	//当前在线人数
	size_t GetPlayerCount();

private:
	PlayerData& AddPlayer(const NFGUID& self, const NFGUID& client, int nGameID, int nGateID);
	bool RemovePlayer(const NFGUID& self);

	// 断开连接
	void BreakClient(const PlayerData& player);

private:

	//监听玩家角色上线消息
	void OnOnlineProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//玩家保存数据完成
	void OnPlayerSaved(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 客户端断线
	void OnUserBreak(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 请求离开游戏
	void OnLeaveGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 角色离开游戏
	void CommandPlayerLeaveGame(const PlayerData& player);
	// 同步离线状态
	void CommandPlayerOffline(const PlayerData& player);

private: // 状态机
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

	//玩家对象数据
	std::map<NFGUID, PlayerData> m_mapPlayers;
	//即将被删除的数据
	std::list<NFGUID> m_listRemovePlayers;

	// 上次时间戳
	int64_t m_dnLastTime = 0;
};

#endif //__WORLD_PLAYERS_H__