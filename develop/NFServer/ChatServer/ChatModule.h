///--------------------------------------------------------------------
/// �ļ���:		ChatModule.h
/// ��  ��:		���췿�������
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
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
	//���������ó�ʼ��
	virtual bool Init();
	virtual bool CommandShut();
	virtual bool ReadyExecute();
public:
	//����һ������
	/// \param szKey ��ͬKEY������ͬ�ķ���ID
	/// \return ����ID
	int CreateRoom(const std::string& key, const bool isRecording = false);
	//�Ƴ�һ������
	/// \param Id ����ID
	void RemoveRoom(const std::string& key);
	//���ӿͻ��ǵ�����
	bool AddPlayerToRoom(const size_t nRoomId, NFGUID& playerGUID);
	//�Ƴ�����һ���ͻ���
	void RemoveRoomPlayer(const size_t nRoomId, NFGUID& playerGUID);
	//û����ҵķ������
	const std::map<int, time_t>& GetNoPlayerRoomMap();

	//����һ�����
	PlayerData* AddPlayerData(const NFGUID& uid);
	//ɾ��һ�����
	void RemovePlayerData(const NFGUID& uid);
	//ȡ��һ���������
	PlayerData* GetPlayerData(const NFGUID& uid);
	PlayerData* GetPlayerData(const NFSOCK& nSock);
	//û�пͻ��˵���Ҷ���
	const std::map<PlayerData*, time_t>& GetNoClientPlayerMap();

	//ȡһ������ID
	int GetRoomID(const std::string& key);
	const std::string& GetRoomKey(const size_t nRoomId);

	//��������ID
	bool SetPlayerSockIndex(const NFGUID& uid, NFSOCK nSockIndex);
	//�������ID
	bool ClearPlayerSockIndex(const NFGUID& uid);

	//������Ϣ������
	bool SendMsgToRoom(const size_t nRoomId, const OuterMsg::ChatMsgRoom& msg);

	//������Ϣ�����
	bool SenddMsgToPlayer(const OuterMsg::ChatMsgPlayer& msg);

	//ȡ�������¼
	const std::vector<Room::RowData*>* GetChatRecording(const size_t nRoomId);

private:
	//ȡ��һ������
	Room* GetRoom(const size_t nRoomID);

	//����������Ϣ
	void AddOfflineMsg(const NFGUID& receive, const OuterMsg::ChatMsgPlayer& msg, const time_t time);
	//��ȡ����������Ϣ
	void ReceiveOfflineMsg(const NFGUID& receive);
	//������Ϣ�������ݿ�
	void SaveOfflineMsgToDB();
	//�����ݿ����������Ϣ
	void RestoreOfflineMsg();
	//���������Ϣ��ʱ
	void CheckOfflineMsgOutTime();

private:
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	ChatServerInit* m_pChatServerInit = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
	NFCMysqlDriver* m_pNFCMysqlDriver = nullptr;

	//��������б�
	std::vector<Room*> mRoomVec;
	//������key�ķ����������KEY
	std::map<std::string, int> mRoomIndexMap;
	//û����ķ�����������
	std::queue<int> mRoomFreeQueue;
	std::string strTemp;

	//û�����������ݶ���
	std::queue<PlayerData*> mPlayerDataFreeQueue;
	//������Ҷ���
	std::map<NFSOCK, PlayerData*> mPlayerDataMap;
	std::map<NFGUID, PlayerData*> mPlayerDataByGuidMap;

	//û�пͻ��˵���Ҷ���
	std::map<PlayerData*, time_t> mNoClientPlayerMap;
	//û����ҵķ������
	std::map<int, time_t> mNoPlayerRoomMap;

	//˽������Ϣ
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