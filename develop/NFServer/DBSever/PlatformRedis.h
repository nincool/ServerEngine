///--------------------------------------------------------------------
/// �ļ���:		PlatformRedis.h
/// ��  ��:		ƽ̨�����ݿ�
/// ˵  ��:		
/// ��������:	2019��10��12��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
