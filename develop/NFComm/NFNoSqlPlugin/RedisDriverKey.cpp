///--------------------------------------------------------------------
/// 文件名:		RedisDriver.h
/// 内  容:		Redis控制驱动程序
/// 说  明:		于管理 redis 的键
/// 创建日期:	2019年10月17日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "RedisDriver.h"


//该命令用于在 key 存在时删除 key
bool RedisDriver::DEL(const std::string& key)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "DEL"
	     << key;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return false;
	}

	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(1))
	{
		std::ostringstream os;
		os << "[RedisDriver::DEL performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

//检查给定 key 是否存在
bool RedisDriver::EXISTS(const std::string& key)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "EXISTS"
	     << key;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return false;
	}

	bool r = (bool)pReply->integer;
	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(1))
	{
		std::ostringstream os;
		os << "[RedisDriver::EXISTS performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/
	
	return r;
}

//为给定 key 设置生存时间，当 key 过期时(生存时间为 0 )，它会被自动删除。
bool RedisDriver::EXPIRE(const std::string& key, const unsigned int secs)
{
	mCom.Reset();
	mCom << "EXPIRE"
	     << key
		 << secs;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return false;
	}

	bool r = (bool)pReply->integer;
	freeReplyObject(pReply);

	return r;
}

//的作用和 EXPIRE 类似，都用于为 key 设置过期时间。 不同在于 EXPIREAT 命令接受的时间参数是 UNIX 时间戳(unix timestamp)
bool RedisDriver::EXPIREAT(const std::string& key, const int64_t unixTime)
{
	mCom.Reset();
	mCom << "EXPIREAT"
	     << key
		 << unixTime;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return false;
	}

	bool r = (bool)pReply->integer;
	freeReplyObject(pReply);

	return r;
}

//移除 key 的过期时间，key 将持久保持。
bool RedisDriver::PERSIST(const std::string& key)
{
	mCom.Reset();
	mCom << "PERSIST"
	     << key;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_INTEGER)
	{
		freeReplyObject(pReply);
		return false;
	}

	bool r = (bool)pReply->integer;
	freeReplyObject(pReply);

	return r;
}

/**
以秒为单位，返回给定 key 的剩余生存时间(TTL, time to live)。
当 key 不存在时，返回 -2 。
当 key 存在但没有设置剩余生存时间时，返回 - 1 。
否则，以秒为单位，返回 key 的剩余生存时间。
*/
int RedisDriver::TTL(const std::string& key)
{
	mCom.Reset();
	mCom << "TTL"
	     << key;
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
返回 key 所储存的值的类型
none(key不存在), string(字符串), list(列表), set(集合), set(有序集), hash(哈希表)
*/
std::string RedisDriver::TYPE(const std::string& key)
{
	mCom.Reset();
	mCom << "TYPE"
	     << key;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return "";
	}

	if (pReply->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(pReply);
		return "";
	}

	std::string type_name(pReply->str, pReply->len);
	freeReplyObject(pReply);

	return type_name;
}

int RedisDriver::SCAN(int it,
	std::vector<std::string>& output,
	const std::string& match /*= ""*/,
	int count /*= 0*/)
{
	mCom.Reset();
	mCom << "SCAN"
		<< it;
	if (count > 0)
	{
		mCom << "COUNT"
			<< count;
	}
	if (!match.empty())
	{
		mCom << "MATCH"
			<< match;
	}

	it = -1;

	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return it;
	}

	if (pReply->type != REDIS_REPLY_ARRAY)
	{
		freeReplyObject(pReply);
		return it;
	}

	if (pReply->elements < 2)
	{
		freeReplyObject(pReply);
		return it;
	}

	if (pReply->element[0]->type != REDIS_REPLY_STRING)
	{
		return it;
	}
	it = ::lexical_cast<int>(pReply->element[0]->str);

	auto& arr = pReply->element[1];
	if (arr->type != REDIS_REPLY_ARRAY)
	{
		return it;
	}

	for (size_t i = 0; i < arr->elements; ++i)
	{
		if (arr->type == REDIS_REPLY_ARRAY)
		{
			output.emplace_back(std::move(std::string(arr->element[i]->str, arr->element[i]->len)));
		}
	}

	return it;
}

bool RedisDriver::SET(const std::string& key, const std::string& value)
{
	mCom.Reset();
	mCom << "SET"
		<< key
		<< value;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return "";
	}

	if (pReply->type != REDIS_REPLY_STATUS || ::strcmp(pReply->str, "OK") != 0)
	{
		freeReplyObject(pReply);
		return false;
	}

	freeReplyObject(pReply);

	return true;
}

bool RedisDriver::GET(const std::string& key, std::string& value)
{
	mCom.Reset();
	mCom << "GET"
		<< key;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return "";
	}

	if (pReply->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(pReply);
		return false;
	}
	value.clear();
	value.append(pReply->str, pReply->len);
	freeReplyObject(pReply);

	return true;
}
