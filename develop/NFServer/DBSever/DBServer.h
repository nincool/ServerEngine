///--------------------------------------------------------------------
/// 文件名:		DBServer.h
/// 内  容:		数据库
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
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

	// 使用加密
	void SetUseEncrypt(bool value) { m_bUseEncrypt = value; }
	bool GetUseEncrypt() { return m_bUseEncrypt; }

	// Mysql连接
	void SetMysqlConnectLog(std::string& value) { m_strMysqlConnectLog = value; }
	const std::string& GetMysqlConnectLog() { return m_strMysqlConnectLog; }
	void SetMysqlConnectColdData(std::string& value) { m_strMysqlConnectColdData = value; }
	const std::string& GetMysqlConnectColdData() { return m_strMysqlConnectColdData; }

	// 获取数据库名称
	const std::string& GetMysqlNameLog() { return m_strMysqlNameLog; }
	const std::string& GetMysqlNameColdData() { return m_strMysqlNameColdData; }

	//账号库连接
	void SetAccountConnect(std::string& value) { m_strAccountConnect = value; }
	const std::string& GetAccountConnect() { return m_strAccountConnect; }

	// Redis连接
	void SetRedisConnect(std::string& value) { m_nRedisConnect = value; }
	const std::string& GetRedisConnect() { return m_nRedisConnect; }

	// 创建服务
	void CreateServer(const NetData& net_data);
	// 添加链接信息
	void ConnectServer(const ConnectCfg& connect_data);
	// 获取某类型IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// 获取某类型Port
	int GetTypePort(ENUM_NET_TYPE type);
	// 创建角色数量
	void SetCreateRoleAmount(int value) { m_nCreateRoleAmount = value; }
	int GetCreateRoleAmount() { return m_nCreateRoleAmount; }
	//更新程序报文
	void UpReport(bool force = false);

	//LOG分表间隔（1为天，2为为周）
	void SetLogTableIntervalType(int value) { mLogTableIntervalType = value; }
	int GetLogTableIntervalType() { return mLogTableIntervalType; }

	//增加LOG表配置
	void AddLogTableCfg(const std::string& sql);
	std::vector<std::string>& GetLogTableCfg() { return m_LogTableCfg; }

private:
	void OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqCloseDBServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// 客户端网络完成
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// NoSql启动完成
	int OnNoSqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// NoSql断开
	int OnNoSqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql启动完成
	int OnMySqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql断开
	int OnMySqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// 检查是否完成本身服务
	int CheckServerStatus();
	// 开启数据库连接
	void InitDBConnect();
	// 解析数据库连接
	bool ParseDBConnect(std::string& DBConnect, std::string& strDBName, std::string& strDBPassword,
		std::string& strDBUser, std::string& strDBIP, int& nDBPort);

	//网络通信测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;
	bool bNoSqlComplete = false;
	bool bMySqlComplete = false;

	// 是否加密
	bool m_bUseEncrypt = false;

	//Mysql连接
	std::string m_strMysqlNameLog = "";
	std::string m_strMysqlConnectLog = "";
	//Mysql冷数据存储库
	std::string m_strMysqlNameColdData = "";
	std::string m_strMysqlConnectColdData = "";
	//账号库连接
	std::string m_strAccountConnect = "";
	//名字库连接
	std::string m_nRedisConnect = "";
	//账号可以创建角色数量
	int m_nCreateRoleAmount = 1;

	//LOG分表间隔（1为天，2为为周）
	int mLogTableIntervalType = 2;

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

	//数据库LOG配置
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
