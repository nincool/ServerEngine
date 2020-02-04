///--------------------------------------------------------------------
/// �ļ���:		LoginServer.h
/// ��  ��:		Login�߼�ģ��
/// ˵  ��:		
/// ��������:	2019��9��7��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _LOGIN_SERVER_H
#define _LOGIN_SERVER_H

#include <map>
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NFISecurityModule.h"

class CLoginServer : public NFIModule
{
public:
	CLoginServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Shut();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();
	virtual bool Execute();

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
	void OnServerInfoProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckConnectWorld(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnVerifyAccount(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// �ͻ����������
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	bool OnWorldView(const NFHttpRequest& req);
	bool OnWorldSelect(const NFHttpRequest& req);
	void OnTimeOut(const NFHttpRequest& req);

	int CheckServerStatus();

	bool OnNetTest(const NFHttpRequest& req);
	void OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bServerNetComplete = false;
	bool bClientNetComplete = false;
	OuterMsg::ServerInfoReportList mServerMap;
	std::map<std::string, int64_t> mWaitHttpRequest;
	std::map<int64_t, std::string> mWaitHttpRequest_byID;

	OuterMsg::AckWorldList m_xWorldList;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIHttpServerModule* m_pHttpNetModule = nullptr;
	NFIClassModule* m_pLogicClassModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NFISecurityModule* m_pSecurityModule = nullptr;
};

#endif