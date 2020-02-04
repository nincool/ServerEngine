///--------------------------------------------------------------------
/// 文件名:		NFCMysqlDriver.cpp
/// 内  容:		mysql驱动程序
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NFCMysqlDriver.h"
#include "../NFUtils/QuickLog.h"

NFCMysqlDriver::NFCMysqlDriver(const int nReconnectTime/* = 60*/, const int nReconnectCount /*= -1*/)
{
	mfCheckReconnect = 0.0f;
	mnDBPort = 0;
	m_pMysqlConnect = NULL;
	mnReconnectTime = nReconnectTime;
	mnReconnectCount = nReconnectCount;
}

NFCMysqlDriver::~NFCMysqlDriver()
{
	CloseConnection();
}

bool NFCMysqlDriver::Connect(const std::string& strDBName, const std::string& strDBHost, const int nDBPort, const std::string& strDBUser, const std::string& strDBPwd)
{
	mstrDBName = strDBName;
	mstrDBHost = strDBHost;
	mnDBPort = nDBPort;
	mstrDBUser = strDBUser;
	mstrDBPwd = strDBPwd;

	return Connect();
}

bool NFCMysqlDriver::Query(const std::string& qstr, mysqlpp::StoreQueryResult& queryResult)
{
	mysqlpp::Connection* pConection = GetConnection();
	if (pConection)
	{
		NFMYSQLTRYBEGIN;
		mysqlpp::Query& query = pConection->query(qstr);
		//query.execute();
		queryResult = query.store();
		query.reset();

		NFMYSQLTRYEND(qstr);
		return queryResult;
	}

	return false;
}

bool NFCMysqlDriver::Query(const std::string& qstr, mysqlpp::UseQueryResult& queryResult)
{
	mysqlpp::Connection* pConection = GetConnection();
	if (pConection)
	{
		NFMYSQLTRYBEGIN;
		mysqlpp::Query& query = pConection->query(qstr);
		//query.execute();

		queryResult = query.use();
		query.reset();

		NFMYSQLTRYEND(qstr);
		return queryResult;
	}

	return false;
}

bool NFCMysqlDriver::Query(const std::string& qstr)
{
	mysqlpp::Connection* pConection = GetConnection();
	if (pConection)
	{
		bool bRet = false;

		NFMYSQLTRYBEGIN;
		mysqlpp::Query& query = pConection->query(qstr);
		bRet = query.exec();
		query.reset();

		NFMYSQLTRYEND(qstr);
		return bRet;
	}

	return false;
}

bool NFCMysqlDriver::Query(mysqlpp::Query& query)
{
	bool bRet = false;

	NFMYSQLTRYBEGIN;
	bRet = query.exec();
	query.reset();
	NFMYSQLTRYEND(query);

	return bRet;
}

bool NFCMysqlDriver::Query(mysqlpp::Query& query, mysqlpp::UseQueryResult& ret)
{
	NFMYSQLTRYBEGIN;
	ret = query.use();
	query.reset();
	NFMYSQLTRYEND(query);

	return ret;
}

mysqlpp::Connection* NFCMysqlDriver::GetConnection()
{
	return m_pMysqlConnect;
}

void NFCMysqlDriver::CloseConnection()
{
	delete m_pMysqlConnect;
	m_pMysqlConnect = NULL;
}

bool NFCMysqlDriver::Enable()
{
	return !IsNeedReconnect();
}

bool NFCMysqlDriver::CanReconnect()
{
	mfCheckReconnect += 0.1f;

	//30分钟检查断线重连
	if (mfCheckReconnect < mnReconnectTime)
	{
		return false;
	}

	if (mnReconnectCount == 0)
	{
		return false;
	}

	mfCheckReconnect = 0.0f;

	return true;
}

bool NFCMysqlDriver::Reconnect()
{
	CloseConnection();
	Connect(mstrDBName, mstrDBHost, mnDBPort, mstrDBUser, mstrDBPwd);

	if (mnReconnectCount > 0)
	{
		mnReconnectCount--;
	}

	return true;
}

bool NFCMysqlDriver::IsNeedReconnect()
{
	//没有配置表
	if (mstrDBHost.length() < 1 || mstrDBUser.length() < 1)
	{
		return false;
	}

	if (NULL == m_pMysqlConnect)
	{
		return true;
	}

	if (!m_pMysqlConnect->connected())
	{
		CloseConnection();
		return true;
	}

	/*if (!m_pMysqlConnect->ping())
	{
		CloseConnection();
		return true;
	}*/

	return false;
}

bool NFCMysqlDriver::Connect()
{
	m_pMysqlConnect = new mysqlpp::Connection();
	if (NULL == m_pMysqlConnect)
	{
		return false;
	}

	NFMYSQLTRYBEGIN
	m_pMysqlConnect->set_option(new mysqlpp::MultiStatementsOption(true));
	m_pMysqlConnect->set_option(new mysqlpp::SetCharsetNameOption("utf8mb4"));
	m_pMysqlConnect->set_option(new mysqlpp::ReconnectOption(true));
	m_pMysqlConnect->set_option(new mysqlpp::ConnectTimeoutOption(60));
	if (!m_pMysqlConnect->connect("", mstrDBHost.c_str(), mstrDBUser.c_str(), mstrDBPwd.c_str(), mnDBPort))
	{
		QLOG_SYSTEM_S << "mysql connect failed"
			<< " ip:" << mstrDBHost
			<< " port:" << mnDBPort;
		CloseConnection();
		// 连接失败
		return false;
	}

	QuickLog::GetRef().OpenNextSystemEchoGreen();
	QLOG_SYSTEM_S << "mysql connect succeed ip:" << mstrDBHost
		<< " port:" << mnDBPort
		<< " name:" << mstrDBName;

	// 设置超时时间为24小时
	mysqlpp::Query& query = m_pMysqlConnect->query("set interactive_timeout = 24*3600");
	query.execute();
	query.reset();

	//初始化库
	Query("CREATE DATABASE IF NOT EXISTS " + mstrDBName + ";");
	Query("USE " + mstrDBName + "");

	NFMYSQLTRYEND("Connect faild")
	return true;
}

const std::string& NFCMysqlDriver::GetConnectName()
{
	return mstrDBName;
}

void NFCMysqlDriver::GetAddress(std::string& ip, int& port)
{
	ip = mstrDBHost;
	port = mnDBPort;
}