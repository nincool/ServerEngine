///--------------------------------------------------------------------
/// 文件名:		Room.cpp
/// 内  容:		聊天房间
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "Room.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMysqlPlugin/NFCMysqlDriver.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NFUtils/StringUtil.h"
#include "mysqlpp/ssqls.h"



// mysql表对应的结构
sql_create_4(ChatRecording,
	1, 4,
	mysqlpp::sql_int, id,
	mysqlpp::sql_varchar, strSendID,
	mysqlpp::sql_blob, strComment,
	mysqlpp::sql_datetime, nTime);


Room::Room(int Id, NFIPluginManager* p)
{
	mId = Id;
	m_pNetServerOuterModule = p->FindModule<NetServerOuterModule>();
	m_pMysqlModule = p->FindModule<NFMysqlModule>();
	m_pChatServerInit = p->FindModule<ChatServerInit>();

	m_pNFCMysqlDriver = m_pMysqlModule->GetMysqlDriver(m_pChatServerInit->GetDBName());
}

Room::~Room()
{
	RecordingToDB();
}

int Room::GetId()
{
	return mId;
}

const std::string& Room::GetKey()
{
	return mKey;
}

//初始化
void Room::Init(const bool recording, const std::string& key)
{
	mFreeState = false;
	mClientSet.clear();
	mIsRecording = recording;
	mKey = key;

	//只有固定房间才记聊天记录
	if (mIsRecording && nullptr != m_pNFCMysqlDriver)
	{
		std::string sql = "CREATE TABLE IF NOT EXISTS ChatRecording_" + mKey + "( \
									id int(11) PRIMARY KEY,\
									strSendID varchar(255), \
									strComment longblob,\
									nTime DateTime\
								);";
			
		m_pNFCMysqlDriver->Query(sql);

		RestoreRecording();
	}
}

void Room::SetFreeState(bool bState)
{
	mFreeState = bState;

	if (bState)
	{
		RecordingToDB();

		auto it = mClientSet.begin();
		auto it_end = mClientSet.end();
		for (; it != it_end; ++it)
		{
			(*it)->LeaveRoom(mId);
		}
		mClientSet.clear();
	}
}

bool Room::GetFreeState()
{
	return mFreeState;
}

//增加玩家到房间
void Room::AddPlayer(PlayerData* pPlayerData)
{
	pPlayerData->EnterRoom(mId);
	mClientSet.insert(pPlayerData);
}

//移除房间一个玩家
void Room::RemovePlayer(PlayerData* pPlayerData)
{
	pPlayerData->LeaveRoom(mId);
	mClientSet.erase(pPlayerData);
}

//取得房间玩家集合
const std::set<PlayerData*>& Room::GetPlayerList()
{
	return mClientSet;
}

//取得当前房间人数
size_t Room::GetPlayerSize()
{
	return mClientSet.size();
}

bool Room::GetmIsRecording()
{
	return mIsRecording;
}

//发送消息
void Room::SendMsg(const OuterMsg::ChatMsgRoom& msg)
{
	std::string strMsg;
	msg.SerializeToString(&strMsg);

	std::list<NFSOCK> list;
	auto it = mClientSet.begin();
	auto itEnd = mClientSet.end();
	for (; it != itEnd; ++it)
	{
		NFSOCK nSockIndex = (*it)->GetSockIndex();
		if (nSockIndex <= -1)
		{
			continue;
		}
		list.push_back(nSockIndex);
	}
	m_pNetServerOuterModule->SendMsgToList(OuterMsg::CS_CHAT_MSG_ROOM, strMsg, list);

	//聊天记录
	if (mIsRecording)
	{
		AddRecording(msg.sender_player(), strMsg, ::time(nullptr));
	}
}

//把聊天记录写到数据库
void Room::RecordingToDB()
{
	if (!mIsRecording || nullptr == m_pNFCMysqlDriver)
	{
		return;
	}

	auto* pConn = m_pNFCMysqlDriver->GetConnection();
	if (pConn == nullptr)
	{
		return;
	}

	std::string table_name = "ChatRecording_";
	table_name.append(mKey);
	
	auto it = m_wait_write_vec.begin();
	auto it_end = m_wait_write_vec.end();
	int id = 0;
	for (; it != it_end; ++it)
	{
		Room::RowData* pRowData = *it;

		ChatRecording save_row;
		save_row.instance_table(table_name.c_str());
		save_row.id = id;
		save_row.strSendID = (pRowData->uid.ToString());
		save_row.strComment = pRowData->content;
		save_row.nTime = mysqlpp::DateTime(pRowData->time);
		++id;

		auto query = pConn->query();
		query.reset();
		query.insert(save_row);
		query.execute();


		m_RowDataPool.push(pRowData);
	}
	m_wait_write_vec.clear();
}

//从数据库恢复聊天记录
void Room::RestoreRecording()
{
	if (!mIsRecording || nullptr == m_pNFCMysqlDriver)
	{
		return;
	}

	std::string sql = "SELECT * FROM ChatRecording_" + mKey;
	mysqlpp::StoreQueryResult queryResult;
	m_pNFCMysqlDriver->Query(sql, queryResult);

	auto it = queryResult.begin();
	auto it_end = queryResult.end();
	for (; it != it_end; ++it)
	{
		mysqlpp::Row& row = *it;
		OuterMsg::GUID gid;
		std::string strUid;
		row[1].to_string(strUid);
		gid.ParseFromString(strUid);
		AddRecording(gid, row[2].c_str(), StringUtil::StringAsInt64(row[3].c_str()));
	}

	if (!queryResult.empty())
	{
		sql = "TRUNCATE TABLE ChatRecording_" + mKey;
		m_pNFCMysqlDriver->Query(sql);
	}
}

//增加聊天记录
void Room::AddRecording(const OuterMsg::GUID& uid, const std::string& content, const time_t time)
{
	//删除以前的
	if (!m_wait_write_vec.empty() && m_wait_write_vec.size() >= m_pChatServerInit->GetRecordingMax())
	{
		Room::RowData* pRowData = m_wait_write_vec[0];
		m_RowDataPool.push(pRowData);

		m_wait_write_vec.erase(m_wait_write_vec.begin());
	}

	//创建新的
	Room::RowData* pRowData = nullptr;
	if (m_RowDataPool.empty())
	{
		pRowData = new RowData();
	}
	else
	{
		pRowData = m_RowDataPool.front();
		m_RowDataPool.pop();
	}
	PBConvert::ToNF(uid, pRowData->uid);
	pRowData->time = time;
	pRowData->content = content;
	m_wait_write_vec.push_back(pRowData);
}

//取得聊天记录
const std::vector<Room::RowData*>& Room::GetChatRecording()
{
	return m_wait_write_vec;
}