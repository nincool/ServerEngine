///--------------------------------------------------------------------
/// 文件名:		ChatModule.h
/// 内  容:		聊天房间管理器
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_ChatModule_H__
#define __H_ChatModule_H__
#include "Room.h"
#include <vector>
#include <map>
#include <queue>
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "ChatServerInit.h"

class ChatModule : public NFIModule
{
public:
	ChatModule(NFIPluginManager* p);
	virtual bool Execute();
	//管理器调用初始化
	virtual bool Init();
	virtual bool CommandShut();
	virtual bool ReadyExecute();
public:
	//创建一个房间
	/// \param szKey 相同KEY分配相同的房间ID
	/// \return 房间ID
	int CreateRoom(const std::string& key, const bool isRecording = false);
	//移除一个房间
	/// \param Id 房间ID
	void RemoveRoom(const std::string& key);
	//增加客户登到房间
	bool AddPlayerToRoom(const size_t nRoomId, NFGUID& playerGUID);
	//移除房间一个客户登
	void RemoveRoomPlayer(const size_t nRoomId, NFGUID& playerGUID);
	//没有玩家的房间对象
	const std::map<int, time_t>& GetNoPlayerRoomMap();

	//增加一个玩家
	PlayerData* AddPlayerData(const NFGUID& uid);
	//删除一个玩家
	void RemovePlayerData(const NFGUID& uid);
	//取得一个玩家数据
	PlayerData* GetPlayerData(const NFGUID& uid);
	PlayerData* GetPlayerData(const NFSOCK& nSock);
	//没有客户端的玩家对象
	const std::map<PlayerData*, time_t>& GetNoClientPlayerMap();

	//取一个房间ID
	int GetRoomID(const std::string& key);
	const std::string& GetRoomKey(const size_t nRoomId);

	//设置网络ID
	bool SetPlayerSockIndex(const NFGUID& uid, NFSOCK nSockIndex);
	//清除网络ID
	bool ClearPlayerSockIndex(const NFGUID& uid);

	//发送消息到房间
	bool SendMsgToRoom(const size_t nRoomId, const OuterMsg::ChatMsgRoom& msg);

	//发送消息给玩家
	bool SenddMsgToPlayer(const OuterMsg::ChatMsgPlayer& msg);

	//取得聊天记录
	const std::vector<Room::RowData*>* GetChatRecording(const size_t nRoomId);

private:
	//取得一个房间
	Room* GetRoom(const size_t nRoomID);

	//缓存离线消息
	void AddOfflineMsg(const NFGUID& receive, const OuterMsg::ChatMsgPlayer& msg, const time_t time);
	//收取缓存离线消息
	void ReceiveOfflineMsg(const NFGUID& receive);
	//离线消息保到数据库
	void SaveOfflineMsgToDB();
	//从数据库加载离线消息
	void RestoreOfflineMsg();
	//检查离线消息超时
	void CheckOfflineMsgOutTime();

private:
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	ChatServerInit* m_pChatServerInit = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
	NFCMysqlDriver* m_pNFCMysqlDriver = nullptr;

	//房间对象列表
	std::vector<Room*> mRoomVec;
	//有名字key的房间对象索引KEY
	std::map<std::string, int> mRoomIndexMap;
	//没分配的房间索引队列
	std::queue<int> mRoomFreeQueue;
	std::string strTemp;

	//没分配的玩家数据对象
	std::queue<PlayerData*> mPlayerDataFreeQueue;
	//在线玩家对象
	std::map<NFSOCK, PlayerData*> mPlayerDataMap;
	std::map<NFGUID, PlayerData*> mPlayerDataByGuidMap;

	//没有客户端的玩家对象
	std::map<PlayerData*, time_t> mNoClientPlayerMap;
	//没有玩家的房间对象
	std::map<int, time_t> mNoPlayerRoomMap;

	//私聊离消息
	struct OfflineMsg
	{
		time_t time = 0;
		OuterMsg::ChatMsgPlayer msg;
	};
	std::map<NFGUID, std::queue<ChatModule::OfflineMsg>> mOfflineMsgMap;
	std::queue<NFGUID> mOfflineMsgOutTime;
	NFINT64 mLastOfflineMsgCheckTime = 0;
};

#endif	//__H_RoomManage_H__