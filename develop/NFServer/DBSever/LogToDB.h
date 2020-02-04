///--------------------------------------------------------------------
/// 文件名:		LogToDB.h
/// 内  容:		日志写入数数据
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "DBServer.h"

class LogToDBModule : public NFIModule
{
public:
	LogToDBModule(NFIPluginManager* p);
	virtual bool Init();
	virtual bool AfterInit();

private:
	//检查是否需要创建日志表
	void CheckTableCreate();
	//创建日志表
	void CreateTable();

	//取当前时间星期一的0点
	time_t GetCurTimeMonday();
	//取今天0点
	time_t GetCurTimeDay();

	void OnLogToDB(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NFMysqlModule* m_pNFMysqlModule = nullptr;
	CDBServer* m_pDBServer = nullptr;
	NFCMysqlDriver* m_pNFIMysqlDriver = nullptr;

	time_t mLastTime = 0;
	std::string mTimeName;
};