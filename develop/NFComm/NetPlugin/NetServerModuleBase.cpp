///--------------------------------------------------------------------
/// 文件名:		NetServerModuleBase.cpp
/// 内  容:		网络服务模式基类
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NetServerModuleBase.h"
#include "NFComm/NFUtils/QLOG.h"

bool NetServerModuleBase::Execute()
{
	if (m_pNet)
	{
		m_pNet->Execute();
	}

	return true;
}

bool NetServerModuleBase::Initialization(const size_t maxSendBufferSize, const size_t maxReadBufferSize,
	const int nMaxClient, const bool threads, const std::string& ip, const int port)
{
	if (NULL != m_pNet)
	{
		QLOG_ERROR << __FUNCTION__ << " m_pNet == NULL";
		return false;
	}

	mIP = ip;
	mPort = port;

	m_pNet = new NetServer(&mCallBackMap, maxSendBufferSize, maxReadBufferSize, nMaxClient, threads);

	return m_pNet->InitServerNet(ip, port);
}

void NetServerModuleBase::AddReceiveCallBack(NFIModule* pBase, NetMsgFun fun)
{
	mCallBackMap.AddReceiveCallBack(pBase, fun);
}

void NetServerModuleBase::AddReceiveCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun)
{
	mCallBackMap.AddReceiveCallBack(nMsgID, pBase, fun);
}

void NetServerModuleBase::AddEventCallBack(NFIModule* pBase, NetEventFun fun)
{
	mCallBackMap.AddEventCallBack(pBase, fun);
}

bool NetServerModuleBase::SendMsg(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return false;
	}

	bool bRet = m_pNet->SendMsg(nMsgID, xData.c_str(), xData.length(), nSockIndex);
	if (!bRet)
	{
		QLOG_WARING << __FUNCTION__ << " send failed name:" << mClassName 
			<< " MsgID:" << nMsgID
			<< " nSockIndex:" << nSockIndex;
	}

	return bRet;
}

bool NetServerModuleBase::SendMsg(const uint16_t nMsgID, const google::protobuf::Message& xData, const NFSOCK nSockIndex)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return false;
	}
	
	std::string strMsg;
	xData.SerializeToString(&strMsg);
	return SendMsg(nMsgID, strMsg, nSockIndex);
}

bool NetServerModuleBase::SendMsgToAll(const uint16_t nMsgID, const std::string& xData)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return false;
	}

	bool bRet = m_pNet->SendMsgToAll(nMsgID, xData.c_str(), xData.length());
	if (!bRet)
	{
		QLOG_WARING << __FUNCTION__ << " SendMsgToAll failed name:" << mClassName 
			<< " MsgID:" << nMsgID;
	}

	return bRet;
}

bool NetServerModuleBase::SendMsgToAll(const uint16_t nMsgID, const google::protobuf::Message& xData)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return false;
	}

	std::string strMsg;
	xData.SerializeToString(&strMsg);
	return SendMsgToAll(nMsgID, strMsg);
}

bool NetServerModuleBase::SendMsgToList(const uint16_t nMsgID, const std::string& xData, const std::list<NFSOCK>& lSockIndex)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return false;
	}

	return m_pNet->SendMsg(nMsgID, xData.c_str(), xData.length(), lSockIndex);
}

NetObj* NetServerModuleBase::GetNetObj(const NFSOCK nSockIndex)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return nullptr;
	}

	return m_pNet->GetNetObject(nSockIndex);
}

void NetServerModuleBase::CloseNetObj(const NFSOCK nSockIndex)
{
	if (nullptr == m_pNet)
	{
		QLOG_WARING << __FUNCTION__ << " m_pNet == NULL name:" << mClassName;
		return;
	}

	m_pNet->CloseNetObj(nSockIndex);
}

const std::string& NetServerModuleBase::GetClassName()
{
	return mClassName;
}

const std::string& NetServerModuleBase::GetIP()
{
	return mIP;
}

const int NetServerModuleBase::GetPort()
{
	return mPort;
}