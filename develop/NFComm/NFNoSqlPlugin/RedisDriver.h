///--------------------------------------------------------------------
/// �ļ���:		RedisDriver.h
/// ��  ��:		Redis�����������򣨲����̰߳�ȫ��
/// ˵  ��:		
/// ��������:	2019��10��17��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_RedisDriver_H__
#define __H_RedisDriver_H__
#include <string>
#include <hiredis/hiredis.h>
#include "NFComm/NFPluginModule/IRedisDriver.h"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "RedisCommand.h"
#include "../NFPluginModule/NFIPluginManager.h"

class RedisDriver : public IRedisDriver
{
public:
	RedisDriver(NFIPluginManager* p);
	virtual ~RedisDriver();

	//������
	virtual bool Connect(const std::string& ip, const int port, const std::string& auth);

	//������
	virtual bool ReConnect();

	//ȡ�õ�ַ
	virtual void GetAddress(std::string& ip, int& port);
	
	//��ǰ�Ƿ����
	virtual bool Enable();

	//֡ѭ��
	virtual bool Execute();

	//ȡ�ô�����
	virtual int GetErrorCode();

	//ȡ��˵��
	virtual const char* GetErrorStr();

	//ִ������
	virtual redisReply* RunRedisCommand(RedisCommand& com);

	//���ڼ�����������������ļ��е������Ƿ����
	virtual bool AUTH(const std::string& auth);

	//�л���ָ�������ݿ�
	virtual bool Select(int dbnum);

//################################## ��(key) ########################################

	//������������ key ����ʱɾ�� key
	virtual bool DEL(const std::string& key);

	//������ key �Ƿ����
	virtual bool EXISTS(const std::string& key);

	//Ϊ���� key ��������ʱ�䣬�� key ����ʱ(����ʱ��Ϊ 0 )�����ᱻ�Զ�ɾ����
	virtual bool EXPIRE(const std::string& key, const unsigned int secs);

	//�����ú� EXPIRE ���ƣ�������Ϊ key ���ù���ʱ�䡣 ��ͬ���� EXPIREAT ������ܵ�ʱ������� UNIX ʱ���(unix timestamp)
	virtual bool EXPIREAT(const std::string& key, const int64_t unixTime);

	//�Ƴ� key �Ĺ���ʱ�䣬key ���־ñ��֡�
	virtual bool PERSIST(const std::string& key);

	/**
	����Ϊ��λ�����ظ��� key ��ʣ������ʱ��(TTL, time to live)��
	�� key ������ʱ������ -2 ��
	�� key ���ڵ�û������ʣ������ʱ��ʱ������ - 1 ��
	��������Ϊ��λ������ key ��ʣ������ʱ�䡣
	*/
	virtual int TTL(const std::string& key);

	/**
	���� key �������ֵ������
	none(key������), string(�ַ���), list(�б�), set(����), set(����), hash(��ϣ��)
	*/
	virtual std::string TYPE(const std::string& key);

	/**
	����������
	���� ������
	it ������ 0 ��ʼ�µĵ���
	match ģ��ƥ��
	count ÿ�η���������� 0ʹ��Ĭ��
	*/
	virtual int SCAN(int it, std::vector<std::string>& output, const std::string& match = "", int count = 0);

//################################## Hash����ϣ��########################################

	/**
	ɾ����ϣ�� key �е�һ������ָ���򣬲����ڵ��򽫱�����
	���ɹ��Ƴ�����������������������Ե���
	*/
	virtual int HDEL(const std::string& key, const std::string& field);
	/**
	ɾ����ϣ�� key �е�һ������ָ���򣬲����ڵ��򽫱�����
	���ɹ��Ƴ�����������������������Ե���
	*/
	virtual int HDEL(const std::string& key, const std::vector<std::string>& fields);

	/**
	�鿴��ϣ�� key �У������� field �Ƿ���ڡ�
	�����ϣ���и����򣬷��� 1 ��
	�����ϣ�����и����򣬻� key �����ڣ����� 0 
	*/
	virtual bool HEXISTS(const std::string& key, const std::string& field);

	/**
	���ع�ϣ�� key �и����� field ��ֵ��
	�������򲻴��ڻ��Ǹ��� key ������ʱ������ nil 
	*/
	virtual bool HGET(const std::string& key, const std::string& field, std::string& value);

	/**
	���ع�ϣ�� key �У����е����ֵ
	�� key �����ڣ����ؿ��б�
	*/
	virtual bool HGETALL(const std::string& key, std::vector<std::pair<std::string, std::string>>& values);

	/**
	* ���ع�ϣ�� key �е�������
	* �� key ������ʱ������һ���ձ�
	*/
	virtual bool HKEYS(const std::string& key, std::vector<std::string>& fields);

	/**
	���ع�ϣ�� key ���������
	�� key ������ʱ������ 0
	*/
	virtual bool HLEN(const std::string& key, int& number);

	/**
	���ع�ϣ�� key �У�һ�������������ֵ
	*/
	virtual bool HMGET(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& values);

	/**
	ͬʱ����� field-value (��-ֵ)�����õ���ϣ�� key ��
	*/
	virtual bool HMSET(const std::string& key, const std::vector<std::pair<std::string, std::string>>& values);

	/**
	ͬʱ����� field-value (��-ֵ)�����õ���ϣ�� key ��
	*/
	virtual bool HMSET(const std::string& key, const std::vector<std::string>& fields, const std::vector<std::string>& values);

	/**
	����ϣ�� key �е��� field ��ֵ��Ϊ value 
	*/
	virtual bool HSET(const std::string& key, const std::string& field, const std::string& value);

//################################## Set�����ϣ�########################################
	/**
	�򼯺����һ����Ա
	*/
	virtual int SADD(const std::string& key, const std::string& member);

	/**
	�Ƴ�������һ����Ա
	*/
	virtual int SREM(const std::string& key, const std::vector<std::string>& members);

	/**
	���ؼ����е����г�Ա
	*/
	virtual bool SMEMBERS(const std::string& key, std::vector<std::string>& output);

//################################## �ַ��� ########################################
	/**
	�����ַ���
	*/
	virtual bool SET(const std::string& key, const std::string& value);
	/**
	��ȡ�ַ���
	*/
	virtual bool GET(const std::string& key, std::string& value);

private:
	NFIPluginManager* m_pPluginManager = nullptr;
	redisContext* m_pContext = nullptr;

	RedisCommand mCom;
	NFINT64 mLastCheckTime = 0;
	bool mEnable = false;
	std::string mAuth;
	std::string mIP;
	int mPort = 0;
};

#endif // __H_RedisDriver_H__