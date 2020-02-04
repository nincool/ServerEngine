///--------------------------------------------------------------------
/// �ļ���:		ColdData.h
/// ��  ��:		������
/// ˵  ��:		
/// ��������:	2019��12��2��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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
		std::string strMysqlName = ""; // mysql������
		std::string strMysqlTable = ""; // mysql����
		ECON_TYPE eRedisType = ECON_TYPE::ECON_TYPE_DATA; // redis������
		std::string strRedisFlag = ""; // redis�洢��ֵ��ʶ
		int nScanIt = 0; // �����α�
	};

public:
	CColdData(NFIPluginManager* p);
	virtual bool Init();
	virtual bool AfterInit();
	virtual bool InitLua();
	virtual bool Execute();

	// �����Ҫɨ���������Ϣ
	bool AddScanInfo(const std::string& strName, ECON_TYPE eType, const std::string& strFlag, bool bPrefix = false);
	// ˢ������
	bool RefreshData(const std::string& strFlag, const std::string& key);

	// �������ж�ʱ��
	void SetColdDataTime(int value) { m_nColdDataTime = value; }
	int GetColdDataTime() { return m_nColdDataTime; }

	// ������ɨ��ʱ��
	void SetColdDataScanTime(int value) { m_nColdDataScanTime = value; }
	int GetColdDataScanTime() { return m_nColdDataScanTime; }

	// �ر�������
	void SetCloseFlag(const std::string& strFlag)
	{ 
		m_strCloseFlag = std::string(",") + strFlag + ","; 
	}
	const std::string& GetCloseFlag() { return m_strCloseFlag; }

private:

	// ��ȡ�´�ɨ��ʱ��
	int64_t GetNextScanTime();
	// ������
	bool CreateTable();
	// �Ƿ�����ɨ��
	bool IsScaning();
	// ɨ������
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

	//�������ж�ʱ�� ��
	int m_nColdDataTime = 2 * ONE_DAY_S;
	//������ɨ��ʱ��� �� (Ĭ�ϵڶ����賿����)
	int m_nColdDataScanTime = 3 * ONE_HOUR_S;

	NFMysqlModule* m_pNFMysqlModule = nullptr;
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;

	std::map<std::string, ScanInfo> m_mapScanInfo;

	// ���رյ������ݴ���
	std::string m_strCloseFlag;

	int64_t m_dnHitTime = 0;
	int64_t m_dnStartTime = 0;
	
};

#endif // __COLD_DATA_H__