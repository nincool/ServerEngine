///--------------------------------------------------------------------
/// �ļ���:		RedisDriver.h
/// ��  ��:		Redis������������ Hash����ϣ��
/// ˵  ��:		
/// ��������:	2019��10��17��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "RedisDriver.h"

/**
ɾ����ϣ�� key �е�һ������ָ���򣬲����ڵ��򽫱�����
���ɹ��Ƴ�����������������������Ե���
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

	//���ܼ�龯��
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
ɾ����ϣ�� key �е�һ������ָ���򣬲����ڵ��򽫱�����
���ɹ��Ƴ�����������������������Ե���
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

	//���ܼ�龯��
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
�鿴��ϣ�� key �У������� field �Ƿ���ڡ�
�����ϣ���и����򣬷��� 1 ��
�����ϣ�����и����򣬻� key �����ڣ����� 0
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

	//���ܼ�龯��
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
���ع�ϣ�� key �и����� field ��ֵ��
�������򲻴��ڻ��Ǹ��� key ������ʱ������ nil
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

	//���ܼ�龯��
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
���ع�ϣ�� key �У����е����ֵ
�� key �����ڣ����ؿ��б�
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

	//���ܼ�龯��
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
���ع�ϣ�� key �е�������
�� key ������ʱ������һ���ձ�
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

	//���ܼ�龯��
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
���ع�ϣ�� key ���������
�� key ������ʱ������ 0
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

	//���ܼ�龯��
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
���ع�ϣ�� key �У�һ�������������ֵ
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
			// Ϊ�˺���ȡ����ƥ��,���
			values.push_back("");
		}
	}

	freeReplyObject(pReply);

	//���ܼ�龯��
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
ͬʱ����� field-value (��-ֵ)�����õ���ϣ�� key ��
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

	//���ܼ�龯��
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
ͬʱ����� field-value (��-ֵ)�����õ���ϣ�� key ��
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

	//���ܼ�龯��
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
����ϣ�� key �е��� field ��ֵ��Ϊ value
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
	//���ܼ�龯��
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