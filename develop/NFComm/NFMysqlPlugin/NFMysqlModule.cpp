///--------------------------------------------------------------------
/// 文件名:		NFMysqlModule.cpp
/// 内  容:		mysql模块
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NFMysqlModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "../NFUtils/QuickLog.h"

NFMysqlModule::NFMysqlModule(NFIPluginManager* p)
{
	pPluginManager = p;
	mnLastCheckTime = 0;
}

NFMysqlModule::~NFMysqlModule()
{
	auto it = mMapMysql.begin();
	auto it_end = mMapMysql.end();
	for (; it != it_end; ++it)
	{
		delete it->second;
	}

	mMapMysql.clear();
	mMapInvalidMsyql.clear();
}

bool NFMysqlModule::Init()
{
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();

	return true;
}

bool NFMysqlModule::Shut()
{
	return true;
}

bool NFMysqlModule::Execute()
{
	CheckMysqlConnect();

	return true;
}

bool NFMysqlModule::AfterInit()
{
	pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass == nullptr)
	{
		return false;
	}

	return true;
}

bool NFMysqlModule::AddMysqlServer(const std::string& strIP,
	const int nPort,
	const std::string& strDBName,
	const std::string& strDBUser,
	const std::string& strDBPwd)
{
	if (strIP.empty()
		|| strDBName.empty()
		|| strDBUser.empty()
		|| strDBPwd.empty()
		)
	{
		QLOG_WARING << __FUNCTION__ << " para is empty"
			<< " ip:" << strIP
			<< " name:" << strDBName
			<< " user:" << strDBUser
			<< " password:" << strDBPwd;
		return false;
	}

	auto it_mysql = mMapMysql.find(strDBName);
	if (mMapMysql.end() != it_mysql)
	{
		QLOG_WARING << __FUNCTION__ << " not find name:" << strDBName;
		return false;
	}

	auto it_invalid_mysql = mMapInvalidMsyql.find(strDBName);
	if (mMapInvalidMsyql.end() != it_invalid_mysql)
	{
		QLOG_WARING << __FUNCTION__ << " not find name:" << strDBName;
		return false;
	}

	NFCMysqlDriver* pMysqlDriver = NF_NEW NFCMysqlDriver();
	if (pMysqlDriver->Connect(strDBName, strIP, nPort, strDBUser, strDBPwd))
	{
		mMapMysql.insert(std::make_pair(strDBName, pMysqlDriver));
	}
	else
	{
		mMapInvalidMsyql.insert(std::make_pair(strDBName, pMysqlDriver));
	}

	return true;
}

void NFMysqlModule::RemoveMysqlServer(const std::string& strDBName)
{
	auto it_mysql = mMapMysql.find(strDBName);
	if (mMapMysql.end() != it_mysql)
	{
		delete it_mysql->second;
		mMapMysql.erase(it_mysql);
		mMapInvalidMsyql.erase(strDBName);
	}
}

NFCMysqlDriver* NFMysqlModule::GetMysqlDriver(const std::string& strDBName)
{
	auto it_mysql = mMapMysql.find(strDBName);
	if (mMapMysql.end() != it_mysql
		&& it_mysql->second->Enable())
	{
		return it_mysql->second;
	}

	return nullptr;
}

void NFMysqlModule::CheckMysqlConnect()
{
	if (mnLastCheckTime + 10 > GetPluginManager()->GetNowTime())
	{
		return;
	}

	auto it_mysql = mMapMysql.begin();
	while (it_mysql != mMapMysql.end())
	{
		if (!it_mysql->second->Enable())
		{
			mMapInvalidMsyql.insert(std::make_pair(it_mysql->first, it_mysql->second));

			it_mysql = mMapMysql.erase(it_mysql);
			continue;
		}

		++it_mysql;
	}

	if (!bMysqlComplete && mMapInvalidMsyql.empty())
	{
		// mysql连接全
		bMysqlComplete = true;

		pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
		if (pServerClass != nullptr)
		{
			pServerClass->RunEventCallBack(EVENT_OnMySqlComplete, NFGUID(), NFGUID(), NFDataList());
		}
	}

	if (bMysqlComplete && !mMapInvalidMsyql.empty())
	{
		// 有mysql断开
		bMysqlComplete = false;
		NFDataList arg;
		auto it_invalid_mysql = mMapInvalidMsyql.begin();
		while (it_invalid_mysql != mMapInvalidMsyql.end())
		{
			arg << it_invalid_mysql->second->GetConnectName();
			++it_invalid_mysql;
		}

		pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
		if (pServerClass != nullptr)
		{
			pServerClass->RunEventCallBack(EVENT_OnMySqlDisConnect, NFGUID(), NFGUID(), arg);
		}
	}

	auto it_invalid_mysql = mMapInvalidMsyql.begin();
	while (it_invalid_mysql != mMapInvalidMsyql.end())
	{
		if (!it_invalid_mysql->second->Enable() && it_invalid_mysql->second->CanReconnect())
		{
			it_invalid_mysql->second->Reconnect();
			if (it_invalid_mysql->second->Enable())
			{
				mMapMysql.insert(std::make_pair(it_invalid_mysql->first, it_invalid_mysql->second));

				it_invalid_mysql = mMapInvalidMsyql.erase(it_invalid_mysql);
				continue;
			}
		}

		++it_invalid_mysql;
	}
}