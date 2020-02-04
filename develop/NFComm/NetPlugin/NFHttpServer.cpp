///--------------------------------------------------------------------
/// 文件名:		NFHttpServer.cpp
/// 内  容:		Http服务网络
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <thread>
#include "NFHttpServer.h"
#include "NFComm/NFUtils/QuickLog.h"
#include "../NFPluginModule/KConstDefine.h"

void NFHttpServer::SetPluginManager(NFIPluginManager* pPluginManager)
{
	m_pPluginManager = pPluginManager;
}

bool NFHttpServer::Execute()
{
	if (mxBase)
	{
		event_base_loop(mxBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}

	OvertimeCheck();

	return true;
}

int NFHttpServer::InitServer(const char* strIP, const unsigned short port, time_t timeoutMax)
{
	mTimeoutMax = timeoutMax;
	mnIndex = 0;

	struct evhttp* http;
	struct evhttp_bound_socket* handle;

#if NF_PLATFORM == NF_PLATFORM_WIN
	WSADATA WSAData;
	WSAStartup(0x101, &WSAData);
#else
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		return (1);
#endif

	mxBase = event_base_new();
	if (!mxBase)
	{
		QLOG_FATAL << __FUNCTION__ << " create event_base fail";
		return 1;
	}

	http = evhttp_new(mxBase);
	if (!http)
	{
		QLOG_FATAL << __FUNCTION__ << " create evhttp fail";
		return 1;
	}

	handle = evhttp_bind_socket_with_handle(http, strIP, port);
	if (!handle)
	{
		QLOG_FATAL << __FUNCTION__ << " bind port: " << port << " failed";
		return 1;
	}

	evhttp_set_gencb(http, listener_cb, (void*)this);

	QuickLog::GetRef().OpenNextSystemEchoGreen();
	QLOG_SYSTEM_S << "Open http net ip:" << strIP
		<< " port:" << port;

	return 0;
}

void NFHttpServer::listener_cb(struct evhttp_request* req, void* arg)
{
	if (req == NULL)
	{
		QLOG_ERROR << __FUNCTION__ << " req == NULL";
		return;
	}

	NFHttpServer* pNet = (NFHttpServer*)arg;
	if (pNet == NULL)
	{
		QLOG_ERROR << __FUNCTION__ << " pNet == NULL";
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	NFHttpRequest* pRequest = pNet->AllowHttpRequest();
	if (pRequest == NULL)
	{
		QLOG_ERROR << __FUNCTION__ << " pRequest == NULL";
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	pRequest->req = req;

	//headers
	struct evkeyvalq* header = evhttp_request_get_input_headers(req);
	if (header == NULL)
	{
		pNet->FreeHttpRequest(*pRequest);

		QLOG_ERROR << __FUNCTION__ << " header == NULL";
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	struct evkeyval* kv = header->tqh_first;
	while (kv)
	{
		pRequest->headers.insert(std::map<std::string, std::string>::value_type(kv->key, kv->value));

		kv = kv->next.tqe_next;
	}

	//uri
	const char* uri = evhttp_request_get_uri(req);
	if (uri == NULL)
	{
		pNet->FreeHttpRequest(*pRequest);

		QLOG_ERROR << __FUNCTION__ << " uri == NULL";
		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		return;
	}

	pRequest->url = uri;
	const char* hostname = evhttp_request_get_host(req);
	if (hostname != NULL)
	{
		//evhttp_find_vhost(http, &http, hostname);
		pRequest->remoteHost = hostname;
	}
	else
	{
		QLOG_ERROR << __FUNCTION__ << " hostname == NULL ";
	}

	pRequest->type = (NFHttpType)evhttp_request_get_command(req);

	//get decodeUri
	struct evhttp_uri* decoded = evhttp_uri_parse(uri);
	if (!decoded)
	{
		pNet->FreeHttpRequest(*pRequest);

		evhttp_send_error(req, HTTP_BADREQUEST, 0);
		QLOG_ERROR << __FUNCTION__ << " bad request";
		return;
	}

	//path
	const char* urlPath = evhttp_uri_get_path(decoded);
	if (urlPath != NULL)
	{
		pRequest->path = urlPath;
	}
	else
	{
		QLOG_ERROR << __FUNCTION__ << " urlPath == NULL";
	}

	evhttp_uri_free(decoded);

	if (evhttp_request_get_command(req) == evhttp_cmd_type::EVHTTP_REQ_GET)
	{
		struct evkeyvalq params;
		evhttp_parse_query(uri, &params);
		struct evkeyval* kv = params.tqh_first;
		while (kv)
		{
			pRequest->params.insert(std::map<std::string, std::string>::value_type(kv->key, kv->value));

			kv = kv->next.tqe_next;
		}
	}

	struct evbuffer* in_evb = evhttp_request_get_input_buffer(req);
	if (in_evb == NULL)
	{
		pNet->FreeHttpRequest(*pRequest);

		QLOG_ERROR << __FUNCTION__ << " urlPath == NULL";
		return;
	}

	size_t len = evbuffer_get_length(in_evb);
	if (len > 0)
	{
		unsigned char* pData = evbuffer_pullup(in_evb, len);
		pRequest->body.clear();

		if (pData != NULL)
		{
			pRequest->body.append((const char*)pData, len);
		}
	}

	if (pNet->mFilter)
	{
		//return 401
		try
		{
			NFWebStatus xWebStatus = pNet->mFilter(*pRequest);
			if (xWebStatus != NFWebStatus::WEB_OK)
			{
				pNet->FreeHttpRequest(*pRequest);

				//401
				pNet->ResponseMsg(*pRequest, "Filter error", xWebStatus);
				return;
			}
		}
		catch (std::exception& e)
		{
			pNet->ResponseMsg(*pRequest, e.what(), NFWebStatus::WEB_ERROR);
		}
		catch (...)
		{
			pNet->ResponseMsg(*pRequest, "UNKNOW ERROR", NFWebStatus::WEB_ERROR);
		}
	}

	// call cb
	try
	{
		if (pNet->mReceiveCB)
		{
			pNet->mReceiveCB(*pRequest);
		}
		else
		{
			pNet->ResponseMsg(*pRequest, "NO PROCESSER", NFWebStatus::WEB_ERROR);
		}
	}
	catch (std::exception& e)
	{
		pNet->ResponseMsg(*pRequest, e.what(), NFWebStatus::WEB_ERROR);
	}
	catch (...)
	{
		pNet->ResponseMsg(*pRequest, "UNKNOW ERROR", NFWebStatus::WEB_ERROR);
	}
}

NFHttpRequest* NFHttpServer::AllowHttpRequest()
{
	if (mxHttpRequestPool.size() <= 0)
	{
		for (int i = 0; i < 100; ++i)
		{
			mxHttpRequestPool.push_back(NF_NEW NFHttpRequest());
		}
	}

	NFHttpRequest* pRequest = mxHttpRequestPool.front();
	mxHttpRequestPool.pop_front();

	pRequest->id = (++mnIndex);
	pRequest->time = NFGetTimeMS();

	mxHttpRequestMap.insert(std::make_pair(pRequest->id, pRequest));
	mOvertimeQueue.push(pRequest);

	// 采集处理中的HTTP数量
	if (m_pPluginManager)
	{
		CATCH_BEGIN
		auto& gauge_family = m_pPluginManager->GetMetricsGauge(2);
		auto& second_gauge = gauge_family.Add({ {"Type", "Http"} });
		second_gauge.Set(mxHttpRequestMap.size());
		CATCH_END
	}

	return pRequest;
}

void NFHttpServer::FreeHttpRequest(const NFHttpRequest& req)
{
	auto it = mxHttpRequestMap.find(req.id);
	if (it != mxHttpRequestMap.end())
	{
		it->second->Reset();
		mxHttpRequestMap.erase(it);
	}
}

bool NFHttpServer::ResponseMsg(const NFHttpRequest& req, const std::string& strMsg,
	NFWebStatus code, const std::string& strReason)
{
	evhttp_request* pHttpReq = (evhttp_request*)req.req;
	if (nullptr == pHttpReq)
	{
		QLOG_ERROR << __FUNCTION__ << " pHttpReq == NULL";
		return false;
	}

	if (code != WEB_OK)
	{
		QLOG_WARING << __FUNCTION__ << " http response:" << strMsg 
			<< " error:" << code 
			<< " reason:" << strReason;
	}
	
	//create buffer
	struct evbuffer* eventBuffer = evbuffer_new();
	if (eventBuffer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " eventBuffer == NULL";
		return false;
	}

	// 采集HTTP的处理耗时
	if (m_pPluginManager)
	{
		CATCH_BEGIN
		time_t nDiff = NFGetTimeMS() - req.time;
		auto& histogram_family = m_pPluginManager->GetMetricsHistogram(3);
		vector<double> vecTemp;
		if (m_pPluginManager->GetHistogramConfig(3, vecTemp))
		{
			auto& second_histogram = histogram_family.Add({ {"Type", "Http"}, {"ID", req.path} }, vecTemp);
			second_histogram.Observe(nDiff);
		}
		CATCH_END
	}

	//send data
	evbuffer_add(eventBuffer, strMsg.c_str(), strMsg.length());

	evhttp_add_header(evhttp_request_get_output_headers(pHttpReq), "Content-Type", "application/octet-stream");

	evhttp_send_reply(pHttpReq, code, strReason.c_str(), eventBuffer);

	//free
	evbuffer_free(eventBuffer);
	FreeHttpRequest(req);

	return true;
}

NFHttpRequest* NFHttpServer::GetNFHttpRequest(int64_t id)
{
	auto it = mxHttpRequestMap.find(id);
	if (it != mxHttpRequestMap.end())
	{
		return it->second;
	}

	return nullptr;
}

void NFHttpServer::OvertimeCheck()
{
	time_t curTime = NFGetTimeMS();

	while (!mOvertimeQueue.empty())
	{
		NFHttpRequest* p = mOvertimeQueue.front();
		if (0 == p->time)
		{
			//已主动回复,删除队列
			mxHttpRequestPool.push_back(p);
			mOvertimeQueue.pop();
			continue;
		}
		else if ((curTime - p->time) > mTimeoutMax)
		{
			if (mTimeOutCB)
			{
				mTimeOutCB(*p);
			}

			// 采集HTTP的超时
			if (m_pPluginManager)
			{
				CATCH_BEGIN
				auto& counter_family = m_pPluginManager->GetMetricsCounter(4);
				auto& second_counter = counter_family.Add({ {"Type", "Http"},{"ID", p->path} });
				second_counter.Increment();
				CATCH_END
			}

			ResponseMsg(*p, "TIMEOUT ERROR", NFWebStatus::WEB_TIMEOUT);

			//删除队列
			mOvertimeQueue.pop();
		}
		else
		{
			break;
		}
	}
}
