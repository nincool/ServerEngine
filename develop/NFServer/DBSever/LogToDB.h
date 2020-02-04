///--------------------------------------------------------------------
/// �ļ���:		LogToDB.h
/// ��  ��:		��־д��������
/// ˵  ��:		
/// ��������:	2019��9��24��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
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
	//����Ƿ���Ҫ������־��
	void CheckTableCreate();
	//������־��
	void CreateTable();

	//ȡ��ǰʱ������һ��0��
	time_t GetCurTimeMonday();
	//ȡ����0��
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