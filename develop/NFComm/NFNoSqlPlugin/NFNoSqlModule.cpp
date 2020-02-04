///--------------------------------------------------------------------
/// �ļ���:		NFNoSqlModule.cpp
/// ��  ��:		���ݿ�
/// ˵  ��:		
/// ��������:	2019��9��2��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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

//��ȡ���ݿ�ڵ�
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

	//delete it->second;����ָ��
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
		// ���ݿ�����ȫ
		bNoSqlComplete = true;
		pServerClass->RunEventCallBack(EVENT_OnNoSqlComplete, NFGUID(), NFGUID(), NFDataList());
	}

	if (bNoSqlComplete && !bAllComplete)
	{
		// ���ݿ��жϿ�
		bNoSqlComplete = false;
		pServerClass->RunEventCallBack(EVENT_OnNoSqlDisConnect, NFGUID(), NFGUID(), args);
	}
}
