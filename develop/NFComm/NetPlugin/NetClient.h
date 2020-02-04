///--------------------------------------------------------------------
/// 文件名:		NetClient.h
/// 内  容:		基础客户模式网络连接
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetClient_H__
#define __H_NetClient_H__

#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NetDefine.h"
#include "NetCallback.h"

#pragma pack(push, 1)

class NetClient
{
public:
	NetClient(NetCallback* pCB, const size_t bufferMax);
	~NetClient();

	NFSOCK Connect(const char* strIP, const unsigned short nPort);
	bool CLose();
	bool Execute();
	bool SendMsg(const int16_t nMsgID, const char* msg, const uint32_t nLen);
	int64_t GetSockIndex();

	void SetLocalServer(const int64_t localServerSock, NetCallback* pLocalServerCallback);
private:

	static void conn_readcb(struct bufferevent* bev, void* user_data);
	bool ProcessRead(struct bufferevent* bev, void* user_data);
	static void conn_writecb(struct bufferevent* bev, void* user_data);
	static void conn_eventcb(struct bufferevent* bev, short events, void* user_data);
	static void log_cb(int severity, const char* msg);
private:
	std::string mstrIP;
	int mnPort = 0;
	bool mbWorking = false;
	size_t mBufferMax = 1024 * 1024;

	static int64_t nSockIndex;
	int64_t nPrSockIndex = -1;
	struct event_base* mBase = nullptr;
	struct bufferevent* mBev = nullptr;

	char* m_pPackData = nullptr;

	//回调
	NetCallback* mpNetCallback;

	//本地服务器直接回调
	NetCallback* mpLocalServerCallback = nullptr;
	int64_t mLocalServerSock = 0;
};

#pragma pack(pop)
#endif //__H_NetClient_H__