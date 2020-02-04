///--------------------------------------------------------------------
/// �ļ���:		ProxyClientLogicModule.h
/// ��  ��:		Proxy��Ϊ�ͻ����߼�ģ��
/// ˵  ��:		
/// ��������:	2019��9��7��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef _PROXY_CLIENTLOGIC_MODULE_H
#define _PROXY_CLIENTLOGIC_MODULE_H

#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CProxyServerLogicModule;
class CProxyClientLogicModule : public NFIModule
{
public:
	CProxyClientLogicModule(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Shut();
	virtual bool Execute();
	virtual bool AfterInit();

	virtual bool VerifyConnectData(const std::string& strAccount, const std::string& strKey);

protected:
	void CheckVerifyTimeOut();
	void OnSelectServerResultProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckEnterGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void Transpond(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	struct ClientConnectData
	{
		ClientConnectData()
		{
			strConnectKey = "";
			mConnectTime = 0;
		}

		std::string strConnectKey;
		NFINT64 mConnectTime;
	};
	map<std::string, ClientConnectData> mWantToConnectMap;

	NFINT64 mLastCheckTime = 0; //��ʱ��֤���ʱ��

	NFIClassModule* m_pClassModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	CProxyServerLogicModule* m_pProxyServerLogicModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
};

#endif