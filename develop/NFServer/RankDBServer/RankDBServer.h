///--------------------------------------------------------------------
/// �ļ���:		RankDBServer.h
/// ��  ��:		���а����ݿ�
/// ˵  ��:		
/// ��������:	2019��12��10��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __RANK_DB_SERVER_H__
#define __RANK_DB_SERVER_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "../DBSever/ColdData.h"

class CRankDBServer : public NFIModule
{
public:
	CRankDBServer(NFIPluginManager* p)
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
	void SetMysqlConnectColdData(std::string& value) { m_strMysqlConnectColdData = value; }
	const std::string& GetMysqlConnectColdData() { return m_strMysqlConnectColdData; }

	// ��ȡ���ݿ�����
	const std::string& GetMysqlNameColdData() { return m_strMysqlNameColdData; }

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
	//���³�����
	void UpReport(bool force = false);

	bool IsComplete() { return bNoSqlComplete && bMySqlComplete; }

private:
	// NoSql�������
	int OnNoSqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// NoSql�Ͽ�
	int OnNoSqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql�������
	int OnMySqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql�Ͽ�
	int OnMySqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// �������ݿ�����
	void InitDBConnect();
	// �������ݿ�����
	bool ParseDBConnect(std::string& DBConnect, std::string& strDBName, std::string& strDBPassword,
		std::string& strDBUser, std::string& strDBIP, int& nDBPort);

	//����ͨ�Ų���
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bNoSqlComplete = false;
	bool bMySqlComplete = false;

	// �Ƿ����
	bool m_bUseEncrypt = false;
	// redis���ݿ�
	std::string m_nRedisConnect = "";
	//Mysql����
	std::string m_strMysqlNameColdData = "";
	std::string m_strMysqlConnectColdData = "";

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
	NFINoSqlModule* m_pNosqlModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CColdData* m_pColdData = nullptr;
};

#endif // __RANK_DB_SERVER_H__
