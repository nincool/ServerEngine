///--------------------------------------------------------------------
/// 文件名:		ProxyClientLogicModule.h
/// 内  容:		Proxy作为客户端逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
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

	NFINT64 mLastCheckTime = 0; //超时验证检测时间

	NFIClassModule* m_pClassModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	CProxyServerLogicModule* m_pProxyServerLogicModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
};

#endif