///--------------------------------------------------------------------
/// 文件名:		ObjectRedis.cpp
/// 内  容:		Object数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "ObjectRedis.h"
#include "NFComm/NFCore/PBConvert.hpp"

CObjectRedis::CObjectRedis(NFIPluginManager * p)
{
	pPluginManager = p;
}

bool CObjectRedis::Init()
{
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pDBServer = pPluginManager->FindModule<CDBServer>();
	m_pColdData = pPluginManager->FindModule<CColdData>();
	
	return true;
}

bool CObjectRedis::AfterInit()
{
	// Object 冷数据处理
	if (!m_pDBServer->GetMysqlConnectColdData().empty())
	{
		m_pColdData->AddScanInfo(m_pDBServer->GetMysqlNameColdData(), ECON_TYPE_DATA, m_pCommonRedis->GetObjectCacheFlag());
	}

	return true;
}

bool CObjectRedis::LoadObjectData(const NFGUID& self, string& strData)
{
	if (self.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (!pDriver->HGET(strKey, m_pCommonRedis->GetObjectDataCacheKey(), strData))
	{
		QLOG_ERROR << __FUNCTION__ << " HGET failed:" << self;
		return false;
	}

	return true;
}

bool CObjectRedis::LoadData(const NFGUID& self, const std::string& key, std::string& value)
{
	if (self.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (!pDriver->HGET(strKey, key, value))
	{
		QLOG_ERROR << __FUNCTION__ << " HGET failed:" << self;
		return false;
	}

	return true;
}

bool CObjectRedis::LoadData(const NFGUID& self, const string_vector& vec_key, string_vector& vec_value)
{
	if (self.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (!pDriver->HMGET(strKey, vec_key, vec_value))
	{
		QLOG_ERROR << __FUNCTION__ << " HMGET failed:" << self.ToString();
		return false;
	}

	return true;
}

bool CObjectRedis::SaveObjectData(const NFGUID & self, const string& strData)
{
	if (self.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (!pDriver->HSET(strKey, m_pCommonRedis->GetObjectDataCacheKey(), strData))
	{
		QLOG_ERROR << __FUNCTION__ << " HSET failed:" << self;
		return false;
	}

	return true;
}

bool CObjectRedis::SaveData(const NFGUID& self, const std::string& key, const std::string& value)
{
	if (self.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (!pDriver->HSET(strKey, key, value))
	{
		QLOG_ERROR << __FUNCTION__ << " HSET failed:" << self;
		return false;
	}

	return true;
}

bool CObjectRedis::SaveData(const NFGUID& self, const string_vector& vec_key, const string_vector& vec_value)
{
	if (self.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (!pDriver->HMSET(strKey, vec_key, vec_value))
	{
		QLOG_ERROR << __FUNCTION__ << " HMSET failed:" << self;
		return false;
	}

	return true;
}

bool CObjectRedis::DeleteObject(const NFGUID& self)
{
	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (pDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL:" << self;
		return false;
	}

	std::string strKey = m_pCommonRedis->GetObjectCacheKey(self);
	if (pDriver->EXISTS(strKey))
	{
		if (!pDriver->DEL(strKey))
		{
			QLOG_ERROR << __FUNCTION__ << " DEL property failed" << self;
			return false;
		}	
	}

	return true;
}