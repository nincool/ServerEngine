///--------------------------------------------------------------------
/// 文件名:		ObjectRedis.h
/// 内  容:		玩家数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _OBJECT_REDIS_MODULE_H
#define _OBJECT_REDIS_MODULE_H

#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "CommonRedis.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "DBServer.h"
#include "ColdData.h"

class CObjectRedis : public NFIModule
{
public:
	CObjectRedis(NFIPluginManager* p);
	
	virtual bool Init();
	virtual bool AfterInit();

	virtual bool LoadObjectData(const NFGUID& self, string& strData);
	virtual bool LoadData(const NFGUID& self, const std::string& key, std::string& value);
	virtual bool LoadData(const NFGUID& self, const string_vector& vec_key, string_vector& vec_value);
	virtual bool SaveObjectData(const NFGUID& self, const string& strData);
	virtual bool SaveData(const NFGUID& self, const std::string& key, const std::string& value);
	virtual bool SaveData(const NFGUID& self, const string_vector& vec_key, const string_vector& vec_value);
	virtual bool DeleteObject(const NFGUID& self);

private:
	ICommonRedis* m_pCommonRedis = nullptr;
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	CDBServer* m_pDBServer = nullptr;
	CColdData* m_pColdData = nullptr;
};

#endif
