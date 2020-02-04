///--------------------------------------------------------------------
/// 文件名:		NFINoSqlModule.h
/// 内  容:		数据库接口
/// 说  明:		
/// 创建日期:	2019年8月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_NOSQL_MODULE_H
#define NFI_NOSQL_MODULE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/IRedisDriver.h"

typedef std::vector<std::string> string_vector;
typedef std::pair<std::string, std::string> string_pair;
typedef std::vector<string_pair> string_pair_vector;
typedef std::pair<std::string, double> string_score_pair;
typedef std::vector<string_score_pair> string_score_vector;

enum ECON_TYPE
{
	ECON_TYPE_ACCOUNT = 0,
	ECON_TYPE_NAME = 1,
	ECON_TYPE_DATA = 2,
	ECON_TYPE_MAX,
};

class NFINoSqlModule : public NFIModule
{
public:
	virtual bool AddConnectSql(const std::string& strID, const std::string& strIP, const int nPort, 
		const std::string& strPass, ECON_TYPE nConnectType) = 0;

	virtual NF_SHARE_PTR<IRedisDriver> GetRedisDriver(ECON_TYPE eType) = 0;
	//virtual NF_SHARE_PTR<IRedisDriver> GetDriverBySuit(ECON_TYPE eType, const std::string& strHash) = 0;
};

#endif