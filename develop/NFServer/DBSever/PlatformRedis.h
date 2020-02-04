///--------------------------------------------------------------------
/// 文件名:		PlatformRedis.h
/// 内  容:		平台服数据库
/// 说  明:		
/// 创建日期:	2019年10月12日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PLATFORM_REDIS_MODULE_H
#define _PLATFORM_REDIS_MODULE_H

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "CommonRedis.h"
#include "ObjectRedis.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPlatformRedis : public CObjectRedis
{
public:
	CPlatformRedis(NFIPluginManager* p) : CObjectRedis(p)
	{
	}

	virtual bool Init();
	virtual bool AfterInit();
	
private:
	bool CreateBattleHelper(OuterMsg::PubOpData& createHelper, int nServerID);

	void OnReqiureStartLoadPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureCreateBattleHelper(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureSavePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureDeletePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	void StringListToPropertyList(OuterMsg::PropertyList* pPropertyList, string_pair_vector& vValueList);
private:
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
