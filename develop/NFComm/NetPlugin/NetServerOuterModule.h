///--------------------------------------------------------------------
/// 文件名:		NetServerOuterModule.h
/// 内  容:		对外公开用的网络服务模块
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetServerOuterModule_H__
#define __H_NetServerOuterModule_H__
#include "NetServerModuleBase.h"
#include <queue>

class NetServerOuterModule : public NetServerModuleBase
{
public:
	NetServerOuterModule(NFIPluginManager* p);

	virtual bool Init();

	virtual bool InitNet(const size_t maxSendBufferSize, const size_t maxReadBufferSize,
		const int nMaxClient, const std::string& ip, const int port);

	bool Execute();
private:
	void OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);
	void OnNetHeartbeat(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	void SafetyCheck();

private:
	//上次安全检查时间
	int64_t mnVerifyLastTime = 0;
	std::queue<int64_t> mWaitVerifyQueue;
};


#endif //__H_NetServerOuterModule_H__
