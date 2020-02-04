///--------------------------------------------------------------------
/// 文件名:		NetServer.h
/// 内  容:		基础服务模式网络连接
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetServer_H__
#define __H_NetServer_H__
#include "NetMsgQue.h"
#include "NetObj.h"
#include <mutex>
#include "NetCallback.h"
#include "event2/util.h"
#include <list>

#pragma pack(push, 1)

class NetServer
{
public:
	NetServer(NetCallback* pCB, const size_t maxSendBufferSize, const size_t maxReadBufferSize,
		const int nMaxClient, const bool threads);
	~NetServer();

	bool InitServerNet(const std::string& ip, const int port);

	virtual void CloseNetObj(const NFSOCK nSockIndex);

	virtual bool SendMsg(const int16_t nMsgID, const char* msg, const size_t nLen, const NFSOCK nSockIndex);
	virtual bool SendMsg(const int16_t nMsgID, const char* msg, const size_t nLen, const std::list<NFSOCK>& fdList);
	virtual bool SendMsgToAll(const int16_t nMsgID, const char* msg, const size_t nLen);

	virtual NetObj* GetNetObject(const NFSOCK nSockIndex);

	virtual bool Execute();

	virtual bool Final();

private:
	bool AddNetObject(const NFSOCK nSockIndex, NetObj* pObject);

	bool CloseNetObjAll();

	static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data);
	static void conn_readcb(struct bufferevent* bev, void* user_data);
	static void conn_writecb(struct bufferevent* bev, void* user_data);
	static void conn_eventcb(struct bufferevent* bev, short events, void* user_data);
	static void log_cb(int severity, const char* msg);
	static void event_fatal_cb(int err);

	bool SendMsg(const char* msg, const size_t nLen, const NFSOCK nSockIndex);

	bool ProcessRead(struct bufferevent* bev, void* user_data);
	
private:
	//消息队列
	NetMsgQue mNetMsgQueue;
	std::mutex Mtx;

	int mMaxConnect = 5000;
	int mMaxSendBufferSize = 1024 * 1024;
	int mMaxReadBufferSize = 1024 * 1024;
	bool mThreads = true;

	//每帧最大处理量
	const int mMaxPerExecute = 32;

	std::map<NFSOCK, NetObj*> mmObject;

	struct event_base* mxBase = nullptr;
	struct evconnlistener* listener = nullptr;

	//回调
	NetCallback* mpNetCallback = nullptr;
};

#pragma pack(pop)

#endif //__H_NetServer_H__