///--------------------------------------------------------------------
/// 文件名:		NFCommonRedisModule.h
/// 内  容:		通用数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_COMMON_REDIS_MODULE_H
#define NF_COMMON_REDIS_MODULE_H

#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/ICommonRedis.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFPluginModule/NFILogModule.h"

class CDBServer;
class CColdData;
class CCommonRedis : public ICommonRedis
{
public:
	CCommonRedis(NFIPluginManager* p);
	virtual bool Init();
    virtual bool AfterInit();

public:
	virtual const std::string& GetLetterCacheFlag();
	virtual const std::string GetLetterCacheKey(const NFGUID& self);
	virtual const std::string GetAccountCacheKey(const std::string& strAccount, int nServerID);
	virtual const std::string GetPasswordCacheKey(const std::string& strAccount);
	virtual const std::string GetRankCacheKey(const std::string& strName);
	virtual const std::string& GetRankDataCacheFlag();
	virtual const std::string GetRankDataCacheKey(const NFGUID& self);
	virtual const std::string GetRankListCacheKey(int nServerID);
	virtual const std::string GetPlayerListCacheKey(int nServerID);
	virtual const std::string GetGuildListCacheKey(int nServerID);
	virtual const std::string GetPubListCacheKey(int nServerID);
	virtual const std::string GetPlatformListCacheKey(int nServerID);
	virtual const std::string& GetObjectCacheFlag();
	virtual const std::string GetObjectCacheKey(const NFGUID& self);
	virtual const std::string& GetObjectDataCacheKey();
	virtual const std::string GetPlayerNameCacheKey(const std::string& strName);
	virtual const std::string& CCommonRedis::GetUpdateTime();

	virtual NF_SHARE_PTR<NFIPropertyManager> NewPropertyManager(const std::string& strClassName);
    virtual NF_SHARE_PTR<NFIRecordManager> NewRecordManager(const std::string& strClassName);

protected:
	//support hmset
	virtual bool ConvertVectorToPropertyManager(string_pair_vector& values, NF_SHARE_PTR<NFIPropertyManager> pPropertyManager);
	virtual bool ConvertVectorToRecordManager(string_pair_vector& values, NF_SHARE_PTR<NFIRecordManager> pRecordManager);

	//support hmset
	virtual bool ConvertPropertyManagerToVector(NF_SHARE_PTR<NFIPropertyManager> pPropertyManager, std::vector<std::string>& vKeyList, std::vector<std::string>& vValueList);
	virtual bool ConvertRecordManagerToVector(NF_SHARE_PTR<NFIRecordManager> pRecordManager, std::vector<std::string>& vKeyList, std::vector<std::string>& vValueList);

protected:
	NFIClassModule* m_pLogicClassModule = nullptr;
    NFINoSqlModule* m_pNoSqlModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CDBServer* m_pDBServer = nullptr;
	CColdData* m_pColdData = nullptr;
};


#endif
