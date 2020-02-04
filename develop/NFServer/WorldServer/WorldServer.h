///--------------------------------------------------------------------
/// �ļ���:		WorldServer.h
/// ��  ��:		�����
/// ˵  ��:		
/// ��������:	2019��9��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef WORLD_SERVER_H
#define WORLD_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "WhiteIP.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NotifyData.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFServer/GameServer/MaintainMsgDataDefine.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CWorldServerState;
class CWorldPlayers;
class CWorldServer : public NFIModule
{
public:
	CWorldServer(NFIPluginManager* p)
	{
		pPluginManager = p;
		m_pWhiteIP = NF_NEW CWhiteIP;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();
	virtual bool Shut();
	virtual bool CommandShut();

	// ������Ϸ����
	void SetServerName(std::string& value) { m_nServerName = value; }
	std::string& GetServerName() { return m_nServerName; }

	// ��С�汾��
	void SetClientVersion(int value) { m_nClientVersion = value; }
	int GetClientVersion() const { return m_nClientVersion; }

	// �Ƿ���������
	void SetWhiteIP(bool value) { m_bUseWhiteIP = value; }
	bool GetWhiteIP() const { return m_bUseWhiteIP; }

	//�����������
	void SetPlayersMax(int v) { m_PlayersMax = v; }
	int GetPlayersMax() const { return m_PlayersMax; }

	// ��Ӱ�����
	bool AddWhiteIPList(const char* ip_list);
	// ������а�������ַ��Χ
	bool ClearWhiteIP();
	// �Ƿ��ڰ�������ַ��Χ��
	bool IsWhiteIP(unsigned int ip_addr);

	//ȡ�÷�������Ϣ�б�
	bool GetServerList(const REQGetServerInfoList& req, ACKGetServerInfoList& ack);

	// ��������
	void CreateServer(const NetData& net_data);
	// ���������Ϣ
	void ConnectServer(const ConnectCfg& connect_data);
	// ��ȡĳ����IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// ��ȡĳ����Port
	int GetTypePort(ENUM_NET_TYPE type);
	// ���֪ͨ�ͻ��˷���
	void NotifyClient(const NotifyData& notify_data);

	// ��ȡ֪ͨ�ͻ��˷����б�
	vector<NotifyData>& GetNotifyList() { return mxNotifyList; }

	//���¿ͻ��˱���
	void UpReport();
private:
	// �����������
	void OnDBNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnGameNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnProxyNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPubNetComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ����OPEN
	void OnDBServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnGameServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnProxyServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPubServerOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ����ر�
	void OnProxyServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnGameServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPubServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDBServerClose(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// ����Ƿ���ɱ������
	int CheckServerStatus();

private:
	bool bServerNetComplete = false;

	//������Ϸ����
	std::string m_nServerName = "";

	//��С�汾��
	int m_nClientVersion = 0;
	//�Ƿ���������
	bool m_bUseWhiteIP = false;
	//���������������
	int m_PlayersMax = 3000;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;
	//֪ͨ�ͻ��˵ķ�����Ϣ
	vector<NotifyData> mxNotifyList;

	// ��������Ϣ
	CWhiteIP* m_pWhiteIP;

	NetClientModule* m_pNetClientModule = nullptr;
	CWorldServerState* m_pWorldServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	CWorldPlayers* m_pCWorldPlayers = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
