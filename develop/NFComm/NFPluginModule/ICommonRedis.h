///--------------------------------------------------------------------
/// 文件名:		ICommonRedis.h
/// 内  容:		Redis通用模块
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef I_COMMON_REDIS_H
#define I_COMMON_REDIS_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIRecordManager.h"

class ICommonRedis : public NFIModule
{
public:
	virtual const std::string& GetLetterCacheFlag() = 0;
	virtual const std::string GetLetterCacheKey(const NFGUID& self) = 0;
	virtual const std::string GetAccountCacheKey(const std::string& strAccount, int nServerID) = 0;
	virtual const std::string GetPasswordCacheKey(const std::string& strAccount) = 0;
	virtual const std::string GetRankCacheKey(const std::string& strName) = 0;
	virtual const std::string& GetRankDataCacheFlag() = 0;
	virtual const std::string GetRankDataCacheKey(const NFGUID& self) = 0;
	virtual const std::string GetRankListCacheKey(int nServerID) = 0;
	virtual const std::string GetPlayerListCacheKey(int nServerID) = 0;
	virtual const std::string GetGuildListCacheKey(int nServerID) = 0;
	virtual const std::string GetPubListCacheKey(int nServerID) = 0;
	virtual const std::string GetPlatformListCacheKey(int nServerID) = 0;
	virtual const std::string& GetObjectCacheFlag() = 0;
	virtual const std::string GetObjectCacheKey(const NFGUID& self) = 0;
	virtual const std::string& GetObjectDataCacheKey() = 0;
	virtual const std::string GetPlayerNameCacheKey(const std::string& strName) = 0;
	virtual const std::string& GetUpdateTime() = 0;

	virtual NF_SHARE_PTR<NFIPropertyManager> NewPropertyManager(const std::string& strClassName) = 0;
	virtual NF_SHARE_PTR<NFIRecordManager> NewRecordManager(const std::string& strClassName) = 0;
};

#endif