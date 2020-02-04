///--------------------------------------------------------------------
/// 文件名:		NFNoSqlModule.cpp
/// 内  容:		数据库
/// 说  明:		
/// 创建日期:	2019年9月2日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <algorithm>
#include "NFNoSqlModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "../NFUtils/QuickLog.h"

NFNoSqlModule::NFNoSqlModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

NFNoSqlModule::~NFNoSqlModule()
{
	mvTypeSqlDriver.clear();
}

bool NFNoSqlModule::Init()
{
	mLastCheckTime = 0;
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

	return true;
}

bool NFNoSqlModule::Shut()
{
	return true;
}

bool NFNoSqlModule::AfterInit()
{
	return true;
}

bool NFNoSqlModule::Execute()
{
	for (auto& it : mvTypeSqlDriver)
	{
		NF_SHARE_PTR<RedisDriver> xNosqlDriver = it.second;
		if (xNosqlDriver)
		{
			xNosqlDriver->Execute();
		}
	}

	CheckConnect();
	
	return true;
}

//获取数据库节点
NF_SHARE_PTR<IRedisDriver> NFNoSqlModule::GetRedisDriver(ECON_TYPE eType)
{
	map<ECON_TYPE, NF_SHARE_PTR<RedisDriver>>::iterator itFind = mvTypeSqlDriver.find(eType);
	if (itFind == mvTypeSqlDriver.end())
	{
		return nullptr;
	}

	NF_SHARE_PTR<RedisDriver> xDriver = itFind->second;
	if (xDriver && xDriver->Enable())
	{
		return dynamic_pointer_cast<IRedisDriver>(xDriver);
	}

	return nullptr;
}

bool NFNoSqlModule::AddConnectSql(const std::string& strID, const std::string& strIP, const int nPort, 
	const std::string& strPass, ECON_TYPE nConnectType)
{
	if (nConnectType < ECON_TYPE_ACCOUNT || nConnectType >= ECON_TYPE_MAX)
	{
		QLOG_ERROR << __FUNCTION__ << " connect type out range:" << nConnectType;
		return false;
	}

	map<ECON_TYPE, NF_SHARE_PTR<RedisDriver>>::iterator itFind = mvTypeSqlDriver.find(nConnectType);
	if (itFind != mvTypeSqlDriver.end())
	{
		QLOG_WARING << __FUNCTION__ << " connect type already exist:" << nConnectType;
		return false;
	}

	NF_SHARE_PTR<RedisDriver> pNoSqlDriver(make_shared<RedisDriver>(pPluginManager));
	if (pNoSqlDriver == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pNoSqlDriver == NULL";
		return false;
	}

	mvTypeSqlDriver.insert(make_pair(nConnectType, pNoSqlDriver));
	if (pNoSqlDriver->Connect(strIP, nPort, strPass))
	{
		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "redis connect succeed ip:" << strIP
			<< " port:" << nPort
			<< " name:" << strID;
		return true;
	}
	
	return false;
}

void NFNoSqlModule::RemoveConnect(ECON_TYPE nConnectType)
{
	auto it = mvTypeSqlDriver.find(nConnectType);
	if (mvTypeSqlDriver.end() == it)
	{
		return;
	}

	//delete it->second;智能指针
	mvTypeSqlDriver.erase(it);
}

void NFNoSqlModule::CheckConnect()
{
	static const int CHECK_TIME = 15;
	if (mLastCheckTime + CHECK_TIME > pPluginManager->GetNowTime())
	{
		return;
	}

	mLastCheckTime = pPluginManager->GetNowTime();
	
	bool bAllComplete = true;
	NFDataList args;

	for (auto& it : mvTypeSqlDriver)
	{
		NF_SHARE_PTR<RedisDriver> xNosqlDriver = it.second;
		if (xNosqlDriver && !xNosqlDriver->Enable())
		{
			xNosqlDriver->ReConnect();
			bAllComplete = false;
			std::string strIP;
			int nPort;
			xNosqlDriver->GetAddress(strIP, nPort);
			args << strIP
				<< nPort;

			QLOG_SYSTEM_S << "redis reconnect ip:" << strIP
				<< " port:" << nPort;
		}
	}
	
	NF_SHARE_PTR<NFIClass> pServerClass = m_pClassModule->GetElement(pPluginManager->GetAppName());
	if (pServerClass == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pServerClass == NULL";
		return;
	}

	if (!bNoSqlComplete && bAllComplete)
	{
		// 数据库连接全
		bNoSqlComplete = true;
		pServerClass->RunEventCallBack(EVENT_OnNoSqlComplete, NFGUID(), NFGUID(), NFDataList());
	}

	if (bNoSqlComplete && !bAllComplete)
	{
		// 数据库有断开
		bNoSqlComplete = false;
		pServerClass->RunEventCallBack(EVENT_OnNoSqlDisConnect, NFGUID(), NFGUID(), args);
	}
}
