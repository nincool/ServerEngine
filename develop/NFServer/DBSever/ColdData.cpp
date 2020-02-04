///--------------------------------------------------------------------
/// 文件名:		ColdData.cpp
/// 内  容:		冷数据
/// 说  明:		
/// 创建日期:	2019年12月2日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ColdData.h"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "mysqlpp/ssqls.h"
#include "mysqlpp/query.h"
#include "mysql.h"

#define LUA_EXT_H
#include "NFComm/NFUtils/LuaExt.hpp"

// mysql表对应的结构
sql_create_3(cold_data,
	1, 3,
	mysqlpp::sql_varchar, key,
	mysqlpp::sql_datetime, save_time,
	mysqlpp::sql_blob, save_data);

const char* const COLD_DATA = "ColdData";

CColdData::CColdData(NFIPluginManager* p)
{
	pPluginManager = p;
}

bool CColdData::Init()
{
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pNFMysqlModule = pPluginManager->FindModule<NFMysqlModule>();

	return true;
}

bool CColdData::AfterInit()
{
	return true;
}

bool CColdData::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<CColdData>("CColdData")
		.addProperty("ColdDataScanTime", &CColdData::GetColdDataScanTime, &CColdData::SetColdDataScanTime)
		.addProperty("ColdDataTime", &CColdData::GetColdDataTime, &CColdData::SetColdDataTime)
		.addProperty("CloseFlag", &CColdData::GetCloseFlag, &CColdData::SetCloseFlag)
		.endClass();

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("init_ColdData", this);

	return true;
}

bool CColdData::Execute()
{
	// 一分钟扫描一次
	if (m_dnHitTime > pPluginManager->GetNowTime())
	{
		return true;
	}
	m_dnHitTime = pPluginManager->GetNowTime();

	if (m_dnStartTime <= 0)
	{
		// 创建表
		if (CreateTable())
		{
			// 创建表成功 初始化扫描时间
			m_dnStartTime = GetNextScanTime();
			return true;
		}

		// 1分钟尝试创建一次表
		m_dnHitTime = pPluginManager->GetNowTime() + ONE_MIN_S;

		return true;
	}
	
	// 每天开启一次
	if (m_dnHitTime < m_dnStartTime)
	{
		return true;
	}
	
	// 重置迭代器
	if (!IsScaning())
	{
		for (auto& it : m_mapScanInfo)
		{
			it.second.nScanIt = 0;
		}

		QLOG_INFO << " ScanData begin ";
	}

	// 扫描数据
	ScanData(m_dnHitTime);

	// 扫描完毕
	if (!IsScaning())
	{
		// 下次扫描时间
		m_dnStartTime = GetNextScanTime();

		QLOG_INFO << " ScanData end next_time:" << NFDateTime(m_dnStartTime * ONE_SEC_MS).ToString();
	}

	return true;
}

int64_t CColdData::GetNextScanTime()
{
	return NFDateTime::Today().TotalSecond() + GetColdDataScanTime() + ONE_DAY_S;
}

bool CColdData::CreateTable()
{
	for (auto& it_map : m_mapScanInfo)
	{
		auto& it = it_map.second;
		NFCMysqlDriver* pMysqlDriver = m_pNFMysqlModule->GetMysqlDriver(it.strMysqlName);
		if (pMysqlDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " mysql:" << it.strMysqlName;
			it.nScanIt = -1;
			return false;
		}

		std::string sql = GetCreateTableSql(it.strMysqlTable);
		if (!pMysqlDriver->Query(sql))
		{
			QLOG_ERROR << __FUNCTION__ << " mysql:" << it.strMysqlName << " sql:" << sql;
			return false;
		}
	}

	return true;
}

bool CColdData::IsScaning()
{
	for (auto& it : m_mapScanInfo)
	{
		if (it.second.nScanIt > 0)
		{
			return true;
		}
	}

	return false;
}

void CColdData::ScanData(int64_t dnCurTime)
{
	for (auto& it_map : m_mapScanInfo)
	{
		auto& it = it_map.second;
		NF_SHARE_PTR<IRedisDriver> pRedisDriver = m_pNoSqlModule->GetRedisDriver(it.eRedisType);
		if (pRedisDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " redis:" << it.eRedisType;
			it.nScanIt = -1;
			continue;
		}

		NFCMysqlDriver* pMysqlDriver = m_pNFMysqlModule->GetMysqlDriver(it.strMysqlName);
		if (pMysqlDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " mysql:" << it.strMysqlName;
			it.nScanIt = -1;
			return;
		}

		if (it.nScanIt < 0)
		{
			// 已经遍历完成
			continue;
		}

		std::vector<std::string> keys;
		it.nScanIt = pRedisDriver->SCAN(it.nScanIt, keys, it.strRedisFlag);
		if (it.nScanIt <= 0)
		{
			// 标识遍历完成
			it.nScanIt = -1;
		}

		for (int i = 0; i < keys.size(); ++i)
		{
			auto& key = keys[i];

			// 是否为冷数据
			if (!IsCold(pRedisDriver, key))
			{
				continue;
			}

			QLOG_INFO << " 1:Save ColdData Find key:" << key;

			// 取热数据
			OuterMsg::ColdDataStore xStore;
			if (!GetDataFromRedis(pRedisDriver, key, xStore))
			{
				continue;
			}

			QLOG_INFO << " 2:Save ColdData GetDataFromRedis succeed key:" << key;

			// 存为冷数据
			if (!SaveDataToMysql(pMysqlDriver, it.strMysqlTable, key, xStore))
			{
				continue;
			}

			QLOG_INFO << " 3:Save ColdData SaveDataToMysql succeed key:" << key;

			// 删除redis数据
			pRedisDriver->DEL(key);

			QLOG_INFO << " 4:Save ColdData DEL redis succeed key:" << key;

			CATCH_BEGIN
			auto& gauge_family = pPluginManager->GetMetricsGauge(15);
			auto& second_gauge = gauge_family.Add({ {"Flag", it_map.first} });
			second_gauge.Increment();
			CATCH_END
		}
	}                         
}

bool CColdData::AddScanInfo(
	const std::string& strName, 
	ECON_TYPE eType, 
	const std::string& strFlag, 
	bool bPrefix/* = false*/)
{
	if (strName.empty() 
		|| strFlag.empty())
	{
		return false;
	}

	// 是否过滤 
	if (m_strCloseFlag.find(strFlag) != std::string::npos)
	{
		// 不允许加入冷数据
		return false;
	}
	
	ScanInfo info;
	info.strMysqlName = strName;
	info.strMysqlTable = COLD_DATA + strFlag;
	info.eRedisType = eType;
	if (bPrefix)
	{
		info.strRedisFlag.append(strFlag);
		info.strRedisFlag.append("*");
	}
	else
	{
		info.strRedisFlag.append("*");
		info.strRedisFlag.append(strFlag);
	}

	m_mapScanInfo[strFlag] = info;

	return true;
}

bool CColdData::RefreshData(const std::string& strFlag, const std::string& key)
{
	if (strFlag.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " flag:" << strFlag << " key:" << key;
		return false;
	}

	auto it = m_mapScanInfo.find(strFlag);
	if (it == m_mapScanInfo.end())
	{
		QLOG_ERROR << __FUNCTION__ << " not find scaninfo flag:" << strFlag << " key:" << key;
		return false;
	}

	// redis type
	auto& eType = it->second.eRedisType;
	// mysql name
	auto& strName = it->second.strMysqlName;
	auto& strTable = it->second.strMysqlTable;

	// 取热数据
	NF_SHARE_PTR<IRedisDriver> pRedisDriver = m_pNoSqlModule->GetRedisDriver(eType);
	if (pRedisDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " redis:" << eType;
		return false;
	}

	// 存在热数据
	if (pRedisDriver->EXISTS(key))
	{
		// 更新时间
		pRedisDriver->HSET(key, m_pCommonRedis->GetUpdateTime(), to_string(pPluginManager->GetNowTime()));
		return true;
	}

	QLOG_INFO << " 1:Query ColdData not find in redis key:" << key;

	// 取冷数据
	NFCMysqlDriver* pMysqlDriver = m_pNFMysqlModule->GetMysqlDriver(strName);
	if (pMysqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " mysql:" << strName;
		return false;
	}
	
	// 判断是否存在冷数据
	if (!IsExistDataMysql(pMysqlDriver, strTable, key))
	{
		QLOG_INFO << " 2:Query ColdData not find in mysql key:" << key;

		// 不存在冷数据 新建热数据
		if (!pRedisDriver->HSET(key, 
			m_pCommonRedis->GetUpdateTime(), 
			to_string(pPluginManager->GetNowTime())))
		{
			QLOG_ERROR << __FUNCTION__ << " new redis data failed key:" << key;
			return false;
		}
		QLOG_INFO << __FUNCTION__ << " new redis data succeed key:" << key;
		
		CATCH_BEGIN
		auto& counter_family = pPluginManager->GetMetricsCounter(14);
		auto& second_counter = counter_family.Add({ {"Flag", strFlag} });
		second_counter.Increment();
		CATCH_END

		return true;
	}

	QLOG_INFO << " 2:Query ColdData find in mysql key:" << key;

	// 取冷数据
	OuterMsg::ColdDataStore xStore;
	if (!GetDataFromMysql(pMysqlDriver, strTable, key, xStore))
	{
		return false;
	}

	QLOG_INFO << " 3:Query ColdData from mysql key:" << key;

	// 存入redis
	if (!SaveDataToRedis(pRedisDriver, key, xStore))
	{
		return false;
	}

	QLOG_INFO << " 4:Query ColdData save redis key:" << key;

	// 删除mysql数据
	if (!DeleteDataToMysql(pMysqlDriver, strTable, key))
	{
		return false;
	}

	QLOG_INFO << " 5:Query ColdData delete mysql key:" << key;

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(15);
	auto& second_gauge = gauge_family.Add({ {"Flag", strFlag} });
	second_gauge.Decrement();
	CATCH_END

	return true;
}

bool CColdData::IsCold(NF_SHARE_PTR<IRedisDriver> pRedisDriver, const std::string& key)
{
	if (pRedisDriver == nullptr
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	// 刷新时间
	std::string value;
	if (!pRedisDriver->HGET(key, m_pCommonRedis->GetUpdateTime(), value))
	{
		// 没有刷新时间，增加当前时间为刷新时间
		value = to_string(pPluginManager->GetNowTime());
		pRedisDriver->HSET(key, m_pCommonRedis->GetUpdateTime(), value);
		return false;
	}

	// 判断是否为冷数据
	int64_t dnUpdatetime = ::lexical_cast<int64_t>(value);
	if (dnUpdatetime + GetColdDataTime() > pPluginManager->GetNowTime())
	{
		// 数据还没有冷
		return false;
	}

	return true;
}

bool CColdData::IsExistDataMysql(NFCMysqlDriver* pMysqlDriver, 
	const std::string strTable, 
	const std::string& key)
{
	if (pMysqlDriver == nullptr
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	const std::string& sql_find = GetFindSql(strTable, key);
	if (sql_find.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " GetFindSql failed key:" << key;
		return false;
	}

	mysqlpp::UseQueryResult queryResult;
	if (!pMysqlDriver->Query(sql_find, queryResult))
	{
		QLOG_ERROR << __FUNCTION__ << " query failed key:" << key;
		return false;
	}

	return !queryResult.fetch_row().empty();
}

bool CColdData::GetDataFromMysql(NFCMysqlDriver* pMysqlDriver,
	const std::string strTable,
	const std::string& key, 
	OuterMsg::ColdDataStore& xStore)
{
	if (pMysqlDriver == nullptr
		|| strTable.empty()
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	// 查询sql
	std::string sql = GetSelectSaveDataSql(strTable, key);
	if (sql.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " GetSelectSaveDataSql failed. table:" << strTable << " key:" << key;
		return false;
	}

	// mysql数据
	mysqlpp::UseQueryResult queryResult;
	if (!pMysqlDriver->Query(sql, queryResult))
	{
		QLOG_ERROR << __FUNCTION__ << " Query failed. sql:" << sql;
		return false;
	}
	mysqlpp::Row row = queryResult.fetch_row();
	if (row.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " Query row empty. sql:" << sql;
		return false;
	}

	auto& save_data = row["save_data"];
	if (save_data.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " Query strData empty. sql:" << sql;
		return false;
	}

	// 反序列化数据
	if (!xStore.ParseFromArray(save_data.c_str(), save_data.length()))
	{
		QLOG_ERROR << __FUNCTION__ << " ParseFromArray failed.";
		return false;
	}

	return true;
}

bool CColdData::SaveDataToMysql(NFCMysqlDriver* pMysqlDriver, 
	const std::string strTable,
	const std::string& key, 
	const OuterMsg::ColdDataStore& xStore)
{
	if (pMysqlDriver == nullptr
		|| strTable.empty()
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	std::string strData = "";
	if (!xStore.SerializeToString(&strData))
	{
		QLOG_ERROR << __FUNCTION__ << " SerializeToString failed";
		return false;
	}

	// 是否已经存在
	bool bExist = IsExistDataMysql(pMysqlDriver, strTable, key);

	// 执行更新或者插入
	auto* pConn = pMysqlDriver->GetConnection();
	if (pConn == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " conn nullptr key:" << key << " mysql:" << pMysqlDriver->GetConnectName() << "," << strTable;
		return false;
	}

	cold_data save_row;
	save_row.instance_table(strTable.c_str());
	save_row.key = key;
	save_row.save_time = mysqlpp::DateTime(NFDateTime::Now().ToString());
	save_row.save_data = strData;

	CATCH_BEGIN
	auto query = pConn->query();
	if (bExist)
	{
		query.update(save_row, save_row);
	}
	else
	{
		query.insert(save_row);
	}
	if (!pMysqlDriver->Query(query))
	{
		QLOG_ERROR << __FUNCTION__ 
			<< " update failed key:" << key 
			<< " mysql:" << pMysqlDriver->GetConnectName();
		return false;
	}
	CATCH_END

	// 检测存入数据
	const std::string sql = GetSelectSaveTimeSql(strTable, key);
	if (sql.empty())
	{
		QLOG_ERROR << __FUNCTION__ 
			<< " GetSelectSaveTimeSql failed. table:" 
			<< strTable << " key:" << key;
		return false;
	}
	// mysql数据 时间
	mysqlpp::UseQueryResult queryResult;
	if (!pMysqlDriver->Query(sql, queryResult))
	{
		QLOG_ERROR << __FUNCTION__ << " Query failed. sql:" << sql;
		return false;
	}
	mysqlpp::Row row = queryResult.fetch_row();
	if (row.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " Query row empty. sql:" << sql;
		return false;
	}

	auto& save_time = row["save_time"];
	if (save_time.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " Query strData empty. sql:" << sql;
		return false;
	}

	return save_time == save_row.save_time;
}

bool CColdData::DeleteDataToMysql(NFCMysqlDriver* pMysqlDriver,
	const std::string strTable,
	const std::string& key)
{
	if (pMysqlDriver == nullptr
		|| strTable.empty()
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	const std::string& sql = GetDeleteSql(strTable, key);
	if (sql.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " GetFindSql failed key:" << key;
		return false;
	}

	if (!pMysqlDriver->Query(sql))
	{
		QLOG_ERROR << __FUNCTION__ << " query failed key:" << key;
		return false;
	}

	return true;
}

bool CColdData::GetDataFromRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver, 
	const std::string& key, 
	OuterMsg::ColdDataStore& xStore)
{
	if (pRedisDriver == nullptr
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	// redis冷却数据
	string_pair_vector values;
	if (!pRedisDriver->HGETALL(key, values))
	{
		QLOG_ERROR << __FUNCTION__ << " hgetall key:" << key;
		return false;
	}
	if (values.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " hgetall empty key:" << key;
		return false;
	}

	// 序列化冷却数据
	for (auto& it : values)
	{
		auto* pData = xStore.add_data();
		if (pData == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " add_data ";
			return false;
		}

		pData->set_name(std::move(it.first));
		pData->set_data(std::move(it.second));
	}

	return true;
}

bool CColdData::SaveDataToRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver,
	const std::string& key, 
	const OuterMsg::ColdDataStore& xStore)
{
	if (pRedisDriver == nullptr
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	// redis数据
	std::vector<std::string> vec_field;
	std::vector<std::string> vec_value;
	for (int i = 0; i < xStore.data_size(); ++i)
	{
		vec_field.push_back(xStore.data(i).name());
		vec_value.push_back(xStore.data(i).data());
	}

	if (!SaveDataToRedis(pRedisDriver, key, vec_field, vec_value))
	{
		return false;
	}

	return true;
}

bool CColdData::SaveDataToRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver,
	const std::string& key, 
	const std::vector<std::string>& vec_field,
	const std::vector<std::string>& vec_value)
{
	if (pRedisDriver == nullptr
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	if (vec_value.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " data empty" << " key:" << key;
		return false;
	}

	if (!pRedisDriver->HMSET(key, vec_field, vec_value))
	{
		QLOG_ERROR << __FUNCTION__ << " hmset failed " << " key:" << key << "field_size:" << vec_field.size() << " value_size" << vec_value.size();
		return false;
	}

	// 更新时间
	if (!pRedisDriver->HSET(key, m_pCommonRedis->GetUpdateTime(), to_string(pPluginManager->GetNowTime())))
	{
		QLOG_ERROR << __FUNCTION__ << " hset failed " << " key:" << key;
		return false;
	}

	return true;
}

bool CColdData::SaveDataToRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver,
	const std::string& key,
	const std::string& field,
	const std::string& value)
{
	if (pRedisDriver == nullptr
		|| key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " nullptr key:" << key;
		return false;
	}

	if (field.empty() || value.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " data empty " << " key:" << key << " field:" << field;
		return false;
	}

	if (!pRedisDriver->HSET(key, field, value))
	{
		QLOG_ERROR << __FUNCTION__ << " hset failed " << " key:" << key;
		return false;
	}

	// 更新时间
	if (!pRedisDriver->HSET(key, m_pCommonRedis->GetUpdateTime(), to_string(pPluginManager->GetNowTime())))
	{
		QLOG_ERROR << __FUNCTION__ << " hset failed " << " key:" << key;
		return false;
	}

	return true;
}

const std::string CColdData::GetCreateTableSql(const std::string& strTableName)
{
	if (strTableName.empty())
	{
		return NULL_STR;
	}

	return std::move(std::string("CREATE TABLE IF NOT EXISTS ") + strTableName + "(" + \
		"`key` VARCHAR(64) NOT NULL," + \
		"`save_time` DateTime," + \
		"`save_data` longblob," + \
		"PRIMARY KEY (`key`)" + \
		");");
}

const std::string CColdData::GetInsertOrUpdateSql(
	const std::string& strTableName, 
	const std::string& strKey, 
	const std::string& strData)
{
	if (strTableName.empty()
		|| strKey.empty())
	{
		return NULL_STR;
	}

	std::string strCurTime = NFDateTime::Now().ToString();
	return std::move(std::string("INSERT INTO ") + strTableName + \
		" VALUES('" + strKey + "', '" + strCurTime + "', '" + strData + "')" + \
		" ON DUPLICATE KEY" + \
		" UPDATE save_time='" + strCurTime + \
		"', save_data='" + strData + "';");
}

const std::string CColdData::GetSelectSaveDataSql(const std::string& strTableName, const std::string& strKey)
{
	if (strTableName.empty()
		|| strKey.empty())
	{
		return NULL_STR;
	}

	return std::move(std::string("SELECT save_data FROM ") + strTableName \
		+ " WHERE " + strTableName + ".key='" + strKey + "';");
}

const std::string CColdData::GetSelectSaveTimeSql(const std::string& strTableName, const std::string& strKey)
{
	if (strTableName.empty()
		|| strKey.empty())
	{
		return NULL_STR;
	}

	return std::move(std::string("SELECT save_time FROM ") + strTableName \
		+ " WHERE " + strTableName + ".key='" + strKey + "';");
}

const std::string CColdData::GetFindSql(const std::string& strTableName, const std::string& strKey)
{
	if (strTableName.empty()
		|| strKey.empty())
	{
		return NULL_STR;
	}

	return std::move(std::string("SELECT 1 FROM ") + strTableName \
		+ " WHERE " + strTableName + ".key='" + strKey + "' LIMIT 1;");
}

const std::string CColdData::GetDeleteSql(const std::string& strTableName, const std::string& strKey)
{
	if (strTableName.empty()
		|| strKey.empty())
	{
		return NULL_STR;
	}

	return std::move(std::string("DELETE FROM ") + strTableName \
		+ " WHERE " + strTableName + ".key='" + strKey + "' LIMIT 1;");
}
