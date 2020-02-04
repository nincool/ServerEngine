///--------------------------------------------------------------------
/// �ļ���:		RankRedis.h
/// ��  ��:		���а����ݿ�
/// ˵  ��:		
/// ��������:	2019��9��26��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __RANK_REDIS_MODULE_H__
#define __RANK_REDIS_MODULE_H__

#include "NFServer/DBSever/CommonRedis.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "RankDBServer.h"
#include "../DBSever/ColdData.h"

class RankRedis : public NFIModule
{
public:
	RankRedis(NFIPluginManager* p);
	
	virtual bool Init();
	virtual bool AfterInit();

	// ��ʼ��
	void OnRankInit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �������а�
	void OnRankCreate(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ɾ�����а�
	void OnRankDelete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ���°�����
	void OnRankUpdate(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��������
	void OnRankSetData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ȡ����
	void OnRankGetData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ������ȡ����
	void OnRankGetDataArray(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ȡ��������
	void OnRankGetRankData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ȡ��Χ����
	void OnRankGetRangeData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	ICommonRedis* m_pCommonRedis = nullptr;
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CRankDBServer* m_pDBServer = nullptr;
	CColdData* m_pColdData = nullptr;

	// �Ƿ��Ѿ�������ʼ��������
	int m_nInit = 0;
};

#endif // __RANK_REDIS_MODULE_H__
