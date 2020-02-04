///--------------------------------------------------------------------
/// �ļ���:		UpdatePubData.h
/// ��  ��:		���Ժͱ�ͬ��PUB����
/// ˵  ��:		
/// ��������:	2019��9��16��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_UPDATE_PUB_DATA_H__
#define __NF_UPDATE_PUB_DATA_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CUpdatePubData : public NFIModule
{
public:
	CUpdatePubData(NFIPluginManager* p);
    ~CUpdatePubData();

    virtual bool Init();
    virtual bool AfterInit();

private:
	void SynPubDataProperty(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void SynPubDataRecord(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
};

#endif // __NF_UPDATE_PUB_DATA_H__
