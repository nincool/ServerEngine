///--------------------------------------------------------------------
/// �ļ���:		Room.cpp
/// ��  ��:		���췿��
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "Room.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMysqlPlugin/NFCMysqlDriver.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NFUtils/StringUtil.h"
#include "mysqlpp/ssqls.h"



// mysql���Ӧ�Ľṹ
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

//��ʼ��
void Room::Init(const bool recording, const std::string& key)
{
	mFreeState = false;
	mClientSet.clear();
	mIsRecording = recording;
	mKey = key;

	//ֻ�й̶�����ż������¼
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

//������ҵ�����
void Room::AddPlayer(PlayerData* pPlayerData)
{
	pPlayerData->EnterRoom(mId);
	mClientSet.insert(pPlayerData);
}

//�Ƴ�����һ�����
void Room::RemovePlayer(PlayerData* pPlayerData)
{
	pPlayerData->LeaveRoom(mId);
	mClientSet.erase(pPlayerData);
}

//ȡ�÷�����Ҽ���
const std::set<PlayerData*>& Room::GetPlayerList()
{
	return mClientSet;
}

//ȡ�õ�ǰ��������
size_t Room::GetPlayerSize()
{
	return mClientSet.size();
}

bool Room::GetmIsRecording()
{
	return mIsRecording;
}

//������Ϣ
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

	//�����¼
	if (mIsRecording)
	{
		AddRecording(msg.sender_player(), strMsg, ::time(nullptr));
	}
}

//�������¼д�����ݿ�
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

//�����ݿ�ָ������¼
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

//���������¼
void Room::AddRecording(const OuterMsg::GUID& uid, const std::string& content, const time_t time)
{
	//ɾ����ǰ��
	if (!m_wait_write_vec.empty() && m_wait_write_vec.size() >= m_pChatServerInit->GetRecordingMax())
	{
		Room::RowData* pRowData = m_wait_write_vec[0];
		m_RowDataPool.push(pRowData);

		m_wait_write_vec.erase(m_wait_write_vec.begin());
	}

	//�����µ�
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

//ȡ�������¼
const std::vector<Room::RowData*>& Room::GetChatRecording()
{
	return m_wait_write_vec;
}