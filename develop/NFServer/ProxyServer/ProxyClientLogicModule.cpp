///--------------------------------------------------------------------
/// 文件名:		ProxyLogicModule.cpp
/// 内  容:		Proxy逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ProxyClientLogicModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/NFDefine.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "ProxyServerLogicModule.h"
#include "NFComm/NFUtils/NFTool.hpp"

bool CProxyClientLogicModule::Init()
{
	mLastCheckTime = pPluginManager->GetNowTime();

	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pProxyServerLogicModule = pPluginManager->FindModule<CProxyServerLogicModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();

    return true;
}

bool CProxyClientLogicModule::Shut()
{
    return true;
}

bool CProxyClientLogicModule::Execute()
{
	CheckVerifyTimeOut();
    return true;
}

bool CProxyClientLogicModule::AfterInit()
{
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SC_ACK_CONNECT_WORLD, this, (NetMsgFun)&CProxyClientLogicModule::OnSelectServerResultProcess);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, this, (NetMsgFun)&CProxyClientLogicModule::Transpond);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SC_ACK_ENTER_GAME, this, (NetMsgFun)&CProxyClientLogicModule::OnAckEnterGame);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_GAME, this, (NetMsgFun)&CProxyClientLogicModule::Transpond);

    return true;
}

void CProxyClientLogicModule::CheckVerifyTimeOut()
{
	if (mLastCheckTime + 10 > pPluginManager->GetNowTime())
	{
		return;
	}

	mLastCheckTime = pPluginManager->GetNowTime();
	map<std::string, ClientConnectData>::iterator iterFirst = mWantToConnectMap.begin();
	while (iterFirst != mWantToConnectMap.end())
	{
		NFINT64 nTime = iterFirst->second.mConnectTime;
		if (mLastCheckTime >= nTime + 15)
		{
			iterFirst = mWantToConnectMap.erase(iterFirst);
		}
		else
		{
			++iterFirst;
		}
	}
}

void CProxyClientLogicModule::OnSelectServerResultProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckConnectWorldResult xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckConnectWorldResult ParseFromArray failed";
		return;
	}

	map<std::string, ClientConnectData>::iterator iterFind = mWantToConnectMap.find(xMsg.account());
	if (iterFind != mWantToConnectMap.end())
	{
		iterFind->second.strConnectKey = xMsg.world_key();
		iterFind->second.mConnectTime = pPluginManager->GetNowTime();
		return;
	}

	ClientConnectData pConnectData;
	pConnectData.strConnectKey = xMsg.world_key();
	pConnectData.mConnectTime = pPluginManager->GetNowTime();
	mWantToConnectMap.insert(make_pair(xMsg.account(), pConnectData));
}

bool CProxyClientLogicModule::VerifyConnectData(const std::string& strAccount, const std::string& strKey)
{
	map<std::string, ClientConnectData>::iterator iTerFind = mWantToConnectMap.find(strAccount);
	if (iTerFind != mWantToConnectMap.end() && strKey == (iTerFind->second).strConnectKey)
	{
		mWantToConnectMap.erase(strAccount);

		return true;
	}

	return false;
}

void CProxyClientLogicModule::OnAckEnterGame(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID nPlayerID;
	OuterMsg::AckEventResult xData;
	if (!ReceivePB(nMsgID, msg, nLen, xData, nPlayerID))
	{
		QLOG_ERROR << __FUNCTION__ << " OnAckEnterGame ReceivePB failed";
		return;
	}

	const NFGUID& xClient = PBToNF(xData.event_client());
	const NFGUID& xPlayer = PBToNF(xData.event_object());

	m_pProxyServerLogicModule->EnterGameSuccessEvent(xClient, xPlayer);
	m_pProxyServerLogicModule->Transpond(nSockIndex, nMsgID, msg, nLen);
}

void CProxyClientLogicModule::Transpond(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pProxyServerLogicModule->Transpond(nSockIndex, nMsgID, msg, nLen);
}