///--------------------------------------------------------------------
/// �ļ���:		DBServer.h
/// ��  ��:		���ݿ�
/// ˵  ��:		
/// ��������:	2019��9��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef DB_SERVER_H
#define DB_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "DBServerState.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CDBServer : public NFIModule
{
public:
	CDBServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// ʹ�ü���
	void SetUseEncrypt(bool value) { m_bUseEncrypt = value; }
	bool GetUseEncrypt() { return m_bUseEncrypt; }

	// Mysql����
	void SetMysqlConnectLog(std::string& value) { m_strMysqlConnectLog = value; }
	const std::string& GetMysqlConnectLog() { return m_strMysqlConnectLog; }
	void SetMysqlConnectColdData(std::string& value) { m_strMysqlConnectColdData = value; }
	const std::string& GetMysqlConnectColdData() { return m_strMysqlConnectColdData; }

	// ��ȡ���ݿ�����
	const std::string& GetMysqlNameLog() { return m_strMysqlNameLog; }
	const std::string& GetMysqlNameColdData() { return m_strMysqlNameColdData; }

	//�˺ſ�����
	void SetAccountConnect(std::string& value) { m_strAccountConnect = value; }
	const std::string& GetAccountConnect() { return m_strAccountConnect; }

	// Redis����
	void SetRedisConnect(std::string& value) { m_nRedisConnect = value; }
	const std::string& GetRedisConnect() { return m_nRedisConnect; }

	// ��������
	void CreateServer(const NetData& net_data);
	// ���������Ϣ
	void ConnectServer(const ConnectCfg& connect_data);
	// ��ȡĳ����IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// ��ȡĳ����Port
	int GetTypePort(ENUM_NET_TYPE type);
	// ������ɫ����
	void SetCreateRoleAmount(int value) { m_nCreateRoleAmount = value; }
	int GetCreateRoleAmount() { return m_nCreateRoleAmount; }
	//���³�����
	void UpReport(bool force = false);

	//LOG�ֱ�����1Ϊ�죬2ΪΪ�ܣ�
	void SetLogTableIntervalType(int value) { mLogTableIntervalType = value; }
	int GetLogTableIntervalType() { return mLogTableIntervalType; }

	//����LOG������
	void AddLogTableCfg(const std::string& sql);
	std::vector<std::string>& GetLogTableCfg() { return m_LogTableCfg; }

private:
	void OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqCloseDBServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �ͻ����������
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// NoSql�������
	int OnNoSqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// NoSql�Ͽ�
	int OnNoSqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql�������
	int OnMySqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql�Ͽ�
	int OnMySqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// ����Ƿ���ɱ������
	int CheckServerStatus();
	// �������ݿ�����
	void InitDBConnect();
	// �������ݿ�����
	bool ParseDBConnect(std::string& DBConnect, std::string& strDBName, std::string& strDBPassword,
		std::string& strDBUser, std::string& strDBIP, int& nDBPort);

	//����ͨ�Ų���
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;
	bool bNoSqlComplete = false;
	bool bMySqlComplete = false;

	// �Ƿ����
	bool m_bUseEncrypt = false;

	//Mysql����
	std::string m_strMysqlNameLog = "";
	std::string m_strMysqlConnectLog = "";
	//Mysql�����ݴ洢��
	std::string m_strMysqlNameColdData = "";
	std::string m_strMysqlConnectColdData = "";
	//�˺ſ�����
	std::string m_strAccountConnect = "";
	//���ֿ�����
	std::string m_nRedisConnect = "";
	//�˺ſ��Դ�����ɫ����
	int m_nCreateRoleAmount = 1;

	//LOG�ֱ�����1Ϊ�죬2ΪΪ�ܣ�
	int mLogTableIntervalType = 2;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	//���ݿ�LOG����
	std::vector<std::string> m_LogTableCfg;

	CDBServerState* m_pDBServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
	NFINoSqlModule* m_pNosqlModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
