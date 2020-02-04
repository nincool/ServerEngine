///--------------------------------------------------------------------
/// �ļ���:		ProxyServer.h
/// ��  ��:		�����
/// ˵  ��:		
/// ��������:	2019��9��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef PROXY_SERVER_H
#define PROXY_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "ProxyServerState.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"

class CProxyServer : public NFIModule
{
public:
	CProxyServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// ���֤��ַ
	void SetLicense(std::string& value) { m_nLicense = value; }
	std::string& GetLicense() { return m_nLicense; }

	// ���֤��ʼʱ��
	void SetLicStartTime(int64_t value) { m_nLicStartTime = value; }
	int64_t GetLicStartTime() const { return m_nLicStartTime; }

	// ���֤����ʱ��
	void SetLicEndTime(int64_t value) { m_nLicEndTime = value; }
	int64_t GetLicEndTime() const { return m_nLicEndTime; }

	// ���֤��Ч��־
	void SetLicVaild(bool value) { m_nLicVaild = value; }
	bool GetLicVaild() const { return m_nLicVaild; }

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
	void OnReqCloseProxyServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqSynInfo(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �ͻ����������
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// ����Ƿ���ɱ������
	int CheckServerStatus();
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;
	NFINT64 m_nLastCheckTime = 0;

	//���֤��ַ
	std::string m_nLicense = "";
	//���֤��ʼʱ��
	int64_t m_nLicStartTime = 0;
	//���֤����ʱ��
	int64_t m_nLicEndTime = 0;
	//���֤��Ч��־
	bool m_nLicVaild = false;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	CProxyServerState* m_pProxyServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
};

#endif
