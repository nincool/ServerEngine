//--------------------------------------------------------------------
/// �ļ���:		IRedisDriver.h
/// ��  ��:		Redis������������
/// ��������:	2019��10��17��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_IRedisDriver_H__
#define __H_IRedisDriver_H__

#include <string>
#include <vector>

class IRedisDriver
{
public:
	//################################## ��(key) ########################################

	//������������ key ����ʱɾ�� key
	virtual bool DEL(const std::string& key) = 0;

	//������ key �Ƿ����
	virtual bool EXISTS(const std::string& key) = 0;

	//Ϊ���� key ��������ʱ�䣬�� key ����ʱ(����ʱ��Ϊ 0 )�����ᱻ�Զ�ɾ����
	virtual bool EXPIRE(const std::string& key, const unsigned int secs) = 0;

	//�����ú� EXPIRE ���ƣ�������Ϊ key ���ù���ʱ�䡣 ��ͬ���� EXPIREAT ������ܵ�ʱ������� UNIX ʱ���(unix timestamp)
	virtual bool EXPIREAT(const std::string& key, const int64_t unixTime) = 0;

	//�Ƴ� key �Ĺ���ʱ�䣬key ���־ñ��֡�
	virtual bool PERSIST(const std::string& key) = 0;

	/**
	����Ϊ��λ�����ظ��� key ��ʣ������ʱ��(TTL, time to live)��
	�� key ������ʱ������ -2 ��
	�� key ���ڵ�û������ʣ������ʱ��ʱ������ - 1 ��
	��������Ϊ��λ������ key ��ʣ������ʱ�䡣
	*/
	virtual int TTL(const std::string& key) = 0;

	/**
	���� key �������ֵ������
	none(key������), string(�ַ���), list(�б�), set(����), set(����), hash(��ϣ��)
	*/
	virtual std::string TYPE(const std::string& key) = 0;

	/**
	����������
	���� ������
	it ������ 0 ��ʼ�µĵ���
	match ģ��ƥ��
	count ÿ�η���������� 0ʹ��Ĭ��
	*/
	virtual int SCAN(int it, std::vector<std::string>& output, const std::string& match = "", int count = 0) = 0;

	//################################## Hash����ϣ��########################################

	/**
	ɾ����ϣ�� key �е�һ������ָ���򣬲����ڵ��򽫱�����
	���ɹ��Ƴ�����������������������Ե���
	*/
	virtual int HDEL(const std::string& key, const std::string& field) = 0;
	/**
	ɾ����ϣ�� key �е�һ������ָ���򣬲����ڵ��򽫱�����
	���ɹ��Ƴ�����������������������Ե���
	*/
	virtual int HDEL(const std::string& key, const std::vector<std::string>& fields) = 0;

	/**
	�鿴��ϣ�� key �У������� field �Ƿ���ڡ�
	�����ϣ���и����򣬷��� 1 ��
	�����ϣ�����и����򣬻� key �����ڣ����� 0
	*/
	virtual bool HEXISTS(const std::string& key, const std::string& field) = 0;

	/**
	���ع�ϣ�� key �и����� field ��ֵ��
	�������򲻴��ڻ��Ǹ��� key ������ʱ������ nil
	*/
	virtual bool HGET(const std::string& key, const std::string& field, std::string& value) = 0;

	/**
	���ع�ϣ�� key �У����е����ֵ
	�� key �����ڣ����ؿ��б�
	*/
	virtual bool HGETALL(const std::string& key, std::vector<std::pair<std::string, std::string>>& values) = 0;

	/**
	* ���ع�ϣ�� key �е�������
	* �� key ������ʱ������һ���ձ�
	*/
	virtual bool HKEYS(const std::string& key, std::vector<std::string>& fields) = 0;

	/**
	���ع�ϣ�� key ���������
	�� key ������ʱ������ 0
	*/
	virtual bool HLEN(const std::string& key, int& number) = 0;

	/**
	���ع�ϣ�� key �У�һ�������������ֵ
	*/
	virtual bool HMGET(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& values) = 0;

	/**
	ͬʱ����� field-value (��-ֵ)�����õ���ϣ�� key ��
	*/
	virtual bool HMSET(const std::string& key, const std::vector<std::pair<std::string, std::string>>& values) = 0;

	/**
	ͬʱ����� field-value (��-ֵ)�����õ���ϣ�� key ��
	*/
	virtual bool HMSET(const std::string& key, const std::vector<std::string>& fields, const std::vector<std::string>& values) = 0;

	/**
	����ϣ�� key �е��� field ��ֵ��Ϊ value
	*/
	virtual bool HSET(const std::string& key, const std::string& field, const std::string& value) = 0;

//################################## Set�����ϣ�########################################
	/**
	�򼯺����һ����Ա
	*/
	virtual int SADD(const std::string& key, const std::string& member) = 0;

	/**
	�Ƴ�������һ����Ա
	*/
	virtual int SREM(const std::string& key, const std::vector<std::string>& members) = 0;

	/**
	���ؼ����е����г�Ա
	*/
	virtual bool SMEMBERS(const std::string& key, std::vector<std::string>& output) = 0;

//################################## �ַ��� ########################################
	/**
	�����ַ���
	*/
	virtual bool SET(const std::string& key, const std::string& value) = 0;
	/**
	��ȡ�ַ���
	*/
	virtual bool GET(const std::string& key, std::string& value) = 0;
};

#endif // __H_IRedisDriver_H__