///--------------------------------------------------------------------
/// �ļ���:		NFMysqlModule.h
/// ��  ��:		mysqlģ��
/// ˵  ��:		
/// ��������:	2019��9��24��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_NFMysqlModule_H__
#define __H_NFMysqlModule_H__
#include <map>
#include "NFCMysqlDriver.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIModule.h"

class NFMysqlModule : public NFIModule
{
public:
	NFMysqlModule(NFIPluginManager* p);
	virtual ~NFMysqlModule();

	virtual bool Init();
	virtual bool Shut();
	virtual bool Execute();
	virtual bool AfterInit();

	virtual bool AddMysqlServer(const std::string& strIP,
								const int nPort,
								const std::string& strDBName,
								const std::string& strDBUser,
								const std::string& strDBPwd);
	
	virtual void RemoveMysqlServer(const std::string& strDBName);
	virtual NFCMysqlDriver* GetMysqlDriver(const std::string& strDBName);

private:
	void CheckMysqlConnect();

private:
	NFIClassModule* m_pClassModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;

	NFINT64 mnLastCheckTime;
	bool bMysqlComplete = false;
	NF_SHARE_PTR<NFIClass> pServerClass = nullptr;
	std::map<std::string, NFCMysqlDriver*> mMapMysql;
	std::map<std::string, NFCMysqlDriver*> mMapInvalidMsyql;
};

#endif //__H_NFMysqlModule_H__