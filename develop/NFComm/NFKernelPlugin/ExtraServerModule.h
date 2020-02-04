///--------------------------------------------------------------------
/// �ļ���:		ExtraServerModule.h
/// ��  ��:		���ӷ������߼�
/// ˵  ��:		
/// ��������:	2019��9��19��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_EXTRA_SERVER_MODULE_H__
#define __H_EXTRA_SERVER_MODULE_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CExtraServerModule : public NFIModule
{
public:
	CExtraServerModule(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();

	virtual bool AfterInit();

private:
	//���յ������ת������Ϣ
	void OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInside = nullptr;
};

#endif //__H_EXTRA_SERVER_H__