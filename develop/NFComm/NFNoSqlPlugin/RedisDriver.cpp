///--------------------------------------------------------------------
/// 文件名:		RedisDriver.h
/// 内  容:		Redis控制驱动程序
/// 说  明:		
/// 创建日期:	2019年10月17日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "RedisDriver.h"
#include <WinSock2.h>
#include <time.h>

RedisDriver::RedisDriver(NFIPluginManager* p)
{
	m_pPluginManager = p;
}

RedisDriver::~RedisDriver()
{
	redisFree(m_pContext);
	m_pContext = nullptr;
}

//新连接
bool RedisDriver::Connect(const std::string& ip, const int port, const std::string& auth)
{
	mAuth = auth;
	mIP = ip;
	mPort = port;

	timeval timeout = {2, 0};
	m_pContext = redisConnectWithTimeout(ip.c_str(), port, timeout);
	if (nullptr == m_pContext || m_pContext->err > 0)
	{
		return false;
	}
	
	mEnable = AUTH(mAuth);

	return mEnable;
}

//重连接
bool RedisDriver::ReConnect()
{
	if (nullptr == m_pContext)
	{
		return false;
	}

	if (REDIS_OK != redisReconnect(m_pContext))
	{
		mEnable = false;
		return false;
	}

	mEnable = AUTH(mAuth);

	return mEnable;
}

//取得地址
void RedisDriver::GetAddress(std::string& ip, int& port)
{
	ip = mIP;
	port = mPort;
}

//当前是否可用
bool RedisDriver::Enable()
{
	if (nullptr == m_pContext || false == mEnable)
	{
		return false;
	}

	if (m_pContext->err != 0)
	{
		mEnable = false;
	}

	return mEnable;
}

//帧循环
bool RedisDriver::Execute()
{
	if (mLastCheckTime + 10 > m_pPluginManager->GetNowTime())
	{
		return true;
	}
	mLastCheckTime = m_pPluginManager->GetNowTime();

	mCom.Reset();
	mCom << "PING";
	RunRedisCommand(mCom);

	if (m_pContext->err != 0)
	{
		mEnable = false;
	}

	return true;
}

//取得错误码
int RedisDriver::GetErrorCode()
{
	if (nullptr == m_pContext)
	{
		return 0;
	}

	return m_pContext->err;
}

//取得说明
const char* RedisDriver::GetErrorStr()
{
	if (nullptr == m_pContext)
	{
		return "";
	}

	return m_pContext->errstr;
}

//执行命令
redisReply* RedisDriver::RunRedisCommand(RedisCommand& com)
{
	if (nullptr == m_pContext)
	{
		return nullptr;
	}

	if (com.GetLength() <= 0)
	{
		return nullptr;
	}

	if (REDIS_OK != redisAppendFormattedCommand(m_pContext, com.GetCStr(), com.GetLength()))
	{
		return nullptr;
	}

	void* reply;
	if (m_pContext->flags & REDIS_BLOCK)
	{
		if (redisGetReply(m_pContext, &reply) != REDIS_OK)
		{
			return nullptr;
		}
			
		return (redisReply*)reply;
	}

	return nullptr;
}


//用于检测给定的密码和配置文件中的密码是否相符
bool RedisDriver::AUTH(const std::string& auth)
{
	mCom.Reset();
	mCom << "AUTH"
		<< auth;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STATUS)
	{
		if (std::string("OK") == std::string(pReply->str, pReply->len) ||
			std::string("ok") == std::string(pReply->str, pReply->len))
		{
			freeReplyObject(pReply);
			return true;
		}
	}

	freeReplyObject(pReply);
	return false;
}

//切换到指定的数据库
bool RedisDriver::Select(int dbnum)
{
	if (nullptr == m_pContext)
	{
		return false;
	}

	mCom.Reset();
	mCom << "SELECT"
		<< dbnum;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type == REDIS_REPLY_STATUS)
	{
		if (std::string("OK") == std::string(pReply->str, pReply->len) ||
			std::string("ok") == std::string(pReply->str, pReply->len))
		{
			freeReplyObject(pReply);
			return true;
		}
	}

	freeReplyObject(pReply);
	return false;
}
