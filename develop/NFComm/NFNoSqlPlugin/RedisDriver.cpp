///--------------------------------------------------------------------
/// �ļ���:		RedisDriver.h
/// ��  ��:		Redis������������
/// ˵  ��:		
/// ��������:	2019��10��17��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
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

//������
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

//������
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

//ȡ�õ�ַ
void RedisDriver::GetAddress(std::string& ip, int& port)
{
	ip = mIP;
	port = mPort;
}

//��ǰ�Ƿ����
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

//֡ѭ��
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

//ȡ�ô�����
int RedisDriver::GetErrorCode()
{
	if (nullptr == m_pContext)
	{
		return 0;
	}

	return m_pContext->err;
}

//ȡ��˵��
const char* RedisDriver::GetErrorStr()
{
	if (nullptr == m_pContext)
	{
		return "";
	}

	return m_pContext->errstr;
}

//ִ������
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


//���ڼ�����������������ļ��е������Ƿ����
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

//�л���ָ�������ݿ�
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
