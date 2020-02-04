///--------------------------------------------------------------------
/// 文件名:		NFIHttpServerModule.h
/// 内  容:		NFIHttpServerModule
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef NFI_HTTP_NET_MODULE_H
#define NFI_HTTP_NET_MODULE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NetPlugin/NFIHttpServer.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"


class NFIHttpServerModule : public NFIModule
{
public:
	virtual ~NFIHttpServerModule() {};

	// register msg callback
	template<typename BaseType>
	bool AddRequestHandler(const std::string& strPath, const NFHttpType eRequestType, BaseType* pBase, bool (BaseType::*handleRecieve)(const NFHttpRequest& req))
	{
		HTTP_RECEIVE_FUNCTOR functor = std::bind(handleRecieve, pBase, std::placeholders::_1);
		HTTP_RECEIVE_FUNCTOR_PTR functorPtr(new HTTP_RECEIVE_FUNCTOR(functor));
		return AddMsgCB(strPath, eRequestType, functorPtr);
	}

	template<typename BaseType>
	bool AddNetFilter(const std::string& strPath, BaseType* pBase, NFWebStatus(BaseType::*handleFilter)(const NFHttpRequest& req))
	{
		HTTP_FILTER_FUNCTOR functor = std::bind(handleFilter, pBase, std::placeholders::_1);
		HTTP_FILTER_FUNCTOR_PTR functorPtr(new HTTP_FILTER_FUNCTOR(functor));

		return AddFilterCB(strPath, functorPtr);
	}

	template<typename BaseType>
	bool AddNetTimeOut(BaseType* pBase, void(BaseType::* handleTimeOut)(const NFHttpRequest& req))
	{
		HTTP_TIMEOUT_FUNCTOR functor = std::bind(handleTimeOut, pBase, std::placeholders::_1);
		return AddTimeOutCB(functor);
	}
public:
	virtual int InitServer(const char* strIP, const unsigned short nPort) = 0;

	virtual bool ResponseMsg(const NFHttpRequest& req, const std::string& strMsg, NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK") = 0;
	virtual bool ResponseMsgByHead(const NFHttpRequest& req, const google::protobuf::Message& xMsg, const int nHead,
		NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK") = 0;
	virtual bool ResponseMsgByHead(const NFHttpRequest& req, const string& strMsg, const int nHead,
		NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK") = 0;

	virtual NFHttpRequest* GetNFHttpRequest(int64_t id) = 0;
private:
	virtual bool AddMsgCB(const std::string& strPath, const NFHttpType eRequestType, const HTTP_RECEIVE_FUNCTOR_PTR& cb) = 0;
	virtual bool AddFilterCB(const std::string& strPath, const HTTP_FILTER_FUNCTOR_PTR& cb) = 0;
	virtual bool AddTimeOutCB(const HTTP_TIMEOUT_FUNCTOR& cb) = 0;
};
#endif
