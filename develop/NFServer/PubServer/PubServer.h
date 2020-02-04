///--------------------------------------------------------------------
/// �ļ���:		PubServer.h
/// ��  ��:		������
/// ˵  ��:		
/// ��������:	2019��9��29��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef PUB_SERVER_H
#define PUB_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "PubServerState.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CPubServer : public NFIModule
{
public:
	CPubServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

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
	void OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqClosePubServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadPubComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadGuildComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ���������ʼ��
	int OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �ͻ����������
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// ����Ƿ���ɱ������
	int CheckServerStatus();

	//����ͨ�Ų���
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	CPubServerState* m_pPubServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
