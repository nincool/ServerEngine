///--------------------------------------------------------------------
/// 文件名:		PubRedis.h
/// 内  容:		公共数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PUB_REDIS_MODULE_H
#define _PUB_REDIS_MODULE_H

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "CommonRedis.h"
#include "ObjectRedis.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPubRedis : public CObjectRedis
{
public:
	CPubRedis(NFIPluginManager* p) : CObjectRedis(p)
	{
	}

	virtual bool Init();
	virtual bool AfterInit();
	
private:
	bool CreateGuild(OuterMsg::PubOpData& createGuild, int nServerID);
	bool CreatePub(OuterMsg::PubOpData& createPub, int nServerID);
	bool ExistPubName(const std::string& strPubName, int nServerID);
	bool ExistGuildName(const std::string& strGuildName, int nServerID);

	void OnReqiureStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureSavePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureSaveGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureDeleteGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReqiureDeletePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	void StringListToPropertyList(OuterMsg::PropertyList* pPropertyList, string_pair_vector& vValueList);
private:
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
