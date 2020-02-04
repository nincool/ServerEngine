///---------------------------------------------------------
/// 文件名: LetterRedis.cpp
/// 内 容:  
/// 说 明:       
/// 创建日期: 2019/08/27
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------
#include "LetterRedis.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"

bool CLetterRedis::Init()
{
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pDBServer = pPluginManager->FindModule<CDBServer>();
	m_pColdData = pPluginManager->FindModule<CColdData>();

	return true;
}

bool CLetterRedis::AfterInit()
{
	//信件
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SEND_LETTER, this, (NetMsgFun)&CLetterRedis::OnSendLetterProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_LOOK_LETTER, this, (NetMsgFun)&CLetterRedis::OnLookLetterProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_DELETE_LETTER, this, (NetMsgFun)&CLetterRedis::OnDeleteLetterProcess);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_QUERY_LETTER, this, (NetMsgFun)&CLetterRedis::OnQueryLetterProcess);
	
	// Letter 冷数据处理
	if (!m_pDBServer->GetMysqlConnectColdData().empty())
	{
		m_pColdData->AddScanInfo(m_pDBServer->GetMysqlNameColdData(), ECON_TYPE_DATA, m_pCommonRedis->GetLetterCacheFlag());
	}

	return true;
}


bool CLetterRedis::SendLetter(const NFGUID& self, const OuterMsg::LetterDataPack& xLetterDataPack, const int nExpireSecond/* = 0*/)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetLetterCacheKey(self);
	string_vector fileds;
	string_vector values;

	for (int i = 0; i < xLetterDataPack.letter_size(); ++i)
	{
		auto xLetter = xLetterDataPack.letter(i);
		std::string value;
		if (!xLetter.SerializeToString(&value))
		{
			continue;
		}

		fileds.push_back(xLetter.ID());
		values.push_back(value);
	}

	if (!pDriver->HMSET(strKey, fileds, values))
	{
		return false;
	}

	if (nExpireSecond > 0)
	{
		pDriver->EXPIRE(strKey, nExpireSecond);
	}

	return true;
}

bool CLetterRedis::LookLetter(const NFGUID& self, const int num, OuterMsg::LetterDataPack& xLetterDataPack, bool bDelete/* = true*/)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strCacheKey = m_pCommonRedis->GetLetterCacheKey(self);

	string_vector fields;
	if (!pDriver->HKEYS(strCacheKey, fields))
	{
		return false;
	}

	// 过滤时间戳
	auto it = std::find(fields.begin(), fields.end(), m_pCommonRedis->GetUpdateTime());
	if (it != fields.end())
	{
		fields.erase(it);
	}

	// 数量 num=0时全部取出来
	if (num > 0 && fields.size() > num)
	{
		fields.erase(fields.begin() + num, fields.end());
	}

	string_vector values;
	if (!pDriver->HMGET(strCacheKey, fields, values))
	{
		return false;
	}
	
	// 获取信件信息
	for (auto& it : values)
	{
		OuterMsg::LetterData* pLetter = xLetterDataPack.add_letter();
		if (pLetter != nullptr)
		{
			pLetter->ParseFromString(it);
		}
	}

	// 删除信件
	if (bDelete && !pDriver->HDEL(strCacheKey, fields))
	{
		return false;
	}

	return true;
}

bool CLetterRedis::DeleteLetter(const NFGUID& self, const std::string& strLetterID)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetLetterCacheKey(self);
	// 全部删除
	if (strLetterID.empty())
	{
		if (!pDriver->DEL(strKey))
		{
			return false;
		}
	}
	else // 指定删除
	{
		if (!pDriver->HDEL(strKey, strLetterID))
		{
			return false;
		}
	}

	return true;
}

bool CLetterRedis::QueryLetter(const NFGUID& self, OuterMsg::AckQueryLetter& xQueryLetter)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return false;
	}

	NF_SHARE_PTR<IRedisDriver> pDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_DATA);
	if (!pDriver)
	{
		QLOG_ERROR << __FUNCTION__ << " pDriver == NULL";
		return false;
	}

	std::string strKey = m_pCommonRedis->GetLetterCacheKey(self);
	int num = 0;
	if (!pDriver->HLEN(strKey, num) || num <= 0)
	{
		return false;
	}

	xQueryLetter.set_num(num);

	return true;
}

void CLetterRedis::OnSendLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::LetterDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " LetterDataPack ParseFromArray failed";
		return;
	}

	NFGUID self;
	PBConvert::ToNF(xMsg.self(), self);
	SendLetter(self, xMsg);
}

void CLetterRedis::OnLookLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqLookLetter xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqLookLetter ParseFromArray failed";
		return;
	}

	OuterMsg::LetterDataPack xData;
	NFGUID self;
	PBConvert::ToNF(xMsg.self(), self);
	*xData.mutable_self() = xMsg.self();
	LookLetter(self, xMsg.num(), xData);

	// 回复
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_LOOK_LETTER, xData, nSockIndex);
}

void CLetterRedis::OnDeleteLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqDeleteLetter xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqDeleteLetter ParseFromArray failed";
		return;
	}

	NFGUID self;
	PBConvert::ToNF(xMsg.self(), self);
	DeleteLetter(self, xMsg.letterID());
}

void CLetterRedis::OnQueryLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	OuterMsg::AckQueryLetter xData;
	*xData.mutable_self() = xMsg;
	NFGUID self;
	PBConvert::ToNF(xMsg, self);
	QueryLetter(self, xData);

	// 回复
	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_QUERY_LETTER, xData, nSockIndex);
}