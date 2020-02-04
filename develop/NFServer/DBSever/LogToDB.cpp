///--------------------------------------------------------------------
/// 文件名:		LogToDB.h
/// 内  容:		日志写入数数据
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "LogToDB.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMysqlPlugin/NFCMysqlDriver.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"

LogToDBModule::LogToDBModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

bool LogToDBModule::Init()
{
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNFMysqlModule = pPluginManager->FindModule<NFMysqlModule>();
	m_pDBServer = pPluginManager->FindModule<CDBServer>();

	return true;
}

bool LogToDBModule::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_LOG_TO_DB, this, (NetMsgFun)&LogToDBModule::OnLogToDB);

	m_pNFIMysqlDriver = m_pNFMysqlModule->GetMysqlDriver(m_pDBServer->GetMysqlNameLog());
	if (nullptr == m_pNFIMysqlDriver)
	{
		return false;
	}

	CheckTableCreate();

	return true;
}

void LogToDBModule::OnLogToDB(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	CheckTableCreate();

	OuterMsg::LogToDB xlog;
	if (!xlog.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " LogToDB ParseFromArray failed";
		return;
	}
	
	int cols = xlog.data_list().v_data_size();
	std::string sql = "INSERT INTO " + xlog.rec_name() + mTimeName + " VALUES (";
	for (int i = 0; i < cols; ++i)
	{
		if (i != 0)
		{
			sql.append(",");
		}
		const std::string& sv = xlog.data_list().v_data(i).v_string();
		sql.append( "'" + sv + "'");
	}
	sql.append(");");

	m_pNFIMysqlDriver->Query(sql);
}

//取当前时间星期一的时间
time_t LogToDBModule::GetCurTimeMonday()
{
	struct tm sTime;
	time_t curTime = time(NULL);
	localtime_s(&sTime, &curTime);

	int wday = sTime.tm_wday - 1;
	if (wday < 0)
	{
		wday = 7 - 1;
	}
	sTime.tm_mday -= wday;
	sTime.tm_hour = 0;
	sTime.tm_min = 0;
	sTime.tm_sec = 0;

	return mktime(&sTime);
}

//取今天0点
time_t LogToDBModule::GetCurTimeDay()
{
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;

	return  mktime(tm);
}

void LogToDBModule::CheckTableCreate()
{
	time_t curTiem = 0;
	if (m_pDBServer->GetLogTableIntervalType() == 2)
	{
		curTiem = GetCurTimeMonday();
	}
	else
	{
		curTiem = GetCurTimeDay();
	}
	
	if (mLastTime != curTiem)
	{
		mLastTime = curTiem;

		struct tm sTime;
		localtime_s(&sTime, &mLastTime);

		mTimeName = to_string(sTime.tm_year + 1900) + "_" + to_string(sTime.tm_mon + 1) + "_" + to_string(sTime.tm_mday);
		
		CreateTable();
	}
}

//创建日志表
void LogToDBModule::CreateTable()
{
	char targetString[2048] = {0};
	int nLen = 0;
	size_t size = m_pDBServer->GetLogTableCfg().size();
	for (size_t i = 0; i < size; ++i)
	{
		nLen = snprintf(targetString, sizeof(targetString), m_pDBServer->GetLogTableCfg()[i].c_str(), mTimeName.c_str());
		std::string sql = targetString;
		m_pNFIMysqlDriver->Query(sql);
	}
}