///--------------------------------------------------------------------
/// �ļ���:		AccountRedis.h
/// ��  ��:		�˺����ݿ�
/// ˵  ��:		
/// ��������:	2019��8��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _ACCOUNT_REDIS_H
#define _ACCOUNT_REDIS_H

#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/IAccountRedis.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFPluginModule/ICommonRedis.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
class CAccountRedis : public IAccountRedis
{
public:
	CAccountRedis(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool AfterInit();

	virtual bool VerifyAccount(const std::string& strAccount, const std::string& strPwd);
private:
	void OnVerifyAccount(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};


#endif
