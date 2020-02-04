///--------------------------------------------------------------------
/// 文件名:		NFHttpServerModule.cpp
/// 内  容:		Http服务模块
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFHttpServer.h"
#include "NFHttpServerModule.h"
#include "NetMsgHead.h"
#include "../NFUtils/QLOG.h"
#include "../NFUtils/QuickLog.h"

NFHttpServerModule::NFHttpServerModule(NFIPluginManager* p)
{
	pPluginManager = p;
	m_pHttpServer = nullptr;
}

NFHttpServerModule::~NFHttpServerModule()
{
	mMsgCBMap.ClearAll();
	mMsgFliterMap.clear();
	mMsgTimeOutVec.clear();
	if (m_pHttpServer)
	{
		delete m_pHttpServer;
		m_pHttpServer = nullptr;
	}
}

bool NFHttpServerModule::Execute()
{
	if (m_pHttpServer)
	{
		m_pHttpServer->Execute();
	}

	return true;
}

int NFHttpServerModule::InitServer(const char* strIP, const unsigned short nPort)
{

	m_pHttpServer = new NFHttpServer(this, &NFHttpServerModule::OnReceiveNetPack, &NFHttpServerModule::OnFilterPack,
		&NFHttpServerModule::OnTimeOutPack);

	m_pHttpServer->SetPluginManager(pPluginManager);

	return m_pHttpServer->InitServer(strIP, nPort, 20000);
}

bool NFHttpServerModule::OnReceiveNetPack(const NFHttpRequest& req)
{
	NFPerformance performance;
	auto it = mMsgCBMap.GetElement(req.type);
	if (it)
	{
		auto itPath = it->find(req.path);
		if (it->end() != itPath)
		{
			HTTP_RECEIVE_FUNCTOR_PTR& pFunPtr = itPath->second;
			HTTP_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();
			if (pFunc == nullptr)
			{
				QLOG_ERROR << __FUNCTION__ << " pFunc == NULL";
				return false;
			}

			try
			{
				pFunc->operator()(req);
			}
			catch (const std::exception&)
			{
				ResponseMsg(req, "unknown error", NFWebStatus::WEB_INTER_ERROR);
			}

			return true;
		}
	}

	if (performance.CheckTimePoint(10))
	{
		QLOG_WARING_S << "[net module performance]" 
			<< " consume:" << performance.TimeScope()
			<< " path:" << req.path;
	}

	return ResponseMsg(req, "", NFWebStatus::WEB_ERROR);
}

NFWebStatus NFHttpServerModule::OnFilterPack(const NFHttpRequest& req)
{
	auto itPath = mMsgFliterMap.find(req.path);
	if (mMsgFliterMap.end() != itPath)
	{
		HTTP_FILTER_FUNCTOR_PTR& pFunPtr = itPath->second;
		HTTP_FILTER_FUNCTOR* pFunc = pFunPtr.get();
		if (pFunc == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pFunc == NULL";
			return NFWebStatus::WEB_ERROR;
		}

		return pFunc->operator()(req);
	}

	return NFWebStatus::WEB_OK;
}

void NFHttpServerModule::OnTimeOutPack(const NFHttpRequest& req)
{
	for (size_t i = 0; i < mMsgTimeOutVec.size(); ++i)
	{
		mMsgTimeOutVec[i](req);
	}
}

bool NFHttpServerModule::AddMsgCB(const std::string& strCommand, const NFHttpType eRequestType, const HTTP_RECEIVE_FUNCTOR_PTR& cb)
{
	auto it = mMsgCBMap.GetElement(eRequestType);
	if (!it)
	{
		mMsgCBMap.AddElement(eRequestType, NF_SHARE_PTR<std::map<std::string, HTTP_RECEIVE_FUNCTOR_PTR>>(NF_NEW std::map<std::string, HTTP_RECEIVE_FUNCTOR_PTR>()));
	}

	it = mMsgCBMap.GetElement(eRequestType);
	auto itPath = it->find(strCommand);
	if (it->end() == itPath)
	{
		it->insert(std::map<std::string, HTTP_RECEIVE_FUNCTOR_PTR>::value_type(strCommand, cb));

		return true;
	}
	else
	{
		QLOG_WARING << __FUNCTION__ << " RequestType:" << eRequestType
			<< " Already Exist Command:" << strCommand;
	}

	return false;
}

bool NFHttpServerModule::AddFilterCB(const std::string& strCommand, const HTTP_FILTER_FUNCTOR_PTR& cb)
{
	auto it = mMsgFliterMap.find(strCommand);
	if (it == mMsgFliterMap.end())
	{
		mMsgFliterMap.insert(std::map<std::string, HTTP_FILTER_FUNCTOR_PTR>::value_type(strCommand, cb));
	}

	return true;
}

bool NFHttpServerModule::AddTimeOutCB(const HTTP_TIMEOUT_FUNCTOR& cb)
{
	if (nullptr == cb)
	{
		QLOG_WARING << __FUNCTION__ << " cb == NULL";
		return false;
	}

	mMsgTimeOutVec.push_back(cb);

	return true;
}

bool NFHttpServerModule::ResponseMsg(const NFHttpRequest& req, const std::string& strMsg,
	NFWebStatus code, const std::string& strReason)
{
	return m_pHttpServer->ResponseMsg(req, strMsg, code, strReason);
}

bool NFHttpServerModule::ResponseMsgByHead(const NFHttpRequest& req, const google::protobuf::Message& xMsg, const int nHead,
	NFWebStatus code/* = NFWebStatus::WEB_OK*/, const std::string& reason/* = "OK"*/)
{
	std::string strMsg;
	xMsg.SerializeToString(&strMsg);

	return ResponseMsgByHead(req, strMsg, nHead, code, reason);
}

bool NFHttpServerModule::ResponseMsgByHead(const NFHttpRequest& req, const std::string& strMsg, const int nHead,
	NFWebStatus code/* = NFWebStatus::WEB_OK*/, const std::string& reason/* = "OK"*/)
{
	NetMsgHead xHead;
	xHead.SetMsgID(nHead);
	xHead.SetBodyLength(strMsg.length());

	char szHead[NetMsgHead::HEAD_LENGTH] = { 0 };
	xHead.EnCode(szHead);

	std::string strOutMsg;
	strOutMsg.append(szHead, NetMsgHead::HEAD_LENGTH);
	strOutMsg.append(strMsg.c_str(), strMsg.length());

	//错误日志
	if (code != WEB_OK)
	{
		QLOG_ERROR << __FUNCTION__ << " url:" << req.path 
			<< " code:" << code 
			<< " msg:" << strMsg;
	}

	return m_pHttpServer->ResponseMsg(req, strOutMsg, code, reason);
}

NFHttpRequest* NFHttpServerModule::GetNFHttpRequest(int64_t id)
{
	return m_pHttpServer->GetNFHttpRequest(id);
}