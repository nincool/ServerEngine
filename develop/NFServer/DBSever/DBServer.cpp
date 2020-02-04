///--------------------------------------------------------------------
/// 文件名:		DBServer.cpp
/// 内  容:		数据库
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "DBServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFUtils/StringUtil.h"
#include "NFComm/NFUtils/CryptoServer/CryptoServer.h"
#include <sys/timeb.h>

bool CDBServer::Init()
{
	m_pDBServerState = pPluginManager->FindModule<CDBServerState>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pMysqlModule = pPluginManager->FindModule<NFMysqlModule>();
	m_pNosqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	m_strMysqlNameLog = std::string("nf_log_") + to_string(pPluginManager->GetServerID());
	m_strMysqlNameColdData = std::string("nf_cold_data_") + to_string(pPluginManager->GetServerID());

	return true;
}

bool CDBServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CDBServer>("CDBServer")
		.addProperty("UseEncrypt", &CDBServer::GetUseEncrypt, &CDBServer::SetUseEncrypt)
		.addProperty("MysqlConnectLog", &CDBServer::GetMysqlConnectLog, &CDBServer::SetMysqlConnectLog)
		.addProperty("MysqlConnectColdData", &CDBServer::GetMysqlConnectColdData, &CDBServer::SetMysqlConnectColdData)
		.addProperty("AccountConnect", &CDBServer::GetAccountConnect, &CDBServer::SetAccountConnect)
		.addProperty("RedisConnect", &CDBServer::GetRedisConnect, &CDBServer::SetRedisConnect)
		.addProperty("CreateRoleAmount", &CDBServer::GetCreateRoleAmount, &CDBServer::SetCreateRoleAmount)
		.addProperty("LogTableIntervalType", &CDBServer::GetLogTableIntervalType, &CDBServer::SetLogTableIntervalType)

		.addFunction("connect_server", &CDBServer::ConnectServer)
		.addFunction("create_server", &CDBServer::CreateServer)
		.addFunction("AddLogTableCfg", &CDBServer::AddLogTableCfg)
		
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CDBServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnMySqlComplete, METHOD_ARGS(CDBServer::OnMySqlComplete));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnMySqlDisConnect, METHOD_ARGS(CDBServer::OnMySqlDisConnect));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnNoSqlComplete, METHOD_ARGS(CDBServer::OnNoSqlComplete));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnNoSqlDisConnect, METHOD_ARGS(CDBServer::OnNoSqlDisConnect));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnServerStateChange, METHOD_ARGS(CDBServer::OnClientNetState));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_SERVER_COMPLETE_ALLNET, this, (NetMsgFun)&CDBServer::OnServerCompleteAllNet);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_OPEN_ALLSERVER, this, (NetMsgFun)&CDBServer::OnOpenAllServer);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_CLOSE_DBSERVER, this, (NetMsgFun)&CDBServer::OnReqCloseDBServer);

	//网络通信测试
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CDBServer::OnNetTest);

	// 开启数据库连接
	InitDBConnect();

	return true;
}

bool CDBServer::InitNetServer()
{
	// 创建服务器网络
	for (int i = 0; i < mxNetList.size(); ++i)
	{
		switch (mxNetList[i].GetNetType())
		{
		case NET_TYPE_INNER:
			bServerNetComplete = m_pNetServerInsideModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_OUTER:
			break;
		case NET_TYPE_MAINTAIN:
			m_pNetServerMaintainModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_HTTP:
			m_pHttpServerModule->InitServer(mxNetList[i].GetIP().c_str(), mxNetList[i].GetPort());
			break;
		default:
			break;
		}
	}

	return true;
}

bool CDBServer::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	UpReport();

	return true;
}

void CDBServer::InitDBConnect()
{
	std::string strDBName = "";
	std::string strDBIP = "";
	int nDBPort = 0;
	std::string strDBUser = "";
	std::string strDBPassword = "";
	// 解析Mysql
	if (ParseDBConnect(m_strMysqlConnectLog, strDBName, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Mysql
		m_pMysqlModule->AddMysqlServer(strDBIP, nDBPort, m_strMysqlNameLog, strDBUser, strDBPassword);
	}
	if (ParseDBConnect(m_strMysqlConnectColdData, strDBName, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Mysql
		m_pMysqlModule->AddMysqlServer(strDBIP, nDBPort, m_strMysqlNameColdData, strDBUser, strDBPassword);
	}

	// 解析Account
	if (ParseDBConnect(m_strAccountConnect, strDBName, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Account
		m_pNosqlModule->AddConnectSql(strDBName, strDBIP, nDBPort, strDBPassword, ECON_TYPE_ACCOUNT);
	}
	
	// 解析Redis
	if (ParseDBConnect(m_nRedisConnect, strDBName, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Name
		std::string strDBTypeName = strDBName;
		strDBTypeName.append("_Name");
		m_pNosqlModule->AddConnectSql(strDBTypeName, strDBIP, nDBPort, strDBPassword, ECON_TYPE_NAME);

		strDBTypeName = strDBName;
		strDBTypeName.append("_Data");
		m_pNosqlModule->AddConnectSql(strDBTypeName, strDBIP, nDBPort, strDBPassword, ECON_TYPE_DATA);
	}
}

bool CDBServer::Execute()
{
	return true;
}

int CDBServer::OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bClientNetComplete)
	{
		return 0;
	}
	
	int nState = (int)args.Int(4);
	if (nState == (int)ConnectState::NORMAL)
	{
		//判定所有强连接是否都正常
		std::vector<const ConnectOBJ*> mConnectVec;
		m_pNetClientModule->GetConnectAll(mConnectVec);
		bool bComplete = true;
		size_t size = mConnectVec.size();
		for (size_t i = 0; i < size; ++i)
		{
			if (!mConnectVec[i]->ConnectCfg.GetIsWeakLink() &&
				mConnectVec[i]->report.server_type() == 0)
			{
				bComplete = false;
				break;
			}
		}

		if (bComplete)
		{
			bClientNetComplete = true;
			CheckServerStatus();
		}
	}

	return 0;
}

int CDBServer::OnMySqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bMySqlComplete)
	{
		return 0;
	}

	bMySqlComplete = true;
	CheckServerStatus();

	return 0;
}

int CDBServer::OnMySqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bMySqlComplete)
	{
		// 添加日志记录
		QLOG_ERROR << __FUNCTION__ << " OnMySqlDisConnect:" + args.ToString();
	}

	return 0;
}

int CDBServer::OnNoSqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bNoSqlComplete)
	{
		return 0;
	}

	bNoSqlComplete = true;
	CheckServerStatus();

	return 0;
}

int CDBServer::OnNoSqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bNoSqlComplete)
	{
		// 添加日志记录
		QLOG_ERROR << __FUNCTION__ << " OnNoSqlDisConnect:" + args.ToString();
	}

	return 0;
}

int CDBServer::CheckServerStatus()
{
	if (bClientNetComplete && bServerNetComplete && bNoSqlComplete && bMySqlComplete)
	{
		//进入等待开启状态
		m_pDBServerState->DBStateMachine(CDBServerState::DBINPUT_WAITINGOPEN);
		if (!m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_NET_COMPLETE_DBSERVER, std::string()))
		{
			QLOG_ERROR << __FUNCTION__ << " DBServer complete send failed";
		}
	}

	return 0;
}

void CDBServer::OnReqCloseDBServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pDBServerState->DBStateMachine(CDBServerState::DBINPUT_CLOSE, msg, nLen);
}

void CDBServer::OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pDBServerState->DBStateMachine(CDBServerState::DBINPUT_OPEN, msg, nLen);

	//回调启动完成事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllNetComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CDBServer::OnOpenAllServer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// 启动回调事件
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass != nullptr)
	{
		pServerClass->RunEventCallBack(EVENT_OnAllServerComplete, NFGUID(), NFGUID(), NFDataList());
	}
}

void CDBServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CDBServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CDBServer::GetTypeIP(ENUM_NET_TYPE type)
{
	for (auto& it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetIP();
		}
	}

	return NULL_STR;
}

int CDBServer::GetTypePort(ENUM_NET_TYPE type)
{
	for (auto& it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetPort();
		}
	}

	return 0;
}

bool CDBServer::ParseDBConnect(std::string& DBConnect, std::string& strDBName, std::string& strDBPassword,
	std::string& strDBUser, std::string& strDBIP, int& nDBPort)
{
	if (DBConnect.empty())
	{
		return false;
	}

	// 清空传入数据
	strDBName = strDBPassword = strDBIP = strDBUser = "";
	nDBPort = 0;

	//	如果开启了加密
	std::string tempConnect = DBConnect;
	if (m_bUseEncrypt)
	{
		tempConnect = Decrypt(DBConnect.c_str(), crypto_key);
	}

	vector<std::string> vec_Mysql = StringUtil::SplitString(tempConnect, ";");
	for (int i = 0; i < vec_Mysql.size(); ++i)
	{
		std::string value = vec_Mysql[i];
		vector<std::string> vec_value = StringUtil::SplitString(value, "=");
		if (vec_value.size() < 2)
		{
			QLOG_ERROR << __FUNCTION__ << " DBConnect string format error";
			return false;
		}

		if (vec_value[0] == "SqlName")
		{
			strDBName = vec_value[1];
		}
		else if (vec_value[0] == "Password")
		{
			strDBPassword = vec_value[1];
		}
		else if (vec_value[0] == "User")
		{
			strDBUser = vec_value[1];
		}
		else if (vec_value[0] == "Source")
		{
			vector<std::string> vec_ipinfo = StringUtil::SplitString(vec_value[1], ":");
			if (vec_ipinfo.size() < 2)
			{
				QLOG_ERROR << __FUNCTION__ << " m_nMysqlConnect Source format error";
				return false;
			}

			strDBIP = vec_ipinfo[0];
			nDBPort = StringUtil::StringAsInt(vec_ipinfo[1].c_str());
		}
	}

	return true;
}

//更新程序报文
void CDBServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	if (m_pDBServerState->GetStateEnum() == CDBServerState::DBSTATE_OPENED)
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	}
	else
	{
		xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	}
	xServerInfoReport.set_state_info(m_pDBServerState->GetState());
	xServerInfoReport.set_server_type(pPluginManager->GetServerType());
	xServerInfoReport.set_server_ip(GetTypeIP(NET_TYPE_INNER));
	xServerInfoReport.set_server_port(GetTypePort(NET_TYPE_INNER));
	xServerInfoReport.set_maintain_ip(GetTypeIP(NET_TYPE_MAINTAIN));
	xServerInfoReport.set_maintain_port(GetTypePort(NET_TYPE_MAINTAIN));

	std::string strMsg;
	xServerInfoReport.SerializeToString(&strMsg);
	m_pNetClientModule->SetReport(strMsg);
	m_pNetServerInsideModule->SetReport(strMsg);

	if (force)
	{
		m_pNetClientModule->UpReport(true);
		m_pNetServerInsideModule->UpReport(true);
	}
}

//网络通信测试
void CDBServer::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::NetTest xmsg;
	xmsg.ParseFromArray(msg, nLen);

	struct timeb t1;
	ftime(&t1);
	xmsg.mutable_data()->append("DB:");
	xmsg.mutable_data()->append(to_string((t1.time * 1000) + t1.millitm));
	xmsg.mutable_data()->append(";");

	m_pNetClientModule->SendMsgBySock(nSockIndex, OuterMsg::SC_TEST_NET, xmsg);
}

//增加LOG表配置
void CDBServer::AddLogTableCfg(const std::string& sql)
{
	m_LogTableCfg.push_back(sql);
}