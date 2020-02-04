///--------------------------------------------------------------------
/// 文件名:		ChatServerInit.cpp
/// 内  容:		聊天服务器初始化
/// 说  明:		
/// 创建日期:	2019年11月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ChatServerInit.h"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NFUtils/CryptoServer/CryptoServer.h"

ChatServerInit::ChatServerInit(NFIPluginManager* p)
{
	pPluginManager = p;
}

//管理器调用初始化
bool ChatServerInit::Init()
{
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetServerOuterModule = pPluginManager->FindModule<NetServerOuterModule>();
	m_pHttpServerModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pMysqlModule = pPluginManager->FindModule<NFMysqlModule>();

	m_strDBName = "ChatData_" + to_string(pPluginManager->GetServerID());

	return true;
}

// 脚本初始化
bool ChatServerInit::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<ChatServerInit>("ChatNet")
		.addProperty("UseEncrypt", &ChatServerInit::GetUseEncrypt, &ChatServerInit::SetUseEncrypt)
		.addProperty("MysqlConnect", &ChatServerInit::GetMysqlConnect, &ChatServerInit::SetMysqlConnect)
		.addFunction("connect_server", &ChatServerInit::ConnectServer)
		.addFunction("create_server", &ChatServerInit::CreateServer)
		.addProperty("RecordingMax", &ChatServerInit::GetRecordingMax, &ChatServerInit::SetRecordingMax)
		.addProperty("OfflineMsgMax", &ChatServerInit::GetOfflineMsgMax, &ChatServerInit::SetOfflineMsgMax)
		.addProperty("OfflineMsgCacheTimeMax", &ChatServerInit::GetOfflineMsgCacheTimeMax, &ChatServerInit::SetOfflineMsgCacheTimeMax)
		.addProperty("SendFrequencyMax", &ChatServerInit::GetSendFrequencyMax, &ChatServerInit::SetSendFrequencyMax)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

// 服务网络初始化
bool ChatServerInit::InitNetServer()
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
			m_pNetServerOuterModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetReadBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
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

	UpReport();
	return true;
}

// 客户网络初始化
bool ChatServerInit::InitNetClient()
{
	for (auto &it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	InitDBConnect();

	UpReport();

	QuickLog::GetRef().OpenNextSystemEchoGreen();
	QLOG_SYSTEM << "Chat Opened";

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(1);
	auto& second_gauge = gauge_family.Add({ {"State", "Opened"} });
	second_gauge.Increment();
	CATCH_END

	return true;
}

void ChatServerInit::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void ChatServerInit::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

void ChatServerInit::InitDBConnect()
{
	std::string strDBIP = "";
	int nDBPort = 0;
	std::string strDBUser = "";
	std::string strDBPassword = "";
	// 解析Mysql
	if (ParseDBConnect(m_nMysqlConnect, strDBPassword, strDBUser, strDBIP, nDBPort))
	{
		// 连接Mysql
		m_pMysqlModule->AddMysqlServer(strDBIP, nDBPort, m_strDBName, strDBUser, strDBPassword);
	}
}

bool ChatServerInit::ParseDBConnect(std::string& DBConnect, std::string& strDBPassword,
	std::string& strDBUser, std::string& strDBIP, int& nDBPort)
{
	// 清空传入数据
	strDBPassword = strDBIP = strDBUser = "";
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
			QLOG_ERROR << __FUNCTION__ << " m_nMysqlConnect string format error";
			return false;
		}

		if (vec_value[0] == "Password")
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
void ChatServerInit::UpReport(bool force/* = false*/)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	xServerInfoReport.set_server_state(OuterMsg::EST_NARMAL);
	xServerInfoReport.set_server_type(pPluginManager->GetServerType());
	xServerInfoReport.set_state_info("Opened");

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