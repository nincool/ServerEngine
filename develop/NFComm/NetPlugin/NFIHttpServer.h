///--------------------------------------------------------------------
/// 文件名:		NFIHttpServer.h
/// 内  容:		Http服务接口
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_HTTP_SERVER_H
#define NFI_HTTP_SERVER_H

#include <cstring>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <assert.h>

#include "NFComm/NFPluginModule/NFPlatform.h"

#if NF_PLATFORM == NF_PLATFORM_WIN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#else
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif
#include "event2/http_struct.h"
#include "../NFPluginModule/NFIPluginManager.h"

enum NFWebStatus
{
    WEB_OK = 200,
	WEB_AUTH = 401,
	WEB_ERROR = 404,
	WEB_INTER_ERROR = 500,
    WEB_TIMEOUT = 503,
};

enum NFHttpType
{
	NF_HTTP_REQ_GET = 1 << 0,
	NF_HTTP_REQ_POST = 1 << 1,
	NF_HTTP_REQ_HEAD = 1 << 2,
	NF_HTTP_REQ_PUT = 1 << 3,
	NF_HTTP_REQ_DELETE = 1 << 4,
	NF_HTTP_REQ_OPTIONS = 1 << 5,
	NF_HTTP_REQ_TRACE = 1 << 6,
	NF_HTTP_REQ_CONNECT = 1 << 7,
	NF_HTTP_REQ_PATCH = 1 << 8
};

class NFHttpRequest
{
public:
	NFHttpRequest()
	{
		Reset();
	}

	void Reset()
	{
		id = 0;
		url.clear();
		path.clear();
		remoteHost.clear();
		//type
		body.clear();
		params.clear();
		headers.clear();
		time = 0;
	}

	int64_t id;

	struct evhttp_request* req;
	std::string url;
	std::string path;
	std::string remoteHost;
	NFHttpType type;
	std::string body;//when using post
	std::map<std::string, std::string> params;//when using get
	std::map<std::string, std::string> headers;
	time_t time;
};

//it should be
typedef std::function<bool(const NFHttpRequest& req)> HTTP_RECEIVE_FUNCTOR;
typedef std::shared_ptr<HTTP_RECEIVE_FUNCTOR> HTTP_RECEIVE_FUNCTOR_PTR;

typedef std::function<NFWebStatus(const NFHttpRequest& req)> HTTP_FILTER_FUNCTOR;
typedef std::shared_ptr<HTTP_FILTER_FUNCTOR> HTTP_FILTER_FUNCTOR_PTR;

typedef std::function<void(const NFHttpRequest& req)> HTTP_TIMEOUT_FUNCTOR;

class NFIHttpServer
{
public:
	virtual void SetPluginManager(NFIPluginManager* pPluginManager) = 0;

    virtual bool Execute() = 0;

    virtual int InitServer(const char* strIP, const unsigned short nPort, time_t timeoutMax = 10000) = 0;

    virtual bool ResponseMsg(const NFHttpRequest& req, const std::string& strMsg, NFWebStatus code, const std::string& strReason = "OK") = 0;

	virtual NFHttpRequest* GetNFHttpRequest(int64_t id) = 0;
};

#endif
