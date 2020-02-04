///--------------------------------------------------------------------
/// �ļ���:		RedisDriver.h
/// ��  ��:		Redis������������
/// ˵  ��:		�ڹ��� redis �ļ�
/// ��������:	2019��10��17��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "RedisDriver.h"


//������������ key ����ʱɾ�� key
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

	//���ܼ�龯��
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

//������ key �Ƿ����
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

	//���ܼ�龯��
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

//Ϊ���� key ��������ʱ�䣬�� key ����ʱ(����ʱ��Ϊ 0 )�����ᱻ�Զ�ɾ����
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

//�����ú� EXPIRE ���ƣ�������Ϊ key ���ù���ʱ�䡣 ��ͬ���� EXPIREAT ������ܵ�ʱ������� UNIX ʱ���(unix timestamp)
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

//�Ƴ� key �Ĺ���ʱ�䣬key ���־ñ��֡�
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
����Ϊ��λ�����ظ��� key ��ʣ������ʱ��(TTL, time to live)��
�� key ������ʱ������ -2 ��
�� key ���ڵ�û������ʣ������ʱ��ʱ������ - 1 ��
��������Ϊ��λ������ key ��ʣ������ʱ�䡣
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
���� key �������ֵ������
none(key������), string(�ַ���), list(�б�), set(����), set(����), hash(��ϣ��)
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
