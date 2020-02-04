///--------------------------------------------------------------------
/// 文件名:		NFNoSqlModule.h
/// 内  容:		数据库接口
/// 说  明:		
/// 创建日期:	2019年9月2日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_DATANOSQL_MODULE_H
#define NF_DATANOSQL_MODULE_H

#include "RedisDriver.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/IRedisDriver.h"

class NFNoSqlModule : public NFINoSqlModule
{
public:
	NFNoSqlModule(NFIPluginManager* p);
	virtual ~NFNoSqlModule();

	virtual bool Init();
	virtual bool Shut();
	virtual bool Execute();
	virtual bool AfterInit();

	virtual bool AddConnectSql(const std::string& strID, const std::string& strIP, 
		const int nPort, const std::string& strPass, ECON_TYPE nConnectType);

	virtual void RemoveConnect(ECON_TYPE nConnectType);

	virtual NF_SHARE_PTR<IRedisDriver> GetRedisDriver(ECON_TYPE eType);
protected:
	void CheckConnect();

protected:
	NFINT64 mLastCheckTime = 0;
	bool bNoSqlComplete = false;

	NFIClassModule* m_pClassModule = nullptr;

	map<ECON_TYPE, NF_SHARE_PTR<RedisDriver>> mvTypeSqlDriver;
};

#endif