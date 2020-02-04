///--------------------------------------------------------------------
/// �ļ���:		Room.h
/// ��  ��:		���췿��
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
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
	//�����¼��
	struct RowData
	{
		NFGUID uid;
		time_t time = 0;
		std::string content;
	};

	Room(int Id, NFIPluginManager* p);
	~Room();

	//��ʼ��
	void Init(const bool recording, const std::string& key);

	int GetId();
	const std::string& GetKey();

	//����״̬
	void SetFreeState(bool bState);
	//����״̬
	bool GetFreeState();

	//�̶����ͷ���
	bool GetmIsRecording();

	//������ҵ�����
	void AddPlayer(PlayerData* pPlayerData);
	//�Ƴ�����һ�����
	void RemovePlayer(PlayerData* pPlayerData);

	//ȡ�÷�����Ҽ���
	const std::set<PlayerData*>& GetPlayerList();
	//ȡ�õ�ǰ��������
	size_t GetPlayerSize();

	//������Ϣ
	void SendMsg(const OuterMsg::ChatMsgRoom& msg);

	//ȡ�������¼
	const std::vector<Room::RowData*>& GetChatRecording();

private:
	//�������¼д�����ݿ�
	void RecordingToDB();

	//�����ݿ�ָ������¼
	void RestoreRecording();

	//���������¼
	void AddRecording(const OuterMsg::GUID& uid, const std::string& content, const time_t time);

private:
	int mId = -1;
	//��ǰ���Ƿ��ڻ���״̬
	bool mFreeState = false;
	//��Ҽ���
	std::set<PlayerData*> mClientSet;
	//�̶����ͷ���
	bool mIsRecording = false;
	//����KEY
	std::string mKey;

	//�����¼
	std::vector<Room::RowData*> m_wait_write_vec;
	std::queue<Room::RowData*> m_RowDataPool;

	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
	ChatServerInit* m_pChatServerInit = nullptr;
	NFCMysqlDriver* m_pNFCMysqlDriver = nullptr;
};

#endif	//__H_Room_H__