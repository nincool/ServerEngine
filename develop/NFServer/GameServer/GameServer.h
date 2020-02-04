///--------------------------------------------------------------------
/// �ļ���:		GameServer.h
/// ��  ��:		��Ϸ��
/// ˵  ��:		
/// ��������:	2019��9��28��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef GAME_SERVER_H
#define GAME_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/ICreateRoleModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "GameServerState.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CGameServer : public NFIModule
{
public:
	CGameServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	virtual void SendMsgPBToGate(const uint16_t nMsgID, const google::protobuf::Message& xMsg, const NFGUID& self);
	virtual void SendMsgToGate(const uint16_t nMsgID, const std::string& strMsg, const NFGUID& self);
	//������Ϣ����ǰ����������ҿͻ���
	virtual bool SendMsgToClientAll(const uint16_t nMsgID, const google::protobuf::Message& xMsg);

	// ��ɫ���ַ�������
	void SetRoleNameMaxSize(int value) { m_nRoleNameMaxSize = value; }
	int GetRoleNameMaxSize() { return m_nRoleNameMaxSize; }

	// ��ɫ���ݱ����¼�
	void SetSaveDataTime(int value) { m_nSaveDataTime = value; }
	int GetSaveDataTime() { return m_nSaveDataTime; }

	// �༭�Ľ�ɫ����ʱ��
	void SetAlterPlayerLifeTime(int value) { m_nAlterPlayerLifeTime = value; }
	int GetAlterPlayerLifeTime() { return m_nAlterPlayerLifeTime; }

	// ��������
	void CreateServer(const NetData& net_data);
	// ���������Ϣ
	void ConnectServer(const ConnectCfg& connect_data);
	// ��ȡĳ����IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// ��ȡĳ����Port
	int GetTypePort(ENUM_NET_TYPE type);

	//���³�����
	void UpReport(bool force = false);
private:
	void OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqCloseGameServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadPubComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadGuildComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadGuidNameComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	void OnCommandToGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �ͻ����������
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// ����Ƿ���ɱ������
	int CheckServerStatus();

	//����ͨ�Ų���
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;

	// ��ɫ����󳤶�
	int m_nRoleNameMaxSize = 10;
	// ��ɫ���ݱ�������ʱ��
	int m_nSaveDataTime = 180000;
	// �༭��ɫ�������¼� ����
	int m_nAlterPlayerLifeTime = 180000;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	CGameServerState* m_pGameServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	ICreateRoleModule* m_pCreateRoleModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
