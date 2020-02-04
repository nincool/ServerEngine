///--------------------------------------------------------------------
/// �ļ���:		ChatNetMsgModule.h
/// ��  ��:		�����������Ϣ����
/// ˵  ��:		
/// ��������:	2019��9��27��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_ChatNetMsgModule_H__
#define __H_ChatNetMsgModule_H__

#include <map>
#include "ChatModule.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "ChatServerInit.h"

class ChatNetMsgModule : public NFIModule
{
public:
	ChatNetMsgModule(NFIPluginManager* p);

	//���������ó�ʼ��
	virtual bool Init();

	//���������ó�ʼ����
	virtual bool AfterInit();

	//����������֡ѭ��
	virtual bool Execute();

private:
	void OnSocketClientEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);

	//���������ڲ���Ϣ
	void OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//�Ƴ�һ�����췿��
	void OnRemoveRoomProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//������ҵ�һ�����췿��
	void OnAddPlayerToRoomProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//�Ƴ����췿��һ����ң�-1Ϊ����˳����з��䣩
	void OnRemoveRoomPlayerProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//���������Ϣ
	void OnUpdataPlayerInfo(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//����һ�����
	void OnBannedChange(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);

	//���¿ͻ�����Ϣ
	void OnUpdataClient(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//�ͻ��˷���Ϣ��ָ������
	void OnClientMsgRoom(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//�ͻ��˷���Ϣ��ָ�����
	void OnClientMsgPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//�ͻ���ȡ�����¼
	void OnGetChatRecording(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	//�ͻ��˶Ͽ�
	void OnDisconnectClient(const NFSOCK nSockIndex, const NET_EVENT eEvent);

	//�߳���ʱ��û�пͻ��˵���Ҷ���
	void CheckPlayer();

	//ɾ����ʱ��û��ҵķ���
	void CheckRoom();

private:
	ChatModule* m_pChatModule = nullptr;
	NFINT64 mLastCheckTime = 0;
	NFINT64 mLastRoomCheckTime = 0;

	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	ChatServerInit* m_pChatServerInit = nullptr;
};

#endif	//__H_ChatNetMsgModule_H__