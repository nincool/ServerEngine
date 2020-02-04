///--------------------------------------------------------------------
/// 文件名:		ColdData.h
/// 内  容:		冷数据
/// 说  明:		
/// 创建日期:	2019年12月2日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __COLD_DATA_H__
#define __COLD_DATA_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFMysqlPlugin/NFMysqlModule.h"
#include "NFComm/NFMysqlPlugin/NFCMysqlDriver.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFPluginModule/ICommonRedis.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFCore/NFDateTime.hpp"

class CColdData : public NFIModule
{
private:
	struct ScanInfo
	{
		std::string strMysqlName = ""; // mysql库名称
		std::string strMysqlTable = ""; // mysql表名
		ECON_TYPE eRedisType = ECON_TYPE::ECON_TYPE_DATA; // redis库类型
		std::string strRedisFlag = ""; // redis存储键值标识
		int nScanIt = 0; // 遍历游标
	};

public:
	CColdData(NFIPluginManager* p);
	virtual bool Init();
	virtual bool AfterInit();
	virtual bool InitLua();
	virtual bool Execute();

	// 添加需要扫描的数据信息
	bool AddScanInfo(const std::string& strName, ECON_TYPE eType, const std::string& strFlag, bool bPrefix = false);
	// 刷新数据
	bool RefreshData(const std::string& strFlag, const std::string& key);

	// 冷数据判定时间
	void SetColdDataTime(int value) { m_nColdDataTime = value; }
	int GetColdDataTime() { return m_nColdDataTime; }

	// 冷数据扫描时间
	void SetColdDataScanTime(int value) { m_nColdDataScanTime = value; }
	int GetColdDataScanTime() { return m_nColdDataScanTime; }

	// 关闭冷数据
	void SetCloseFlag(const std::string& strFlag)
	{ 
		m_strCloseFlag = std::string(",") + strFlag + ","; 
	}
	const std::string& GetCloseFlag() { return m_strCloseFlag; }

private:

	// 获取下次扫描时间
	int64_t GetNextScanTime();
	// 创建表
	bool CreateTable();
	// 是否正在扫描
	bool IsScaning();
	// 扫描数据
	void ScanData(int64_t dnCurTime);

	bool IsCold(NF_SHARE_PTR<IRedisDriver> pRedisDriver, const std::string& key);
	bool IsExistDataMysql(NFCMysqlDriver* pMysqlDriver, const std::string strTable, const std::string& key);
	bool GetDataFromMysql(NFCMysqlDriver* pMysqlDriver, const std::string strTable, const std::string& key, OuterMsg::ColdDataStore& xStore);
	bool SaveDataToMysql(NFCMysqlDriver* pMysqlDriver, const std::string strTable, const std::string& key, const OuterMsg::ColdDataStore& xStore);
	bool DeleteDataToMysql(NFCMysqlDriver* pMysqlDriver, const std::string strTable, const std::string& key);
	bool GetDataFromRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver, const std::string& key, OuterMsg::ColdDataStore& xStore);
	bool SaveDataToRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver, const std::string& key, const OuterMsg::ColdDataStore& xStore);
	bool SaveDataToRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver, const std::string& key, const std::vector<std::string>& vec_field, const std::vector<std::string>& vec_value);
	bool SaveDataToRedis(NF_SHARE_PTR<IRedisDriver> pRedisDriver, const std::string& key, const std::string& field, const std::string& value);

	const std::string GetCreateTableSql(const std::string& strTableName);
	const std::string GetInsertOrUpdateSql(const std::string& strTableName, const std::string& strKey, const std::string& strData);
	const std::string GetSelectSaveDataSql(const std::string& strTableName, const std::string& strKey);
	const std::string GetSelectSaveTimeSql(const std::string& strTableName, const std::string& strKey);
	const std::string GetFindSql(const std::string& strTableName, const std::string& strKey);
	const std::string GetDeleteSql(const std::string& strTableName, const std::string& strKey);
private:

	//冷数据判定时间 秒
	int m_nColdDataTime = 2 * ONE_DAY_S;
	//冷数据扫描时间点 秒 (默认第二天凌晨三点)
	int m_nColdDataScanTime = 3 * ONE_HOUR_S;

	NFMysqlModule* m_pNFMysqlModule = nullptr;
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;

	std::map<std::string, ScanInfo> m_mapScanInfo;

	// 被关闭的冷数据处理
	std::string m_strCloseFlag;

	int64_t m_dnHitTime = 0;
	int64_t m_dnStartTime = 0;
	
};

#endif // __COLD_DATA_H__