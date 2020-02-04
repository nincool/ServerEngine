///--------------------------------------------------------------------
/// 文件名:		RedisDriver.h
/// 内  容:		Redis控制驱动程序 Hash（哈希表）
/// 说  明:		
/// 创建日期:	2019年10月17日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "RedisDriver.h"

/**
删除哈希表 key 中的一个或多个指定域，不存在的域将被忽略
被成功移除的域的数量，不包括被忽略的域
*/
int RedisDriver::HDEL(const std::string& key, const std::string& field)
{
	//NFPerformance performance;
	mCom.Reset();
	mCom << "HDEL"
	     << key
		 << field;
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

	int i = pReply->integer;
	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(1))
	{
		std::ostringstream os;
		os << "[RedisDriver::HDEL performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return i;
}

/**
删除哈希表 key 中的一个或多个指定域，不存在的域将被忽略
被成功移除的域的数量，不包括被忽略的域
*/
int RedisDriver::HDEL(const std::string& key, const std::vector<std::string>& fields)
{
	//NFPerformance performance;

	mCom.Reset();
	mCom << "HDEL"
		 << key;
	auto it = fields.begin();
	auto it_end = fields.end();
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

	int i = pReply->integer;
	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(2))
	{
		std::ostringstream os;
		os << "[RedisDriver::HDEL performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return i;
}

/**
查看哈希表 key 中，给定域 field 是否存在。
如果哈希表含有给定域，返回 1 。
如果哈希表不含有给定域，或 key 不存在，返回 0
*/
bool RedisDriver::HEXISTS(const std::string& key, const std::string& field)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HEXISTS"
	    << key
		<< field;
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

	bool b = (bool)pReply->integer;
	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(1))
	{
		std::ostringstream os;
		os << "[RedisDriver::HEXISTS performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return b;
}

/**
返回哈希表 key 中给定域 field 的值。
当给定域不存在或是给定 key 不存在时，返回 nil
*/
bool RedisDriver::HGET(const std::string& key, const std::string& field, std::string& value)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HGET"
	     << key
		 << field;
	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	if (pReply->type != REDIS_REPLY_STRING)
	{
		freeReplyObject(pReply);
		return false;
	}
	value.clear();
	value.append(pReply->str, pReply->len);
	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(1))
	{
		std::ostringstream os;
		os << "[RedisDriver::HGET performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
返回哈希表 key 中，所有的域和值
若 key 不存在，返回空列表。
*/
bool RedisDriver::HGETALL(const std::string& key, std::vector<std::pair<std::string, std::string>>& values)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HGETALL"
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

	for (int k = 0; k < (int)pReply->elements; k = k + 2)
	{
		values.emplace_back(std::move(std::pair<std::string, std::string>{
			std::string(pReply->element[k]->str, pReply->element[k]->len),
				std::string(pReply->element[k + 1]->str, pReply->element[k + 1]->len)
		}));
	}

	//性能检查警告
	/*if (performance.CheckTimePoint(2))
	{
		std::ostringstream os;
		os << "[RedisDriver::HGETALL performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
返回哈希表 key 中的所有域。
当 key 不存在时，返回一个空表
*/
bool RedisDriver::HKEYS(const std::string& key, std::vector<std::string>& fields)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HKEYS"
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

	for (int k = 0; k < (int)pReply->elements; k++)
	{
		fields.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
	}

	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(2))
	{
		std::ostringstream os;
		os << "[RedisDriver::HKEYS performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
返回哈希表 key 中域的数量
当 key 不存在时，返回 0
*/
bool RedisDriver::HLEN(const std::string& key, int& number)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HLEN"
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
	number = (int)pReply->integer;

	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(1))
	{
		std::ostringstream os;
		os << "[RedisDriver::HLEN performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
返回哈希表 key 中，一个或多个给定域的值
*/
bool RedisDriver::HMGET(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& values)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HMGET"
	     << key;
	auto it = fields.begin();
	auto it_end = fields.end();
	for (; it != it_end; ++it)
	{
		mCom << (*it);
	}

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

	for (int k = 0; k < (int)pReply->elements; k++)
	{
		if (pReply->element[k]->type == REDIS_REPLY_STRING)
		{
			values.emplace_back(std::move(std::string(pReply->element[k]->str, pReply->element[k]->len)));
		}
		else
		{
			// 为了和所取数据匹配,填空
			values.push_back("");
		}
	}

	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(2))
	{
		std::ostringstream os;
		os << "[RedisDriver::HMGET performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
同时将多个 field-value (域-值)对设置到哈希表 key 中
*/
bool RedisDriver::HMSET(const std::string& key, const std::vector<std::pair<std::string, std::string>>& values)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HMSET"
	     << key;
	auto it = values.begin();
	auto it_end = values.end();
	for (; it != it_end; ++it)
	{
		mCom << it->first;
		mCom << it->second;
	}

	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(2))
	{
		std::ostringstream os;
		os << "[RedisDriver::HMSET performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
同时将多个 field-value (域-值)对设置到哈希表 key 中
*/
bool RedisDriver::HMSET(const std::string& key, const std::vector<std::string>& fields, const std::vector<std::string>& values)
{
	/*NFPerformance performance;*/
	if (fields.size() != values.size())
	{
		return false;
	}

	mCom.Reset();
	mCom << "HMSET"
	     << key;
	int size = (int)fields.size();
	for (int i = 0; i < size; ++i)
	{
		mCom << (fields[i]);
		mCom << (values[i]);
	}

	redisReply* pReply = RunRedisCommand(mCom);
	if (nullptr == pReply)
	{
		return false;
	}

	freeReplyObject(pReply);

	//性能检查警告
	/*if (performance.CheckTimePoint(2))
	{
		std::ostringstream os;
		os << "[RedisDriver::HMSET performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}

/**
将哈希表 key 中的域 field 的值设为 value
*/
bool RedisDriver::HSET(const std::string& key, const std::string& field, const std::string& value)
{
	/*NFPerformance performance;*/

	mCom.Reset();
	mCom << "HSET"
	     << key
		 << field
		 << value;
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
		os << "[RedisDriver::HSET performance problem] ";
		os << "consume:";
		os << performance.TimeScope();
		m_pLogModule->LogWarning(os, __FUNCTION__, __LINE__);
	}*/

	return true;
}