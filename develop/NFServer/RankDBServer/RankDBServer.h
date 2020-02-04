///--------------------------------------------------------------------
/// 文件名:		RankDBServer.h
/// 内  容:		排行榜数据库
/// 说  明:		
/// 创建日期:	2019年12月10日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
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

	// 使用加密
	void SetUseEncrypt(bool value) { m_bUseEncrypt = value; }
	bool GetUseEncrypt() { return m_bUseEncrypt; }

	// Mysql连接
	void SetMysqlConnectColdData(std::string& value) { m_strMysqlConnectColdData = value; }
	const std::string& GetMysqlConnectColdData() { return m_strMysqlConnectColdData; }

	// 获取数据库名称
	const std::string& GetMysqlNameColdData() { return m_strMysqlNameColdData; }

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
	//更新程序报文
	void UpReport(bool force = false);

	bool IsComplete() { return bNoSqlComplete && bMySqlComplete; }

private:
	// NoSql启动完成
	int OnNoSqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// NoSql断开
	int OnNoSqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql启动完成
	int OnMySqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// MySql断开
	int OnMySqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// 开启数据库连接
	void InitDBConnect();
	// 解析数据库连接
	bool ParseDBConnect(std::string& DBConnect, std::string& strDBName, std::string& strDBPassword,
		std::string& strDBUser, std::string& strDBIP, int& nDBPort);

	//网络通信测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	bool bNoSqlComplete = false;
	bool bMySqlComplete = false;

	// 是否加密
	bool m_bUseEncrypt = false;
	// redis数据库
	std::string m_nRedisConnect = "";
	//Mysql连接
	std::string m_strMysqlNameColdData = "";
	std::string m_strMysqlConnectColdData = "";

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
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
