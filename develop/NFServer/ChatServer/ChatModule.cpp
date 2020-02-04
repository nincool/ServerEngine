///--------------------------------------------------------------------
/// 文件名:		ChatModule.cpp
/// 内  容:		聊天房间管理器
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ChatModule.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/QLOG.h"
#include "NFComm/NFUtils/StringUtil.h"
#include "mysqlpp/ssqls.h"

// mysql表对应的结构
sql_create_3(Chat_Offline_Msg,
	1, 3,
	mysqlpp::sql_int, id,
	mysqlpp::sql_datetime, nTime,
	mysqlpp::sql_blob, strComment);

ChatModule::ChatModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

bool ChatModule::CommandShut()
{
	//保存离线消息
	SaveOfflineMsgToDB();

	//保存房间聊天记录
	auto it = mRoomIndexMap.begin();
	auto it_end = mRoomIndexMap.end();
	for (; it != it_end; ++it)
	{
		delete mRoomVec[it->second];
	}

	pPluginManager->SetExit(true);
	return true;
}

bool ChatModule::Execute()
{
	//检查离线消息超时
	CheckOfflineMsgOutTime();

	return true;
}

//管理器调用初始化
bool ChatModule::Init()
{
	m_pNetServerOuterModule = pPluginManager->FindModule<NetServerOuterModule>();
	m_pChatServerInit = pPluginManager->FindModule<ChatServerInit>();
	m_pMysqlModule = pPluginManager->FindModule<NFMysqlModule>();

	return true;
}

bool ChatModule::ReadyExecute()
{
	SaveOfflineMsgToDB();

	//从数据库加载离线消息
	RestoreOfflineMsg();

	return true;
}

//创建一个房间
/// \param szKey 相同KEY分配相同的房间ID
/// \return 房间ID
int ChatModule::CreateRoom(const std::string& key, const bool isRecording)
{
	if (key.empty())
	{
		QLOG_WARING << __FUNCTION__ << " key is empty";
		return -1;
	}

	//是否已存在
	auto it = mRoomIndexMap.find(key);
	if (mRoomIndexMap.end() != it)
	{
		return it->second;
	}

	//分配新房间
	int nRoomId = -1;
	if (!mRoomFreeQueue.empty())
	{
		nRoomId = mRoomFreeQueue.front();
		mRoomFreeQueue.pop();
		mRoomVec[nRoomId]->SetFreeState(false);
	}
	else
	{
		auto* pRoom = new Room((int)mRoomVec.size(), pPluginManager);
		mRoomVec.push_back(pRoom);
		nRoomId = pRoom->GetId();
	}
	mRoomVec[nRoomId]->Init(isRecording, key);

	//房间记录到KEY
	mRoomIndexMap.insert(std::make_pair(key, nRoomId));

	//没有玩家的房间对象
	mNoPlayerRoomMap.insert(std::make_pair(nRoomId, time(nullptr)));

	return nRoomId;
}

//移除一个房间
/// \param Id 房间ID
void ChatModule::RemoveRoom(const std::string& key)
{
	auto it = mRoomIndexMap.find(key);
	if (mRoomIndexMap.end() == it)
	{
		return;
	}

	int Id = it->second;
	Room* pRoom = GetRoom(Id);
	if (nullptr == pRoom)
	{
		return;
	}

	//把房间里所有玩家记录去掉
	const std::set<PlayerData*>& playerDataSet = pRoom->GetPlayerList();
	std::set<PlayerData*>::const_iterator itBegin = playerDataSet.begin();
	std::set<PlayerData*>::const_iterator itEnd = playerDataSet.end();
	for (; itBegin != itEnd; ++itBegin)
	{
		(*itBegin)->LeaveRoom(Id);
	}

	//回收房间对象
	pRoom->SetFreeState(true);
	mRoomIndexMap.erase(it);
	mRoomFreeQueue.push(Id);

	//没有玩家的房间对象
	mNoPlayerRoomMap.erase(Id);
}

//取得一个房间
Room* ChatModule::GetRoom(const size_t nRoomID)
{
	if (nRoomID >= mRoomVec.size())
	{
		QLOG_WARING << __FUNCTION__ << " roomid out range id:" << nRoomID
			<< " size:" << mRoomVec.size();
		return nullptr;
	}

	if (nullptr == mRoomVec[nRoomID])
	{
		QLOG_WARING << __FUNCTION__ << " roomid not exist id:" << nRoomID;
		return nullptr;
	}

	if (mRoomVec[nRoomID]->GetFreeState())
	{
		QLOG_WARING << __FUNCTION__ << " roomid is freestate id:" << nRoomID;
		return nullptr;
	}

	return mRoomVec[nRoomID];
}

//增加客户登到房间
bool ChatModule::AddPlayerToRoom(const size_t nRoomId, NFGUID& playerGUID)
{
	Room* pRoom = GetRoom(nRoomId);
	if (nullptr == pRoom)
	{
		return false;
	}

	auto it = mPlayerDataByGuidMap.find(playerGUID);
	if (mPlayerDataByGuidMap.end() == it)
	{
		QLOG_WARING << __FUNCTION__ << " playerGUID already exist:" << playerGUID;
		return false;
	}
	PlayerData* pPlayerData = it->second;

	pRoom->AddPlayer(pPlayerData);
	pPlayerData->EnterRoom(nRoomId);

	//没有玩家的房间对象
	mNoPlayerRoomMap.erase(nRoomId);

	return true;
}

//移除房间一个客户登
void ChatModule::RemoveRoomPlayer(const size_t nRoomId, NFGUID& playerGUID)
{
	Room* pRoom = GetRoom(nRoomId);
	if (nullptr == pRoom)
	{
		QLOG_WARING << __FUNCTION__ << " room id not exist:" << nRoomId;
		return;
	}

	auto it = mPlayerDataByGuidMap.find(playerGUID);
	if (mPlayerDataByGuidMap.end() == it)
	{
		QLOG_WARING << __FUNCTION__ << " playerGUID not exist:" << playerGUID;
		return;
	}
	PlayerData* pPlayerData = it->second;

	pRoom->RemovePlayer(pPlayerData);
	pPlayerData->LeaveRoom(nRoomId);

	//没有玩家的房间对象
	if (pRoom->GetPlayerSize() == 0)
	{
		mNoPlayerRoomMap.insert(std::make_pair(nRoomId, time(nullptr)));
	}
}

//没有玩家的房间对象
const std::map<int, time_t>& ChatModule::GetNoPlayerRoomMap()
{
	return mNoPlayerRoomMap;
}

//增加在一个玩家
PlayerData* ChatModule::AddPlayerData(const NFGUID& uid)
{
	if (mPlayerDataByGuidMap.find(uid) != mPlayerDataByGuidMap.end())
	{
		return nullptr;
	}

	PlayerData* pPlayerData = nullptr;
	if (mPlayerDataFreeQueue.empty())
	{
		pPlayerData = new PlayerData();
	}
	else
	{
		pPlayerData = mPlayerDataFreeQueue.front();
		mPlayerDataFreeQueue.pop();
	}
	pPlayerData->Init(uid);

	mPlayerDataByGuidMap.insert(std::make_pair(uid, pPlayerData));

	//记录还没有客户端的玩家
	mNoClientPlayerMap.insert(std::make_pair(pPlayerData, time(nullptr)));

	return pPlayerData;
}

//删除一个玩家
void ChatModule::RemovePlayerData(const NFGUID& uid)
{
	auto it = mPlayerDataByGuidMap.find(uid);
	if (mPlayerDataByGuidMap.end() == it)
	{
		return;
	}
	PlayerData* pPlayerData = it->second;
	
	//退出玩家所在房间
	const std::set<int> setRoomID = pPlayerData->GetRoomIdSet();
	auto room_id_it = setRoomID.begin();
	auto room_id_it_end = setRoomID.end();
	for (; room_id_it != room_id_it_end; ++room_id_it)
	{
		Room* pRoom = GetRoom(*room_id_it);
		if (nullptr == pRoom)
		{
			continue;
		}
		pRoom->RemovePlayer(pPlayerData);
	}

	//回收玩家对象
	mPlayerDataMap.erase(pPlayerData->GetSockIndex());
	mPlayerDataByGuidMap.erase(it);
	mNoClientPlayerMap.erase(pPlayerData);
	pPlayerData->Rese();
	mPlayerDataFreeQueue.push(pPlayerData);
}

//取得一个玩家数据
PlayerData* ChatModule::GetPlayerData(const NFGUID& uid)
{
	auto it = mPlayerDataByGuidMap.find(uid);
	if (mPlayerDataByGuidMap.end() == it)
	{
		return nullptr;
	}

	return it->second;
}

PlayerData* ChatModule::GetPlayerData(const NFSOCK& nSock)
{
	auto it = mPlayerDataMap.find(nSock);
	if (mPlayerDataMap.end() == it)
	{
		QLOG_WARING << __FUNCTION__ << " playersock not exist:" << nSock;
		return nullptr;
	}

	return it->second;
}

int ChatModule::GetRoomID(const std::string& key)
{
	auto it = mRoomIndexMap.find(key);
	if (mRoomIndexMap.end() == it)
	{
		return -1;
	}

	return it->second;
}

const std::string& ChatModule::GetRoomKey(const size_t nRoomId)
{
	Room* pRoom = GetRoom(nRoomId);
	if (nullptr == pRoom)
	{
		return strTemp;
	}

	return pRoom->GetKey();
}

//设置网络ID
bool ChatModule::SetPlayerSockIndex(const NFGUID& uid, NFSOCK nSockIndex)
{
	PlayerData* pPlayerData = GetPlayerData(uid);
	if (nullptr == pPlayerData)
	{
		QLOG_WARING << __FUNCTION__ << " playerGUID not exist:" << uid;
		return false;
	}

	pPlayerData->SetSockIndex(nSockIndex);
	mPlayerDataMap[nSockIndex] = pPlayerData;

	//删除没有客户端的玩家对象
	mNoClientPlayerMap.erase(pPlayerData);

	//收取缓存离线消息
	ReceiveOfflineMsg(uid);

	return true;
}

//清除网络ID
bool ChatModule::ClearPlayerSockIndex(const NFGUID& uid)
{
	PlayerData* pPlayerData = GetPlayerData(uid);
	if (nullptr == pPlayerData)
	{
		QLOG_WARING << __FUNCTION__ << " playerGUID not exist:" << uid;
		return false;
	}

	pPlayerData->SetSockIndex(-1);
	mPlayerDataMap.erase(pPlayerData->GetSockIndex());

	//记录还没有客户端的玩家
	mNoClientPlayerMap.insert(std::make_pair(pPlayerData, time(nullptr)));

	return true;
}

//没有客户端的玩家对象
const std::map<PlayerData*, time_t>& ChatModule::GetNoClientPlayerMap()
{
	return mNoClientPlayerMap;
}

bool ChatModule::SendMsgToRoom(const size_t nRoomId, const OuterMsg::ChatMsgRoom& msg)
{
	Room* pRoom = GetRoom(nRoomId);
	if (nullptr == pRoom)
	{
		return false;
	}

	pRoom->SendMsg(msg);

	return true;
}

//发送消息给玩家
bool ChatModule::SenddMsgToPlayer(const OuterMsg::ChatMsgPlayer& msg)
{
	NFGUID target_player;
	PBConvert::ToNF(msg.target_player(), target_player);

	PlayerData* pTargetPlayerData = GetPlayerData(target_player);
	if (nullptr == pTargetPlayerData || pTargetPlayerData->GetSockIndex() == -1)
	{
		AddOfflineMsg(target_player, msg, pPluginManager->GetNowTime());
	}
	else
	{
		m_pNetServerOuterModule->SendMsg(OuterMsg::CS_CHAT_MSG_PLAYER, msg, pTargetPlayerData->GetSockIndex());
	}

	return true;
}

//取得聊天记录
const std::vector<Room::RowData*>* ChatModule::GetChatRecording(const size_t nRoomId)
{
	Room* pRoom = GetRoom(nRoomId);
	if (nullptr == pRoom)
	{
		return nullptr;
	}

	return &pRoom->GetChatRecording();
}

//缓存离线消息
void ChatModule::AddOfflineMsg(const NFGUID& receive, const OuterMsg::ChatMsgPlayer& msg, const time_t time)
{
	if (mOfflineMsgMap.size() >= 10000)
	{
		//出错超容删除一个
		mOfflineMsgMap.erase(mOfflineMsgOutTime.front());
		mOfflineMsgOutTime.pop();
	}

	ChatModule::OfflineMsg xmsg;
	xmsg.time = time;
	xmsg.msg = msg;
	mOfflineMsgMap[receive].push(std::move(xmsg));

	mOfflineMsgOutTime.push(receive);
}

//收取缓存离线消息
void ChatModule::ReceiveOfflineMsg(const NFGUID& receive)
{
	auto it = mOfflineMsgMap.find(receive);
	if (mOfflineMsgMap.end() == it)
	{
		return;
	}

	size_t size = it->second.size();
	for (size_t i = 0; i < size; ++i)
	{
		SenddMsgToPlayer(it->second.front().msg);
		it->second.pop();
	}
	mOfflineMsgMap.erase(it);
}

//离线消息保到数据库
void ChatModule::SaveOfflineMsgToDB()
{
	m_pNFCMysqlDriver = m_pMysqlModule->GetMysqlDriver(m_pChatServerInit->GetDBName());
	if (nullptr == m_pNFCMysqlDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " m_pNFCMysqlDriver == NULL";
		return;
	}

	auto* pConn = m_pNFCMysqlDriver->GetConnection();
	if (pConn == nullptr)
	{
		return;
	}

	//创建表
	std::string sql = "CREATE TABLE IF NOT EXISTS Chat_Offline_Msg ( \
									id int(11) PRIMARY KEY,\
									nTime DateTime, \
									strComment longblob\
								);";
	m_pNFCMysqlDriver->Query(sql);


	size_t size = 0;
	auto it = mOfflineMsgMap.begin();
	auto it_end = mOfflineMsgMap.end();
	int id = 0;
	for (; it != it_end; ++it)
	{
		size = it->second.size();
		for (size_t i = 0; i < size; ++i)
		{
			ChatModule::OfflineMsg& msg = it->second.front();
			std::string strMsg;
			msg.msg.SerializeToString(&strMsg);

			Chat_Offline_Msg save_row;
			save_row.instance_table("Chat_Offline_Msg");
			save_row.nTime = mysqlpp::DateTime(msg.time);
			save_row.strComment = strMsg;
			save_row.id = id;
			++id;

			auto query = pConn->query();
			query.reset();
			query.insert(save_row);
			query.execute();

			it->second.pop();
		}
	}
}

//从数据库加载离线消息
void ChatModule::RestoreOfflineMsg()
{
	m_pNFCMysqlDriver = m_pMysqlModule->GetMysqlDriver(m_pChatServerInit->GetDBName());
	if (nullptr == m_pNFCMysqlDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " m_pNFCMysqlDriver == NULL";
		return;
	}

	int p = 0;
	int s = 500;
	while (true)
	{
		std::string sql = "SELECT * FROM Chat_Offline_Msg limit ";
		sql += to_string(p);
		sql += ", ";
		sql += to_string(p * s);

		++p;

		mysqlpp::StoreQueryResult queryResult;
		m_pNFCMysqlDriver->Query(sql, queryResult);

		size_t size = queryResult.size();
		if (size == 0)
		{
			return;
		}
		for (int i = 0; i < size; ++i)
		{
			time_t time = StringUtil::StringAsInt64(queryResult[i][1].c_str());
			OuterMsg::ChatMsgPlayer msg;
			msg.ParseFromString(queryResult[i][2].c_str());
			NFGUID uid;
			PBConvert::ToNF(msg.target_player(), uid);

			AddOfflineMsg(uid, msg, time);
		}
	}

	std::string sql = "TRUNCATE TABLE Chat_Offline_Msg";
	m_pNFCMysqlDriver->Query(sql);
}

//检查离线消息超时
void ChatModule::CheckOfflineMsgOutTime()
{
	if (mLastOfflineMsgCheckTime + 1 > pPluginManager->GetNowTime())
	{
		return;
	}
	mLastOfflineMsgCheckTime = pPluginManager->GetNowTime();

	while (!mOfflineMsgOutTime.empty())
	{
		NFGUID uid = mOfflineMsgOutTime.front();
		auto it = mOfflineMsgMap.find(uid);
		if (mOfflineMsgMap.end() == it)
		{
			mOfflineMsgOutTime.pop();
			continue;
		}

		if (it->second.empty())
		{
			mOfflineMsgOutTime.pop();
			mOfflineMsgMap.erase(it);
			continue;
		}

		if (pPluginManager->GetNowTime() - it->second.front().time >= m_pChatServerInit->GetOfflineMsgCacheTimeMax())
		{
			mOfflineMsgOutTime.pop();
			it->second.pop();
			if (it->second.empty())
			{
				mOfflineMsgMap.erase(it);
			}
		}
		else
		{
			//结果这次循环，因为后面的都没超时
			return;
		}
	}
}