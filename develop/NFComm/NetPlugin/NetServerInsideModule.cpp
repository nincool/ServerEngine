///--------------------------------------------------------------------
/// 文件名:		NetServerInsideModule.cpp
/// 内  容:		内部进程之间用的网络服务模块
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NetServerInsideModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/QLOG.h"
#include "../NFPluginModule/ConnectData.h"
#include <sys/timeb.h>
#include "NFComm/NFKernelPlugin/PerformanceCountModule.h"
#include "../NFUtils/QuickLog.h"
#include "LocalNetServer.h"

NetServerInsideModule::NetServerInsideModule(NFIPluginManager* p)
{
	pPluginManager = p;
	mClassName = "NetServerInsideModule";
}

bool NetServerInsideModule::ReadyExecute()
{
	struct timeb curTime_s;
	ftime(&curTime_s);
	mlastTime = (curTime_s.time * 1000) + curTime_s.millitm;

	return true;
}

bool NetServerInsideModule::Execute()
{
	struct timeb curTime_s;
	ftime(&curTime_s);
	int64_t curTime = (curTime_s.time * 1000) + curTime_s.millitm;
	int64_t difference = curTime - mlastTime;
	if (difference > 300)
	{
		QLOG_WARING_S << "consumption per frame " << difference << "ms.";
	}

	mlastTime = curTime;

	UpReport();
	return NetServerModuleBase::Execute();
}

bool NetServerInsideModule::Init()
{
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

	mCallBackMap.OpenLog(pPluginManager->FindModule<PerformanceCountModule>(), pPluginManager);

	mCallBackMap.AddEventCallBack(this, (NetEventFun)&NetServerInsideModule::OnSocketEvent);
	mCallBackMap.AddReceiveCallBack(OuterMsg::SS_SERVER_REFRESH, this, (NetMsgFun)&NetServerInsideModule::OnReport);

	return true;
}

bool NetServerInsideModule::InitNet(const size_t bufferMax,
	const int nMaxClient, const std::string& ip, const int port)
{
	LocalNetServer::AddLocalServer(ip, port);

	return NetServerModuleBase::Initialization(bufferMax, bufferMax, nMaxClient, false, ip, port);
}

void NetServerInsideModule::OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent)
{
	if ((int)eEvent & (int)NET_EVENT::NET_EVENT_CONNECTED)
	{
		UpReport(true);
		auto it_sock = mServerMapSock.find(nSockIndex);
		if (it_sock != mServerMapSock.end())
		{
			//LOG
			auto* pNetObj = GetNetObj(nSockIndex);
			if (pNetObj != nullptr)
			{
				QuickLog::GetRef().OpenNextSystemEchoGreen();
				QLOG_SYSTEM_S << "accept client "
					<< ",ip:" << pNetObj->GetIP()
					<< ",port:" << pNetObj->GetPort();
			}
		}
	}
	else
	{
		auto it_sock = mServerMapSock.find(nSockIndex);
		if (it_sock != mServerMapSock.end())
		{
			//LOG
			auto* pNetObj = GetNetObj(nSockIndex);
			if (pNetObj != nullptr)
			{
				QLOG_SYSTEM_S << "accept client off "
					<< ",name:" << it_sock->second->report.server_name()
					<< ",ip:" << pNetObj->GetIP()
					<< ",port:" << pNetObj->GetPort()
					<< ",serverid:" << it_sock->second->report.server_id()
					<< ",appid:" << it_sock->second->report.app_id()
					<< ",districtid:" << it_sock->second->report.district_id()
					<< ",servertype:" << it_sock->second->report.server_type()
					<< ",serverstate:" << it_sock->second->report.server_state();
			}

			//改变变事件
			NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
			if (pServerClass != nullptr)
			{
				NFDataList args;
				args << OuterMsg::EST_MAINTEN
					<< it_sock->second->report.server_id()
					<< it_sock->second->report.app_id()
					<< it_sock->second->report.server_type()
					<< (int)ConnectState::DISCONNECT
					<< 0
					<< nSockIndex;
				pServerClass->RunEventCallBack(EVENT_OnServerStateChange, NFGUID(), NFGUID(), args);
			}

			//修改TYPE表里的记录
			auto& vec = mServerMap[it_sock->second->report.server_id()][it_sock->second->report.server_type()];
			auto it_type = vec.begin();
			auto it_type_end = vec.end();
			for (; it_type != it_type_end; ++it_type)
			{
				if ((*it_type)->nSockIndex == nSockIndex)
				{
					if ((*it_type)->report.server_state() != OuterMsg::EST_MAINTEN)
					{
						//删除TYPE表里的记录
						delete (*it_type);
						vec.erase(it_type);
					}
					else
					{
						//设为维护
						(*it_type)->nSockIndex = -1;
						(*it_type)->mReportVersion = 0;
						(*it_type)->report.set_server_state(OuterMsg::EST_MAINTEN);
					}
					break;
				}
			}

			mServerMapSock.erase(it_sock);
		}
	}
}
void NetServerInsideModule::OnReport(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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

	//完全相同不能连接
	if (xMsg.server_id() == pPluginManager->GetServerID() &&
		xMsg.app_id() == pPluginManager->GetAppID() &&
		xMsg.server_type() == pPluginManager->GetServerType())
	{
		QLOG_ERROR << __FUNCTION__ << " server inside repeat! serverName:" << xMsg.server_name()
			<< " serverid:" << xMsg.server_id()
			<< " appid:" << xMsg.app_id()
			<< " servertype" << xMsg.server_type();
		return;
	}

	int isNew = 0;
	auto it_sock = mServerMapSock.find(nSockIndex);
	if (mServerMapSock.end() == it_sock)
	{
		InsideServerData* pServerData = GetServerCfg(xMsg.server_id(), xMsg.app_id(), xMsg.district_id(), xMsg.server_type());
		if (nullptr == pServerData)
		{
			pServerData = new InsideServerData();
			pServerData->nSockIndex = nSockIndex;
			pServerData->report = xMsg;
			
			//新记录
			mServerMap[xMsg.server_id()][xMsg.server_type()].push_back(pServerData);
			isNew = 1;
		}
		else
		{
			if (-1 != pServerData->nSockIndex)
			{
				//重复
				NetObj* pNetObj = GetNetObj(nSockIndex);
				if (nullptr != pNetObj)
				{
					QLOG_ERROR << __FUNCTION__ << " server inside repeat! serverName:" << xMsg.server_name() 
						<< " source:" << pNetObj->GetIP();
				}

				return;
			}
			else
			{
				pServerData->nSockIndex = nSockIndex;
			}
		}
		
		//sock表里的记录
		mServerMapSock.insert(std::make_pair(nSockIndex, pServerData));

		//LOG
		auto* pNetObj = GetNetObj(nSockIndex);
		if (pNetObj != nullptr)
		{
			QLOG_SYSTEM_S << "accept register client "
				<< ",name:" << xMsg.server_name()
				<< ",ip:" << pNetObj->GetIP()
				<< ",port:" << pNetObj->GetPort()
				<< ",serverid:" << xMsg.server_id()
				<< ",appid:" << xMsg.app_id()
				<< ",districtid:" << xMsg.district_id()
				<< ",servertype:" << xMsg.server_type()
				<< ",serverstate:" << xMsg.server_state();
		}
	}
	else
	{
		it_sock->second->report = xMsg;
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
			<< nSockIndex;
		pServerClass->RunEventCallBack(EVENT_OnServerStateChange, NFGUID(), NFGUID(), args);
	}

	if (xMsg.net_callback_sock() != 0)
	{
		auto* pNetObj = GetNetObj(nSockIndex);
		if (pNetObj != nullptr)
		{
			pNetObj->SetLocalClient(xMsg.net_callback_sock(), (NetCallback*)xMsg.net_callback());
			SetReport(mstrReport);
		}
	}
}

// 取得所有服务组的服务器类型列表
bool NetServerInsideModule::GetTypeServerAll(int nType, std::vector<const InsideServerData*>& mapValue)
{
	bool bR = false;
	auto it = mServerMap.begin();
	auto it_end = mServerMap.end();
	for (; it != it_end; ++it)
	{
		auto it_type = it->second.find(nType);
		if (it_type != it->second.end())
		{
			size_t size = it_type->second.size();
			for (int i = 0; i < size; ++i)
			{
				mapValue.push_back(it_type->second[i]);
				bR = true;
			}
		}
	}

	return bR;
}

// 取得指定服务组的服务器类型列表
bool NetServerInsideModule::GetTypeServer(int serverID, int nType, std::vector<const InsideServerData*>& mapValue)
{
	bool bR = false;
	auto it = mServerMap.find(serverID);
	if (mServerMap.end() != it)
	{
		auto it_type = it->second.find(nType);
		if (it_type != it->second.end())
		{
			size_t size = it_type->second.size();
			for (int i = 0; i < size; ++i)
			{
				mapValue.push_back(it_type->second[i]);
				bR = true;
			}
		}
	}

	return bR;
}


// 通过SOCKID 查找某个固定服务器
const InsideServerData* NetServerInsideModule::GetSockIDServer(int nSockIndex)
{
	auto it_sock = mServerMapSock.find(nSockIndex);
	if (mServerMapSock.end() == it_sock)
	{
		QLOG_WARING << __FUNCTION__ << " mServerMapSock not find sockid:" << nSockIndex;
		return nullptr;
	}

	return it_sock->second;
}

// 取得同组指定服务器信息
const InsideServerData* NetServerInsideModule::GetSameGroupServer(int app_id, int type)
{
	return GetServerCfg(pPluginManager->GetServerID(), app_id, pPluginManager->GetDistrictID(), type);
}

// 给类型服务器发送信息
bool NetServerInsideModule::SendMsgToTypeServer(const NF_SERVER_TYPES eType, int nMsgID, std::string& strData)
{
	bool bResult = false;
	auto it = mServerMap.begin();
	auto it_end = mServerMap.end();
	for (; it != it_end; ++it)
	{
		auto it_type = it->second.find(eType);
		if (it->second.end() == it_type)
		{
			continue;
		}

		auto it_vec = it_type->second.begin();
		auto it_vec_end = it_type->second.end();
		for (; it_vec != it_vec_end; ++it_vec)
		{
			if (this->SendMsg(nMsgID, strData, (*it_vec)->nSockIndex))
			{
				bResult = true;
			}
		}
	}

	if (false == bResult)
	{
		//一个都没发送成功
		QLOG_ERROR << "SendMsgToTypeServer fail. msgid:" << nMsgID;
	}

	return bResult;
}
// 给类型服务器发送信息
bool NetServerInsideModule::SendMsgToTypeServer(const NF_SERVER_TYPES eType, int nMsgID, google::protobuf::Message& xData)
{
	std::string strData;
	xData.SerializeToString(&strData);

	return SendMsgToTypeServer(eType, nMsgID, strData);
}

// 查找某个固定服务器
InsideServerData* NetServerInsideModule::GetServerCfg(int server_id, int app_id, int district_id, int type)
{
	if (pPluginManager->GetDistrictID() != district_id)
	{
		return nullptr;
	}

	auto it = mServerMap.find(server_id);
	if (mServerMap.end() == it)
	{
		return nullptr;
	}

	auto it_type = it->second.find(type);
	if (it_type == it->second.end())
	{
		return nullptr;
	}

	size_t size = it_type->second.size();
	for (int i = 0; i < size; ++i)
	{
		InsideServerData* pInsideServerData = it_type->second[i];
		if (nullptr == pInsideServerData)
		{
			return nullptr;
		}

		if (pInsideServerData->report.server_id() == server_id &&
			pInsideServerData->report.app_id() == app_id)
		{
			return pInsideServerData;
		}
	}

	return nullptr;
}

void NetServerInsideModule::SetReport(std::string& report)
{
	mstrReport = report;
	++mReportVersion;
}

void NetServerInsideModule::UpReport(bool force)
{
	if (!force)
	{
		if (mnLastReportTime + 10 > GetPluginManager()->GetNowTime())
		{
			return;
		}
		mnLastReportTime = GetPluginManager()->GetNowTime();
	}

	auto it = mServerMapSock.begin();
	auto it_end = mServerMapSock.end();
	for (; it != it_end; ++it)
	{
		if (-1 == it->second->nSockIndex)
		{
			continue;
		}

		if (it->second->mReportVersion != mReportVersion)
		{
			NetObj* pNetObj = GetNetObj(it->second->nSockIndex);
			if (nullptr != pNetObj)
			{
				if (pNetObj->IsLocalClient())
				{
					OuterMsg::ServerInfoReport xMsg;
					xMsg.ParseFromArray(mstrReport.c_str(), mstrReport.length());
					xMsg.set_net_callback_sock(it->first);
					xMsg.set_net_callback((unsigned __int64)&mCallBackMap);
					xMsg.SerializeToString(&mstrReport);
				}
			}

			if (SendMsg(OuterMsg::SS_SERVER_REFRESH, mstrReport, it->second->nSockIndex))
			{
				it->second->mReportVersion = mReportVersion;
			}

			OuterMsg::ServerInfoReport xMsg;
			xMsg.ParseFromArray(mstrReport.c_str(), mstrReport.length());
			xMsg.set_net_callback_sock(0);
			xMsg.set_net_callback(0);
			xMsg.SerializeToString(&mstrReport);
		}
		else
		{
			SendMsg(OuterMsg::SS_SERVER_REFRESH, "", it->second->nSockIndex);
		}
	}
}