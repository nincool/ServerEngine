///--------------------------------------------------------------------
/// 文件名:		RankDBServer.cpp
/// 内  容:		排行榜数据库
/// 说  明:		
/// 创建日期:	2019年12月10日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "RankDBServer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFUtils/StringUtil.h"
#include "NFComm/NFUtils/CryptoServer/CryptoServer.h"
#include <sys/timeb.h>

bool CRankDBServer::Init()
{
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pMysqlModule = pPluginManager->FindModule<NFMysqlModule>();
	m_pNosqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pColdData = pPluginManager->FindModule<CColdData>();

	m_strMysqlNameColdData = std::string("nf_cold_data_") + to_string(pPluginManager->GetServerID());

	return true;
}

bool CRankDBServer::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CRankDBServer>("CRankDBServer")
		.addProperty("UseEncrypt", &CRankDBServer::GetUseEncrypt, &CRankDBServer::SetUseEncrypt)
		.addProperty("MysqlConnectColdData", &CRankDBServer::GetMysqlConnectColdData, &CRankDBServer::SetMysqlConnectColdData)
		.addProperty("RedisConnect", &CRankDBServer::GetRedisConnect, &CRankDBServer::SetRedisConnect)
		.addProperty("RedisConnect", &CRankDBServer::GetRedisConnect, &CRankDBServer::SetRedisConnect)

		.addFunction("connect_server", &CRankDBServer::ConnectServer)
		.addFunction("create_server", &CRankDBServer::CreateServer)
		
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool CRankDBServer::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnMySqlComplete, METHOD_ARGS(CRankDBServer::OnMySqlComplete));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnMySqlDisConnect, METHOD_ARGS(CRankDBServer::OnMySqlDisConnect));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnNoSqlComplete, METHOD_ARGS(CRankDBServer::OnNoSqlComplete));
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnNoSqlDisConnect, METHOD_ARGS(CRankDBServer::OnNoSqlDisConnect));

	//网络通信测试
	//m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::CS_TEST_NET, this, (NetMsgFun)&CRankDBServer::OnNetTest);

	// 开启数据库连接
	InitDBConnect();
	return true;
}

bool CRankDBServer::InitNetServer()
{
	// 创建服务器网络
	for (int i = 0; i < mxNetList.size(); ++i)
	{
		switch (mxNetList[i].GetNetType())
		{
		case NET_TYPE_INNER:
			m_pNetServerInsideModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_OUTER:
			break;
		case NET_TYPE_MAINTAIN:
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

bool CRankDBServer::InitNetClient()
{
	for (auto& it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	UpReport();

	return true;
}

void CRankDBServer::InitDBConnect()
{
	std::string strDBName = "";
	std::string strDBIP = "";
	int nDBPort = 0;
	std::string strDBUser = "";
	std::string strDBPassword = "";
	// 解析Mysql
	if (ParseDBConnect(m_strMysqlConnectColdData, strDBName, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Mysql
		m_pMysqlModule->AddMysqlServer(strDBIP, nDBPort, m_strMysqlNameColdData, strDBUser, strDBPassword);
	}
	
	// 解析Redis
	if (ParseDBConnect(m_nRedisConnect, strDBName, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Name
		std::string strDBTypeName = strDBName;

		strDBTypeName = strDBName;
		strDBTypeName.append("_Data");
		m_pNosqlModule->AddConnectSql(strDBTypeName, strDBIP, nDBPort, strDBPassword, ECON_TYPE_DATA);
	}
}

bool CRankDBServer::Execute()
{
	return true;
}

int CRankDBServer::OnMySqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bMySqlComplete)
	{
		return 0;
	}

	bMySqlComplete = true;

	return 0;
}

int CRankDBServer::OnMySqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bMySqlComplete)
	{
		// 添加日志记录
		QLOG_ERROR << __FUNCTION__ << " " << args.ToString();
	}

	return 0;
}

int CRankDBServer::OnNoSqlComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bNoSqlComplete)
	{
		return 0;
	}

	bNoSqlComplete = true;

	return 0;
}

int CRankDBServer::OnNoSqlDisConnect(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (bNoSqlComplete)
	{
		// 添加日志记录
		QLOG_ERROR << __FUNCTION__ << " " <<  args.ToString();
	}

	return 0;
}

void CRankDBServer::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void CRankDBServer::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& CRankDBServer::GetTypeIP(ENUM_NET_TYPE type)
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

int CRankDBServer::GetTypePort(ENUM_NET_TYPE type)
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

bool CRankDBServer::ParseDBConnect(std::string& DBConnect, std::string& strDBName, std::string& strDBPassword,
	std::string& strDBUser, std::string& strDBIP, int& nDBPort)
{
	if (DBConnect.empty())
	{
		QLOG_WARING << __FUNCTION__ << " DBConnect is empty";
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
			QLOG_WARING << __FUNCTION__ << "DBConnect string format error";
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
				QLOG_WARING << __FUNCTION__ << "m_nMysqlConnect Source format error";
				return false;
			}

			strDBIP = vec_ipinfo[0];
			nDBPort = StringUtil::StringAsInt(vec_ipinfo[1].c_str());
		}
	}

	return true;
}

//更新程序报文
void CRankDBServer::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	xServerInfoReport.set_server_state(OuterMsg::EST_CRASH);
	xServerInfoReport.set_state_info("Opened");
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
void CRankDBServer::OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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