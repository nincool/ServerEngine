///---------------------------------------------------------
/// 文件名: LetterRedis.h
/// 内 容:  
/// 说 明:       
/// 创建日期: 2019/08/27
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------
#ifndef _LETTER_REDIS_MODULE_H__
#define _LETTER_REDIS_MODULE_H__

#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFPluginModule/ICommonRedis.h"
#include "NFComm/NFPluginModule/ILetterRedis.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "DBServer.h"
#include "ColdData.h"

class CLetterRedis : public ILetterRedis
{
public:
	CLetterRedis(NFIPluginManager* p)
	{
		pPluginManager = p; 
	}
	virtual bool Init(); 
	virtual bool AfterInit();

	virtual bool SendLetter(const NFGUID& self, const OuterMsg::LetterDataPack& xLetterDataPack, const int nExpireSecond = 0);
	virtual bool LookLetter(const NFGUID& self, const int num, OuterMsg::LetterDataPack& xLetterDataPack, bool bDelete = true);
	virtual bool DeleteLetter(const NFGUID& self, const std::string& strLetterID);
	virtual bool QueryLetter(const NFGUID& self, OuterMsg::AckQueryLetter& xQueryLetter);

protected:
	void OnSendLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnLookLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeleteLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnQueryLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	ICommonRedis* m_pCommonRedis = nullptr;
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CDBServer* m_pDBServer = nullptr;
	CColdData* m_pColdData = nullptr;
};

#endif // _LETTER_REDIS_MODULE_H__