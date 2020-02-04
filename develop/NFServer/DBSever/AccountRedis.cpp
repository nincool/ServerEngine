///--------------------------------------------------------------------
/// 文件名:		AccountRedis.cpp
/// 内  容:		账号数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "AccountRedis.h"

bool CAccountRedis::Init()
{
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pCommonRedis = pPluginManager->FindModule<ICommonRedis>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CAccountRedis::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_VERIFY_ACCOUNT, this, (NetMsgFun)&CAccountRedis::OnVerifyAccount);

	return true;
}

bool CAccountRedis::VerifyAccount(const std::string& strAccount, const std::string& strPwd)
{
	if (strAccount.empty() || strPwd.empty())
	{
		QLOG_WARING << __FUNCTION__ << " para is empty";
		return false;
	}

	std::string strAccountKey = m_pCommonRedis->GetPasswordCacheKey(strAccount);
	NF_SHARE_PTR<IRedisDriver> xNoSqlDriver = m_pNoSqlModule->GetRedisDriver(ECON_TYPE_ACCOUNT);
	if (xNoSqlDriver)
	{
		std::string strPassword;
		if (xNoSqlDriver->GET(strAccountKey, strPassword) && strPassword == strPwd)
		{
			return true;
		}
	}

	return false;
}

void CAccountRedis::OnVerifyAccount(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ReqAccountLogin xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ReqAccountLogin ParseFromArray failed";
		return;
	}

	OuterMsg::MsgReply reply;
	reply.set_extra_data(std::string(msg, nLen));

	if (!VerifyAccount(xMsg.account(), xMsg.password()))
	{
		reply.set_ret_code(OuterMsg::EGEC_ACCOUNTPWD_INVALID);
	}
	else
	{
		reply.set_ret_code(OuterMsg::EGEC_SUCCESS);
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::CS_VERIFY_ACCOUNT, reply, nSockIndex);
}