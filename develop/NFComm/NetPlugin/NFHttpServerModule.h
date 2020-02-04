///--------------------------------------------------------------------
/// 文件名:		NFHttpServerModule.h
/// 内  容:		Http服务模块
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_HTTP_SERVER_MODULE_H
#define NF_HTTP_SERVER_MODULE_H

#include <iostream>
#include "NFComm/NFCore/NFMapEx.hpp"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "NFComm/NetPlugin/NFIHttpServer.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"

class NFHttpServerModule : public NFIHttpServerModule
{
public:
    NFHttpServerModule(NFIPluginManager* p);

    virtual ~NFHttpServerModule();

public:
    virtual int InitServer(const char* strIP, const unsigned short nPort);

    virtual bool Execute();

    virtual bool ResponseMsg(const NFHttpRequest& req, const std::string& strMsg, 
		NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK");
	virtual bool ResponseMsgByHead(const NFHttpRequest& req, const google::protobuf::Message& xMsg, const int nHead,
		NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK");
	virtual bool ResponseMsgByHead(const NFHttpRequest& req, const std::string& strMsg, const int nHead,
		NFWebStatus code = NFWebStatus::WEB_OK, const std::string& reason = "OK");

	virtual NFHttpRequest* GetNFHttpRequest(int64_t id);

private:
	virtual bool OnReceiveNetPack(const NFHttpRequest& req);
	virtual NFWebStatus OnFilterPack(const NFHttpRequest& req);
	virtual void OnTimeOutPack(const NFHttpRequest& req);

	virtual bool AddMsgCB(const std::string& strCommand, const NFHttpType eRequestType, const HTTP_RECEIVE_FUNCTOR_PTR& cb);
	virtual bool AddFilterCB(const std::string& strCommand, const HTTP_FILTER_FUNCTOR_PTR& cb);
	virtual bool AddTimeOutCB(const HTTP_TIMEOUT_FUNCTOR& cb);

private:
	NFIHttpServer* m_pHttpServer = nullptr;

	NFMapEx<NFHttpType, std::map<std::string, HTTP_RECEIVE_FUNCTOR_PTR>> mMsgCBMap;
	std::map<std::string, HTTP_FILTER_FUNCTOR_PTR> mMsgFliterMap;
	std::vector<HTTP_TIMEOUT_FUNCTOR> mMsgTimeOutVec;
};

#endif
