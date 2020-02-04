///--------------------------------------------------------------------
/// 文件名:		RankRedis.cpp
/// 内  容:		排行榜数据库
/// 说  明:		
/// 创建日期:	2019年9月26日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "RankRedis.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"

// 每次查询范围最大数量
const int MAX_EVERY_GET_RANGE = 100;

RankRedis::RankRedis(NFIPluginManager * p)
{
	pPluginManager = p;
}

bool RankRedis::Init()
{
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pDBServer = pPluginManager->FindModule<CRankDBServer>();
	m_pColdData = pPluginManager->FindModule<CColdData>();
	
	return true;
}

bool RankRedis::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_INIT, this, (NetMsgFun)&RankRedis::OnRankInit);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_CREATE, this, (NetMsgFun)&RankRedis::OnRankCreate);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_DELETE, this, (NetMsgFun)&RankRedis::OnRankDelete);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_UPDATE, this, (NetMsgFun)&RankRedis::OnRankUpdate);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_SET_DATA, this, (NetMsgFun)&RankRedis::OnRankSetData);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_GET_DATA, this, (NetMsgFun)&RankRedis::OnRankGetData);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_GET_DATA_ARRAY, this, (NetMsgFun)&RankRedis::OnRankGetDataArray);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_GET_RANK_DATA, this, (NetMsgFun)&RankRedis::OnRankGetRankData);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_DB_RANK_LIST_GET_RANGE_DATA, this, (NetMsgFun)&RankRedis::OnRankGetRangeData);

	// Rank 冷数据处理
	if (!m_pDBServer->GetMysqlConnectColdData().empty())
	{
		m_pColdData->AddScanInfo(m_pDBServer->GetMysqlNameColdData(), ECON_TYPE_DATA, m_pCommonRedis->GetRankDataCacheFlag());
	}

	return true;
}

void RankRedis::OnRankInit(
	const NFSOCK nSockIndex, 
	const int nMsgID, 
	const char* msg, 
	const uint32_t nLen)
{
	// 已经反馈过数据
	if (m_nInit > 0)
	{
		// 请求次数超过10次，说明需要再次初始化
		++m_nInit;
		if (m_nInit > 10)
		{
			m_nInit = 0;
		}

		return;
	}

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	string_pair_vector vec_data;
	if (!xNoSqlDriver->HGETALL(m_pCommonRedis->GetRankListCacheKey(nServerID), vec_data))
	{
		QLOG_ERROR << __FUNCTION__ << " HGETALL failed ";
		return;
	}

	OuterMsg::RankListInit xInit;
	for (int i = 0; i < vec_data.size(); ++i)
	{
		xInit.Clear();

		auto* pCreate = xInit.mutable_create();
		if (pCreate == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " mutable_create failed ";
			continue;
		}

		if (!pCreate->ParseFromString(vec_data[i].second))
		{
			QLOG_WARING << __FUNCTION__ << " ParseFromString failed RankName:" << vec_data[i].first;
			continue;
		}

		// 排行榜所有成员
		std::string& strRankName = vec_data[i].first;
		std::string strRankKey = m_pCommonRedis->GetRankCacheKey(strRankName);
		string_pair_vector vec_rank_datas;
		if (!xNoSqlDriver->HGETALL(strRankKey, vec_rank_datas))
		{
			QLOG_ERROR << __FUNCTION__ << " HGETALL failed:" << strRankKey;
			continue;
		}

		// 成员分数数据
		for (auto& it : vec_rank_datas)
		{
			auto* pScore = xInit.add_data_rank();
			if (pScore != nullptr)
			{
				pScore->ParseFromString(it.second);
			}
		}
		
		std::string strInit;
		if (!xInit.SerializeToString(&strInit))
		{
			QLOG_WARING << __FUNCTION__ << "SerializeToString failed";
			continue;
		}

		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_DB_RANK_LIST_INIT, strInit, nSockIndex);
	}

	xInit.Clear();
	xInit.set_finish(1);
	std::string strInit;
	if (!xInit.SerializeToString(&strInit))
	{
		QLOG_WARING << __FUNCTION__ << " SerializeToString failed";
	}
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_DB_RANK_LIST_INIT, strInit, nSockIndex);

	m_nInit++;

	if (m_pDBServer->IsComplete())
	{
		CATCH_BEGIN
		auto& gauge_family = pPluginManager->GetMetricsGauge(1);
		auto& second_gauge = gauge_family.Add({ {"State", "Opened"} });
		second_gauge.Increment();
		CATCH_END
	}
}

void RankRedis::OnRankCreate(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::RankListCreate xCreate;
	if (!xCreate.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " RankListCreate ParseFromArray failed";
		return;
	}

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	std::string strRankName = xCreate.name();
	std::string strCreate(msg, nLen);
	if (!xNoSqlDriver->HSET(m_pCommonRedis->GetRankListCacheKey(nServerID), strRankName, strCreate))
	{
		QLOG_ERROR << __FUNCTION__ << " HSET failed:" << m_pCommonRedis->GetRankListCacheKey(nServerID)
			<< " name:" << strRankName;
		return;
	}
}

void RankRedis::OnRankDelete(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::RankListDelete xDelete;
	if (!xDelete.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " RankListDelete ParseFromArray failed";
		return;
	}

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;
		return;
	}

	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (pServerData == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " no find server data nSockIndex:" << nSockIndex;
		return;
	}
	const int& nServerID = pServerData->report.server_id();

	std::string strRankName = xDelete.name();
	if (!xNoSqlDriver->HDEL(m_pCommonRedis->GetRankListCacheKey(nServerID), strRankName))
	{
		QLOG_ERROR << __FUNCTION__ << " HDEL failed " << m_pCommonRedis->GetRankListCacheKey(nServerID) 
			<< " name:" << strRankName;
		return;
	}

	std::string strRankKey = m_pCommonRedis->GetRankCacheKey(strRankName);
	if (!xNoSqlDriver->DEL(strRankKey))
	{
		QLOG_ERROR << __FUNCTION__ << " del failed:" << strRankKey;
		return;
	}
}

void RankRedis::OnRankUpdate(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::RankListUpdate xUpdate;
	if (!xUpdate.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " RankListUpdate ParseFromArray failed";
		return;
	}

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;
		return;
	}

	const std::string& strRankName = xUpdate.name();
	if (strRankName.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " RankName empty";
		return;
	}

	std::string strRankKey = m_pCommonRedis->GetRankCacheKey(strRankName);

	// 删除数据
	NFGUID del;
	PBConvert::ToNF(xUpdate.del(), del);
	if (!del.IsNull())
	{
		if (!xNoSqlDriver->HDEL(strRankKey, del.ToString()))
		{
			std::ostringstream str;
			QLOG_ERROR << __FUNCTION__ << " HDEL failed:" << strRankKey
				<< " uid:" << del;
			return;
		}
	}

	auto& xScore = xUpdate.update();
	NFGUID add;
	PBConvert::ToNF(xScore.id(), add);
	if (add.IsNull())
	{
		// 没有需要刷新的数据
		return;
	}

	if (!xNoSqlDriver->HSET(strRankKey, add.ToString(), xScore.SerializeAsString()))
	{
		QLOG_ERROR << __FUNCTION__ << " HSET failed:" << strRankKey 
			<< " uid:" << add;
		return;
	}

	// 更新数据
	if (xScore.arr_data().empty())
	{
		// 没有分数更新
		return;
	}

	string_vector key;
	string_vector value;
	for (int i = 0; i < xScore.arr_data_size(); ++i)
	{
		key.push_back(xScore.arr_data(i).name());
		value.push_back(to_string(xScore.arr_data(i).data()));
	}

	if (key.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " score empty";
		return;
	}

	std::string strDataKey = m_pCommonRedis->GetRankDataCacheKey(add);
	if (!xNoSqlDriver->HMSET(strDataKey, key, value))
	{
		QLOG_ERROR << __FUNCTION__ << " HMSET failed:" << strDataKey;
		return;
	}
}

void RankRedis::OnRankSetData(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::RankListData xData;
	if (!xData.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " RankListData ParseFromArray failed";
		return;
	}

	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (xNoSqlDriver == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;
		return;
	}

	NFGUID guid;
	PBConvert::ToNF(xData.id(), guid);
	if (guid.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " guid is null";
		return;
	}

	string_vector key;
	string_vector value;
	for (auto& it : xData.map_data())
	{
		if (it.first.empty())
		{
			QLOG_ERROR << __FUNCTION__ << " key is empty";
			continue;
		}
		key.push_back(it.first);
		value.push_back(it.second);
	}

	std::string strDataKey = m_pCommonRedis->GetRankDataCacheKey(guid);
	if (!xNoSqlDriver->HMSET(strDataKey, key, value))
	{
		QLOG_ERROR << __FUNCTION__ << " HMSET failed:" << strDataKey;
		return;
	}
}

void RankRedis::OnRankGetData(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg ParseFromArray failed";
		return;
	}

	OuterMsg::RankListData xData;
	if (!xData.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << " RankListData ParseFromString failed";
		return;
	}

	OuterMsg::RankListDataAck reply;
	*reply.mutable_id() = std::move(xData.id());
	reply.set_ret_code(OuterMsg::RANK_CODE_SUCCEED);
	*reply.mutable_attach_data() = std::move(xData.attach_data());

	do 
	{
		NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
		if (xNoSqlDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;

			reply.set_ret_code(OuterMsg::RANK_CODE_UNKNOW);
			break;
		}

		NFGUID guid;
		PBConvert::ToNF(xData.id(), guid);
		if (guid.IsNull())
		{
			QLOG_WARING << __FUNCTION__ << " guid is null";

			reply.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		string_vector key;
		string_vector value;
		for (auto& it : xData.map_data())
		{
			key.push_back(it.first);
		}

		std::string strDataKey = m_pCommonRedis->GetRankDataCacheKey(guid);
		if (!xNoSqlDriver->HMGET(strDataKey, key, value))
		{
			QLOG_ERROR << __FUNCTION__ << " HMGET failed:" << strDataKey;

			reply.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		if (value.empty() || key.size() != value.size())
		{
			reply.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		auto* pMapData = reply.mutable_map_data();
		if (pMapData == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " get map data failed:" << strDataKey;

			reply.set_ret_code(OuterMsg::RANK_CODE_UNKNOW);
			break;
		}

		pMapData->clear();
		if (!value.empty() && key.size() == value.size())
		{
			for (int i = 0; i < key.size(); ++i)
			{
				(*pMapData)[key[i]] = value[i];
			}
		}
	} while (false);
	
	std::string strReply;
	if (!reply.SerializeToString(&strReply))
	{
		QLOG_WARING << __FUNCTION__ << "RankListData SerializeToString failed";
		return;
	}

	xMsgBase.set_data(std::move(strReply));
	std::string strMsgBase;
	if (!xMsgBase.SerializeToString(&strMsgBase))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg SerializeToString failed";
		return;
	}
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_DB_RETURN_RANK_LIST, strMsgBase, nSockIndex);
}

void RankRedis::OnRankGetDataArray(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ExtraMsg ParseFromArray failed";
		return;
	}

	OuterMsg::RankListDataArray xData;
	if (!xData.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << " RankListDataArray ParseFromString failed";
		return;
	}

	// 需要取得属性名
	string_vector key;
	string_vector value;
	for (int i = 0; i < xData.name_list_size(); ++i)
	{
		key.push_back(xData.name_list(i));
	}
	
	OuterMsg::RankListDataArrayAck reply;
	do
	{
		NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
		if (xNoSqlDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;

			reply.set_ret_code(OuterMsg::RANK_CODE_UNKNOW);
			break;
		}

		for (int i = 0; i < xData.id_list_size(); ++i)
		{
			if (i > MAX_EVERY_GET_RANGE)
			{
				break;
			}

			auto* pData = reply.add_data_list();
			if (pData == nullptr)
			{
				continue;
			}
			*pData->mutable_id() = std::move(xData.id_list(i));

			NFGUID guid;
			PBConvert::ToNF(pData->id(), guid);
			if (guid.IsNull())
			{
				QLOG_ERROR << __FUNCTION__ << " guid is null";

				reply.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
				continue;
			}

			value.clear();
			std::string strDataKey = m_pCommonRedis->GetRankDataCacheKey(guid);
			if (!xNoSqlDriver->HMGET(strDataKey, key, value))
			{
				QLOG_ERROR << __FUNCTION__ << "HMGET failed:" << strDataKey;

				reply.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
				continue;
			}

			if (value.empty() || key.size() != value.size())
			{
				reply.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
				continue;
			}

			for (int i = 0; i < value.size(); ++i)
			{
				*pData->add_data_list() = std::move(value[i]);
			}
		}
	} while (false);

	std::string strReply;
	*reply.mutable_name_list() = std::move(xData.name_list());
	*reply.mutable_attach_data() = std::move(xData.attach_data());
	if (!reply.SerializeToString(&strReply))
	{
		QLOG_WARING << __FUNCTION__ << "RankListData SerializeToString failed";
		return;
	}

	xMsgBase.set_data(std::move(strReply));
	std::string strMsgBase;
	if (!xMsgBase.SerializeToString(&strMsgBase))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg SerializeToString failed";
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_DB_RETURN_RANK_LIST, strMsgBase, nSockIndex);
}

void RankRedis::OnRankGetRankData(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg ParseFromArray failed";
		return;
	}

	OuterMsg::RankListRankAck xRankAck;
	if (!xRankAck.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__  << "RankListRankAck ParseFromString failed";
		return;
	}

	do
	{
		NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
		if (xNoSqlDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;

			xRankAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		auto* pMapData = xRankAck.mutable_map_data();
		if (pMapData == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " get map data failed";

			xRankAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		// 统计需要获取的属性名
		string_vector key;
		string_vector value;
		for (auto& it : *pMapData)
		{
			key.push_back(it.first);
		}

		if (key.empty())
		{
			QLOG_WARING << __FUNCTION__ << " data name empty";

			xRankAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		value.clear();
		NFGUID guid;
		PBConvert::ToNF(xRankAck.id(), guid);
		if (guid.IsNull())
		{
			xRankAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}
		std::string strDataKey = m_pCommonRedis->GetRankDataCacheKey(guid);
		if (!xNoSqlDriver->HMGET(strDataKey, key, value))
		{
			QLOG_ERROR << __FUNCTION__ << " HMGET failed:" << strDataKey;

			xRankAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		if (value.empty() || key.size() != value.size())
		{
			xRankAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		pMapData->clear();
		if (!value.empty() && key.size() == value.size())
		{
			for (int i = 0; i < key.size(); ++i)
			{
				(*pMapData)[key[i]] = value[i];
			}
		}
	} while (false);

	std::string strRankAck;
	if (!xRankAck.SerializeToString(&strRankAck))
	{
		QLOG_WARING << __FUNCTION__ << "RankListRank SerializeToString failed";
		return;
	}

	xMsgBase.set_data(std::move(strRankAck));
	std::string strMsgBase;
	if (!xMsgBase.SerializeToString(&strMsgBase))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg SerializeToString failed";
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_DB_RETURN_RANK_LIST, strMsgBase, nSockIndex);
}

void RankRedis::OnRankGetRangeData(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg ParseFromArray failed";
		return;
	}

	OuterMsg::RankListRangeAck xRangeAck;
	if (!xRangeAck.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << "RankListRangeAck ParseFromString failed";
		return;
	}

	do 
	{
		NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
		if (xNoSqlDriver == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " not find NoSqlDriver:" << ECON_TYPE_DATA;

			xRangeAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		if (xRangeAck.data_size() <= 0)
		{
			xRangeAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		string_vector key;
		string_vector value;
		auto& range_data0 = xRangeAck.data(0);

		// 统计需要获取的属性名
		for (auto& it : range_data0.map_data())
		{
			key.push_back(it.first);
		}

		if (key.empty())
		{
			QLOG_WARING << __FUNCTION__ << " data name empty";

			xRangeAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
			break;
		}

		for (int i = 0; i < xRangeAck.data_size(); ++i)
		{
			auto* pRangeData = xRangeAck.mutable_data(i);
			if (pRangeData == nullptr)
			{
				continue;
			}

			value.clear();
			NFGUID guid;
			PBConvert::ToNF(pRangeData->id(), guid);
			if (guid.IsNull())
			{
				continue;
			}
			std::string strDataKey = m_pCommonRedis->GetRankDataCacheKey(guid);
			if (!xNoSqlDriver->HMGET(strDataKey, key, value))
			{
				QLOG_ERROR << __FUNCTION__ << " HMGET failed:" << strDataKey;
				continue;
			}

			if (value.empty() || key.size() != value.size())
			{
				xRangeAck.set_ret_code(OuterMsg::RANK_CODE_DB_GET_DATA_FAILED);
				continue;
			}

			auto* pMapData = pRangeData->mutable_map_data();
			if (pMapData == nullptr)
			{
				QLOG_WARING << __FUNCTION__ << " get map data failed:" << strDataKey;
				return;
			}

			pMapData->clear();
			if (!value.empty() && key.size() == value.size())
			{
				for (int i = 0; i < key.size(); ++i)
				{
					(*pMapData)[key[i]] = value[i];
				}
			}
		}
	} while (false);

	std::string strRangeAck;
	if (!xRangeAck.SerializeToString(&strRangeAck))
	{
		QLOG_WARING << __FUNCTION__ << "RankListRange SerializeToString failed ";
		return;
	}

	xMsgBase.set_data(std::move(strRangeAck));
	std::string strMsgBase;
	if (!xMsgBase.SerializeToString(&strMsgBase))
	{
		QLOG_WARING << __FUNCTION__ << "ExtraMsg SerializeToString failed";
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_DB_RETURN_RANK_LIST, strMsgBase, nSockIndex);
}

