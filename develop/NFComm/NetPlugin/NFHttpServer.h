///--------------------------------------------------------------------
/// 文件名:		NFHttpServer.h
/// 内  容:		Http服务网络
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_HTTP_SERVER_H
#define NF_HTTP_SERVER_H

#include "NFIHttpServer.h"

#if NF_PLATFORM == NF_PLATFORM_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x) & S_IFMT) == S_IFDIR)
#endif
#ifndef LIBEVENT_SRC
#pragma comment( lib, "libevent.lib")
#endif

#else

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <unistd.h>
#include <dirent.h>
#include <atomic>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <event2/bufferevent.h>
#include "event2/bufferevent_struct.h"
#include "event2/event.h"
#include <event2/http.h>
#include <event2/buffer.h>
#include <event2/util.h>
#include <event2/keyvalq_struct.h>
#include <queue>

class NFHttpServer : public NFIHttpServer
{
public:
	NFHttpServer()
	{
		mxBase = nullptr;
		mnIndex = 0;
		mTimeoutMax = 10000;
	}

	template<typename BaseType>
	NFHttpServer(BaseType* pBaseType, bool (BaseType::* handleRecieve)(const NFHttpRequest& req),
		NFWebStatus(BaseType::* handleFilter)(const NFHttpRequest& req),
		void (BaseType::* handleTimeOut)(const NFHttpRequest& req))
	{
		mxBase = NULL;
		mReceiveCB = std::bind(handleRecieve, pBaseType, std::placeholders::_1);
		mFilter = std::bind(handleFilter, pBaseType, std::placeholders::_1);
		mTimeOutCB = std::bind(handleTimeOut, pBaseType, std::placeholders::_1);
	}

	virtual ~NFHttpServer()
	{
		if (mxBase)
		{
			event_base_free(mxBase);
			mxBase = NULL;
		}
	};

	virtual void SetPluginManager(NFIPluginManager* pPluginManager);

	virtual bool Execute();

	virtual int InitServer(const char* strIP, const unsigned short nPort, time_t timeoutMax = 10000);

	virtual bool ResponseMsg(const NFHttpRequest& req, const std::string& strMsg, NFWebStatus code, const std::string& strReason = "OK");

	virtual NFHttpRequest* GetNFHttpRequest(int64_t id);

private:
	static void listener_cb(struct evhttp_request* req, void* arg);

	NFHttpRequest* AllowHttpRequest();
	void FreeHttpRequest(const NFHttpRequest& req);
	void OvertimeCheck();

private:
	NFIPluginManager* m_pPluginManager = nullptr;
	struct event_base* mxBase;
	HTTP_RECEIVE_FUNCTOR mReceiveCB;
	HTTP_FILTER_FUNCTOR mFilter;
	HTTP_TIMEOUT_FUNCTOR mTimeOutCB;

	int64_t mnIndex = 0;
	std::map<int64_t, NFHttpRequest*> mxHttpRequestMap;
	std::list<NFHttpRequest*> mxHttpRequestPool;
	std::queue<NFHttpRequest*> mOvertimeQueue;

	time_t mTimeoutMax;
};

#endif
