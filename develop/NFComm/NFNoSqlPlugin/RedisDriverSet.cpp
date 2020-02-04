///--------------------------------------------------------------------
/// 文件名:		RedisDriver.h
/// 内  容:		Redis控制驱动程序 Set（集合）
/// 说  明:		
/// 创建日期:	2019年10月17日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "RedisDriver.h"
/**
向集合添加一个成员
*/
int RedisDriver::SADD(const std::string& key, const std::string& member)
{
	mCom.Reset();
	mCom << "SADD"
	     << key
		 << member;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return 0;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return 0;
	}

	int i = (int)pReply->integer;
	freeReplyObject(pReply);

	return i;
}

/**
移除集合中一个成员
*/
int RedisDriver::SREM(const std::string& key, const std::vector<std::string>& members)
{
	mCom.Reset();
	mCom << "SREM"
	     << key;
	auto it = members.begin();
	auto it_end = members.end();
	for (; it != it_end; ++it)
	{
		mCom << (*it);
	}

	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return 0;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return 0;
	}

	int i = (int)pReply->integer;
	freeReplyObject(pReply);

	return i;
}

/**
返回集合中的所有成员
*/
bool RedisDriver::SMEMBERS(const std::string& key, std::vector<std::string>& output)
{
	mCom.Reset();
	mCom << "SMEMBERS"
	     << key;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(pReply);
		return false;
	}

	for (size_t k = 0; k < pReply->elements; k++)
	{
		if (pReply->element[k]->type == REDIS_REPLY_STRING)
		{
			output.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
		}
	}

	freeReplyObject(pReply);

	return true;
}