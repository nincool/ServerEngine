///--------------------------------------------------------------------
/// 文件名:		Room.h
/// 内  容:		聊天房间
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_Room_H__
#define __H_Room_H__
#include <set>
#include <queue>
#include "PlayerData.h"
#include "ChatServerInit.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "NFComm/NFPluginModule/NFGUID.h"

class NFCMysqlDriver;
class Room
{
public:
	//聊天记录行
	struct RowData
	{
		NFGUID uid;
		time_t time = 0;
		std::string content;
	};

	Room(int Id, NFIPluginManager* p);
	~Room();

	//初始化
	void Init(const bool recording, const std::string& key);

	int GetId();
	const std::string& GetKey();

	//回收状态
	void SetFreeState(bool bState);
	//回收状态
	bool GetFreeState();

	//固定类型房间
	bool GetmIsRecording();

	//增加玩家到房间
	void AddPlayer(PlayerData* pPlayerData);
	//移除房间一个玩家
	void RemovePlayer(PlayerData* pPlayerData);

	//取得房间玩家集合
	const std::set<PlayerData*>& GetPlayerList();
	//取得当前房间人数
	size_t GetPlayerSize();

	//发送消息
	void SendMsg(const OuterMsg::ChatMsgRoom& msg);

	//取得聊天记录
	const std::vector<Room::RowData*>& GetChatRecording();

private:
	//把聊天记录写到数据库
	void RecordingToDB();

	//从数据库恢复聊天记录
	void RestoreRecording();

	//增加聊天记录
	void AddRecording(const OuterMsg::GUID& uid, const std::string& content, const time_t time);

private:
	int mId = -1;
	//当前就是否在回收状态
	bool mFreeState = false;
	//玩家集合
	std::set<PlayerData*> mClientSet;
	//固定类型房间
	bool mIsRecording = false;
	//房间KEY
	std::string mKey;

	//聊天记录
	std::vector<Room::RowData*> m_wait_write_vec;
	std::queue<Room::RowData*> m_RowDataPool;

	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
	ChatServerInit* m_pChatServerInit = nullptr;
	NFCMysqlDriver* m_pNFCMysqlDriver = nullptr;
};

#endif	//__H_Room_H__