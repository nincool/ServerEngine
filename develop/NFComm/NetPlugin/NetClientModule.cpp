///--------------------------------------------------------------------
/// 文件名:		NetClientModule.h
/// 内  容:		做为客户端连接服务网服网络
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NetClientModule.h"
#include "../NFMessageDefine/OuterMsgDefine.h"
#include "../NFUtils/QuickLog.h"
#include "LocalNetServer.h"

NetClientModule::NetClientModule(NFIPluginManager* p)
{
	this->pPluginManager = p;
}

bool NetClientModule::Init()
{
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

	return true;
}

bool NetClientModule::AfterInit()
{
	for (int i = 0; i < NF_SERVER_TYPES::NF_ST_MAX; ++i)
	{
		mCallBackMap[i].AddEventCallBack(this, (NetEventFun)&NetClientModule::OnSocketEvent);
		mCallBackMap[i].AddReceiveCallBack(OuterMsg::SS_SERVER_REFRESH, this, (NetMsgFun)&NetClientModule::OnReport);
	}
	
	return true;
}

bool NetClientModule::Execute()
{
	for (int i = 0; i < (int)mConnectVec.size(); ++i)
	{
		if (nullptr == mConnectVec[i]->mpClientNet ||
			mConnectVec[i]->eState == ConnectState::DISCONNECT)
		{
			continue;
		}
		mConnectVec[i]->mpClientNet->Execute();
	}

	//处理等待连接对象
	ProcessConnect();
	//上发报文
	UpReport();

	return true;
}

void NetClientModule::AddServerConnect(ConnectCfg& conCfg)
{
	for (int i = 0; i < mConnectVec.size(); ++i)
	{
		ConnectOBJ* pConnectOBJ = mConnectVec[i];
		if (pConnectOBJ->ConnectCfg.GetIP() == conCfg.GetIP() &&
			pConnectOBJ->ConnectCfg.GetPort() == conCfg.GetPort())
		{
			QLOG_ERROR << __FUNCTION__ << " exist: ip" << conCfg.GetIP()
				<< " port:" << conCfg.GetPort();
			return;
		}
	}

	ConnectOBJ* pConnectData = new ConnectOBJ();
	pConnectData->ConnectCfg = conCfg;

	mConnectVec.push_back(pConnectData);
	mWaitConnectQue.push(pConnectData);
}

void NetClientModule::RemoveServerConnect(ConnectCfg& conCfg)
{
	while (!mWaitConnectQue.empty())
	{
		ConnectOBJ* pConnectData = mWaitConnectQue.front();
		if (pConnectData->ConnectCfg.GetIP() == conCfg.GetIP() &&
			pConnectData->ConnectCfg.GetPort() == conCfg.GetPort())
		{
			mWaitConnectQue.front() = nullptr;
			break;
		}
	}

	for (int i = 0; i < mConnectVec.size(); ++i)
	{
		ConnectOBJ* pConnectOBJ = mConnectVec[i];
		if (pConnectOBJ->ConnectCfg.GetIP() == conCfg.GetIP() &&
			pConnectOBJ->ConnectCfg.GetPort() == conCfg.GetPort())
		{
			if (pConnectOBJ->mpClientNet)
			{
				mServerSockIndexMap.erase(pConnectOBJ->mpClientNet->GetSockIndex());
			}

			if (pConnectOBJ->report.server_type() != 0)
			{
				mServerMap[pConnectOBJ->report.server_type()][pConnectOBJ->report.server_type()].erase(pConnectOBJ->report.app_id());
			}

			if (pConnectOBJ->mpClientNet)
			{
				delete pConnectOBJ->mpClientNet;
			}
			delete pConnectOBJ;
			pConnectOBJ = nullptr;
			mConnectVec.erase(mConnectVec.begin() + i);
			break;
		}
	}
}

void NetClientModule::ProcessConnect()
{
	if (mnLastConnectTime + 3 > GetPluginManager()->GetNowTime())
	{
		return;
	}
	mnLastConnectTime = GetPluginManager()->GetNowTime();

	std::queue<ConnectOBJ*> temp;
	while (!mWaitConnectQue.empty())
	{
		ConnectOBJ* pConnectData = mWaitConnectQue.front();
		mWaitConnectQue.pop();
		if (nullptr == pConnectData)
		{
			continue;
		}

		if (nullptr != pConnectData->mpClientNet)
		{
			delete pConnectData->mpClientNet;
		}
		pConnectData->mpClientNet = new NetClient(&mCallBackMap[pConnectData->ConnectCfg.GetServerType()], pConnectData->ConnectCfg.GetBufferSize());

		//连接
		pConnectData->eState = ConnectState::CONNECTING;
		NFSOCK sockIndex = pConnectData->mpClientNet->Connect(pConnectData->ConnectCfg.GetIP().c_str(), pConnectData->ConnectCfg.GetPort());
		if (-1 == sockIndex)
		{
			pConnectData->eState = ConnectState::DISCONNECT;

			temp.push(pConnectData);

			QLOG_ERROR << __FUNCTION__ << " NetClient Connect Error, ip:" << pConnectData->ConnectCfg.GetIP()
				<< " port:" << pConnectData->ConnectCfg.GetPort() 
				<< " name:" << pConnectData->report.server_name();
		}
		else
		{
			mServerSockIndexMap.insert(std::make_pair(sockIndex, pConnectData));
		}
	}

	//记录下次重连
	if (!temp.empty())
	{
		temp.swap(mWaitConnectQue);
	}
}

ConnectOBJ* NetClientModule::GetConnectData(const int64_t nSockIndex)
{
	auto it = mServerSockIndexMap.find(nSockIndex);
	auto it_end = mServerSockIndexMap.end();
	if (it != it_end)
	{
		return it->second;
	}
	
	return nullptr;
}

ConnectOBJ* NetClientModule::GetConnectData(const int serverID, const int serverType, const int serverAppID)
{
	auto it = mServerMap.find(serverID);
	if (mServerMap.end() == it)
	{
		QLOG_WARING << __FUNCTION__ << " mServerMap not find server id:" << serverID;
		return nullptr;
	}

	auto it_type = it->second.find(serverType);
	if (it->second.end() == it_type)
	{
		QLOG_WARING << __FUNCTION__ << " mServerMap not find server type:" << serverType;
		return nullptr;
	}

	auto it_appid = it_type->second.find(serverAppID);
	if (it_type->second.end() == it_appid)
	{
		QLOG_WARING << __FUNCTION__ << " mServerMap not find app id:" << serverAppID;
		return nullptr;
	}

	return it_appid->second;
}

void NetClientModule::AddReceiveCallBack(const NF_SERVER_TYPES eType, NFIModule* pBase, NetMsgFun fun)
{
	mCallBackMap[eType].AddReceiveCallBack(pBase, fun);
}

void NetClientModule::AddReceiveCallBack(const NF_SERVER_TYPES eType, const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun)
{
	mCallBackMap[eType].AddReceiveCallBack(nMsgID, pBase, fun);
}

void NetClientModule::AddEventCallBack(const NF_SERVER_TYPES eType, NFIModule* pBase, NetEventFun fun)
{
	mCallBackMap[eType].AddEventCallBack(pBase, fun);
}

bool NetClientModule::SendMsgByType(const NF_SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData)
{
	bool re = false;
	auto it = mServerSockIndexMap.begin();
	auto it_end = mServerSockIndexMap.end();
	for (; it != it_end; ++it)
	{
		if (ConnectState::NORMAL != it->second->eState)
		{
			continue;
		}

		if (it->second->report.server_type() != eType)
		{
			continue;
		}

		if (nullptr == it->second->mpClientNet)
		{
			continue;
		}

		if (it->second->mpClientNet->SendMsg(nMsgID, strData.c_str(), strData.length()))
		{
			re = true;
		}
	}

	if (!re)
	{
		QLOG_WARING << "SendMsgByType connectobj find. Type:" 
			<< eType << ",MsgID:" << nMsgID; 
	}

	return re;
}

bool NetClientModule::SendMsgByType(const NF_SERVER_TYPES eType, const uint16_t nMsgID, const google::protobuf::Message& msg)
{
	std::string strData;
	msg.SerializeToString(&strData);

	return SendMsgByType(eType, nMsgID, strData);
}

bool NetClientModule::SendMsg(const int appID, const NF_SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData)
{
	ConnectOBJ* pConnectOBJ = GetConnectData(pPluginManager->GetServerID(), eType, appID);
	if (nullptr == pConnectOBJ)
	{
		QLOG_WARING << "connectobj find.";
		return false;
	}

	if (ConnectState::NORMAL != pConnectOBJ->eState || nullptr == pConnectOBJ->mpClientNet)
	{
		QLOG_WARING << "connectobj state error.";
		return false;
	}

	return pConnectOBJ->mpClientNet->SendMsg(nMsgID, strData.c_str(), strData.length());
}

bool NetClientModule::SendMsg(const int appID, const NF_SERVER_TYPES eType, const uint16_t nMsgID, const google::protobuf::Message& msg)
{
	std::string strData;
	msg.SerializeToString(&strData);

	return SendMsg(appID, eType, nMsgID, strData);
}

bool NetClientModule::SendMsgBySock(const int64_t nSockIndex, const uint16_t nMsgID, const std::string& strData)
{
	ConnectOBJ* pConnectOBJ = GetConnectData(nSockIndex);
	if (nullptr == pConnectOBJ || nullptr == pConnectOBJ->mpClientNet)
	{
		QLOG_WARING << "connectobj find.";
		return false;
	}

	return pConnectOBJ->mpClientNet->SendMsg(nMsgID, strData.c_str(), strData.length());
}

bool NetClientModule::SendMsgBySock(const int64_t nSockIndex, const uint16_t nMsgID, const google::protobuf::Message& msg)
{
	std::string strMsg;
	msg.SerializeToString(&strMsg);

	return SendMsgBySock(nSockIndex, nMsgID, strMsg);
}

void NetClientModule::OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent)
{
	ConnectOBJ* pConnectData = GetConnectData(nSockIndex);
	if (nullptr == pConnectData)
	{
		return;
	}

	if ((int)eEvent & (int)NET_EVENT::NET_EVENT_CONNECTED)
	{
		pConnectData->eState = ConnectState::NORMAL;

		UpReport(true);

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "connected server ip:" << pConnectData->ConnectCfg.GetIP()
			<< " port:" << pConnectData->ConnectCfg.GetPort();
	}
	else
	{
		//改变变事件
		NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
		if (pServerClass != nullptr)
		{
			NFDataList args;
			args << OuterMsg::EST_MAINTEN
				<< pConnectData->report.server_id()
				<< pConnectData->report.app_id()
				<< pConnectData->report.server_type()
				<< (int)ConnectState::DISCONNECT
				<< 0
				<< nSockIndex
				<< 1;
			pServerClass->RunEventCallBack(EVENT_OnServerStateChange, NFGUID(), NFGUID(), args);
		}

		pConnectData->eState = ConnectState::DISCONNECT;
		pConnectData->mReportVersion = 0;
		mServerSockIndexMap.erase(nSockIndex);

		mWaitConnectQue.push(pConnectData);

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "connect off event:" << (int)eEvent
			<< ",ip:" << pConnectData->ConnectCfg.GetIP()
			<< ",port:" << pConnectData->ConnectCfg.GetPort();
	}
}

void NetClientModule::OnReport(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	if (nLen == 0)
	{
		return;
	}

	OuterMsg::ServerInfoReport xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ServerInfoReport ParseFromArray failed";
		return;
	}

	ConnectOBJ* pConnectOBJ = GetConnectData(nSockIndex);
	if (nullptr == pConnectOBJ)
	{
		return;
	}

	//新服注册记录
	int isNew = 0;
	if (pConnectOBJ->report.server_type() == 0)
	{
		isNew = 1;
		pConnectOBJ->report = xMsg;
		mServerMap[xMsg.server_id()][xMsg.server_type()][xMsg.app_id()] = pConnectOBJ;

		QLOG_SYSTEM_S << "new server add ip:" << pConnectOBJ->ConnectCfg.GetIP()
			<< ",port:" << pConnectOBJ->ConnectCfg.GetPort()
			<< ",name:" << pConnectOBJ->report.server_name();
	}
	else
	{
		pConnectOBJ->report = xMsg;
	}

	//改变变事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		NFDataList args;
		args << xMsg.server_state()
			<< xMsg.server_id()
			<< xMsg.app_id()
			<< xMsg.server_type()
			<< (int)ConnectState::NORMAL
			<< isNew
			<< nSockIndex
			<< 1;
		pServerClass->RunEventCallBack(EVENT_OnServerStateChange, NFGUID(), NFGUID(), args);
	} 

	if (xMsg.net_callback_sock() != 0)
	{
		pConnectOBJ->mpClientNet->SetLocalServer(xMsg.net_callback_sock(), (NetCallback*)xMsg.net_callback());
	}
}

const ConnectOBJ* NetClientModule::GetConnectBySock(const int64_t nSockIndex)
{
	auto it = mServerSockIndexMap.find(nSockIndex);
	auto it_end = mServerSockIndexMap.end();
	if (it != it_end)
	{
		return it->second;
	}

	return nullptr;
}

const ConnectOBJ* NetClientModule::GetConnect(const int serverID, const int serverType, const int serverAppID)
{
	return GetConnectData(serverID, serverType, serverAppID);
}

void NetClientModule::GetConnectByType(const NF_SERVER_TYPES eType, std::vector<const ConnectOBJ*>& outVec)
{
	auto it = mServerSockIndexMap.begin();
	auto it_end = mServerSockIndexMap.end();
	for (; it != it_end; ++it)
	{
		if (it->second->report.server_type() != eType)
		{
			continue;
		}

		outVec.push_back(it->second);
	}
}

void NetClientModule::GetConnectAll(std::vector<const ConnectOBJ*>& outVec)
{
	auto it = mConnectVec.begin();
	auto it_end = mConnectVec.end();
	for (; it != it_end; ++it)
	{
		outVec.push_back(*it);
	}
}

void NetClientModule::SetReport(std::string& report)
{
	mstrReport = report;
	++mReportVersion;
}

void NetClientModule::UpReport(bool force)
{
	if (!force)
	{
		if (mnLastReportTime + 10 > GetPluginManager()->GetNowTime())
		{
			return;
		}

		mnLastReportTime = GetPluginManager()->GetNowTime();
	}

	auto it = mServerSockIndexMap.begin();
	auto it_end = mServerSockIndexMap.end();
	for (; it != it_end; ++it)
	{
		if (nullptr == it->second->mpClientNet || it->second->eState != ConnectState::NORMAL)
		{
			continue;
		}

		if (it->second->mReportVersion != mReportVersion)
		{
			if (LocalNetServer::IsLocalServer(it->second->ConnectCfg.GetIP(), it->second->ConnectCfg.GetPort()))
			{
				OuterMsg::ServerInfoReport xMsg;
				xMsg.ParseFromArray(mstrReport.c_str(), mstrReport.length());
				xMsg.set_net_callback_sock(it->first);
				xMsg.set_net_callback((unsigned __int64)&mCallBackMap[it->second->ConnectCfg.GetServerType()]);
				xMsg.SerializeToString(&mstrReport);
			}
			else
			{
				OuterMsg::ServerInfoReport xMsg;
				xMsg.ParseFromArray(mstrReport.c_str(), mstrReport.length());
				xMsg.set_net_callback_sock(0);
				xMsg.set_net_callback(0);
				xMsg.SerializeToString(&mstrReport);
			}

			if (it->second->mpClientNet->SendMsg(OuterMsg::SS_SERVER_REFRESH, mstrReport.c_str(), mstrReport.length()))
			{
				it->second->mReportVersion = mReportVersion;
			}
		}
		else
		{
			it->second->mpClientNet->SendMsg(OuterMsg::SS_SERVER_REFRESH, "", 0);
		}
	}
}