///--------------------------------------------------------------------
/// �ļ���:		ChatModule.cpp
/// ��  ��:		���췿�������
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "ChatModule.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/QLOG.h"
#include "NFComm/NFUtils/StringUtil.h"
#include "mysqlpp/ssqls.h"

// mysql���Ӧ�Ľṹ
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
	//����������Ϣ
	SaveOfflineMsgToDB();

	//���淿�������¼
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
	//���������Ϣ��ʱ
	CheckOfflineMsgOutTime();

	return true;
}

//���������ó�ʼ��
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

	//�����ݿ����������Ϣ
	RestoreOfflineMsg();

	return true;
}

//����һ������
/// \param szKey ��ͬKEY������ͬ�ķ���ID
/// \return ����ID
int ChatModule::CreateRoom(const std::string& key, const bool isRecording)
{
	if (key.empty())
	{
		QLOG_WARING << __FUNCTION__ << " key is empty";
		return -1;
	}

	//�Ƿ��Ѵ���
	auto it = mRoomIndexMap.find(key);
	if (mRoomIndexMap.end() != it)
	{
		return it->second;
	}

	//�����·���
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

	//�����¼��KEY
	mRoomIndexMap.insert(std::make_pair(key, nRoomId));

	//û����ҵķ������
	mNoPlayerRoomMap.insert(std::make_pair(nRoomId, time(nullptr)));

	return nRoomId;
}

//�Ƴ�һ������
/// \param Id ����ID
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

	//�ѷ�����������Ҽ�¼ȥ��
	const std::set<PlayerData*>& playerDataSet = pRoom->GetPlayerList();
	std::set<PlayerData*>::const_iterator itBegin = playerDataSet.begin();
	std::set<PlayerData*>::const_iterator itEnd = playerDataSet.end();
	for (; itBegin != itEnd; ++itBegin)
	{
		(*itBegin)->LeaveRoom(Id);
	}

	//���շ������
	pRoom->SetFreeState(true);
	mRoomIndexMap.erase(it);
	mRoomFreeQueue.push(Id);

	//û����ҵķ������
	mNoPlayerRoomMap.erase(Id);
}

//ȡ��һ������
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

//���ӿͻ��ǵ�����
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

	//û����ҵķ������
	mNoPlayerRoomMap.erase(nRoomId);

	return true;
}

//�Ƴ�����һ���ͻ���
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

	//û����ҵķ������
	if (pRoom->GetPlayerSize() == 0)
	{
		mNoPlayerRoomMap.insert(std::make_pair(nRoomId, time(nullptr)));
	}
}

//û����ҵķ������
const std::map<int, time_t>& ChatModule::GetNoPlayerRoomMap()
{
	return mNoPlayerRoomMap;
}

//������һ�����
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

	//��¼��û�пͻ��˵����
	mNoClientPlayerMap.insert(std::make_pair(pPlayerData, time(nullptr)));

	return pPlayerData;
}

//ɾ��һ�����
void ChatModule::RemovePlayerData(const NFGUID& uid)
{
	auto it = mPlayerDataByGuidMap.find(uid);
	if (mPlayerDataByGuidMap.end() == it)
	{
		return;
	}
	PlayerData* pPlayerData = it->second;
	
	//�˳�������ڷ���
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

	//������Ҷ���
	mPlayerDataMap.erase(pPlayerData->GetSockIndex());
	mPlayerDataByGuidMap.erase(it);
	mNoClientPlayerMap.erase(pPlayerData);
	pPlayerData->Rese();
	mPlayerDataFreeQueue.push(pPlayerData);
}

//ȡ��һ���������
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

//��������ID
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

	//ɾ��û�пͻ��˵���Ҷ���
	mNoClientPlayerMap.erase(pPlayerData);

	//��ȡ����������Ϣ
	ReceiveOfflineMsg(uid);

	return true;
}

//�������ID
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

	//��¼��û�пͻ��˵����
	mNoClientPlayerMap.insert(std::make_pair(pPlayerData, time(nullptr)));

	return true;
}

//û�пͻ��˵���Ҷ���
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

//������Ϣ�����
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

//ȡ�������¼
const std::vector<Room::RowData*>* ChatModule::GetChatRecording(const size_t nRoomId)
{
	Room* pRoom = GetRoom(nRoomId);
	if (nullptr == pRoom)
	{
		return nullptr;
	}

	return &pRoom->GetChatRecording();
}

//����������Ϣ
void ChatModule::AddOfflineMsg(const NFGUID& receive, const OuterMsg::ChatMsgPlayer& msg, const time_t time)
{
	if (mOfflineMsgMap.size() >= 10000)
	{
		//������ɾ��һ��
		mOfflineMsgMap.erase(mOfflineMsgOutTime.front());
		mOfflineMsgOutTime.pop();
	}

	ChatModule::OfflineMsg xmsg;
	xmsg.time = time;
	xmsg.msg = msg;
	mOfflineMsgMap[receive].push(std::move(xmsg));

	mOfflineMsgOutTime.push(receive);
}

//��ȡ����������Ϣ
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

//������Ϣ�������ݿ�
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

	//������
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

//�����ݿ����������Ϣ
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

//���������Ϣ��ʱ
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
			//������ѭ������Ϊ����Ķ�û��ʱ
			return;
		}
	}
}