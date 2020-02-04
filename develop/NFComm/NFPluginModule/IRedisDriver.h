//--------------------------------------------------------------------
/// 文件名:		IRedisDriver.h
/// 内  容:		Redis控制驱动程序
/// 创建日期:	2019年10月17日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_IRedisDriver_H__
#define __H_IRedisDriver_H__

#include <string>
#include <vector>

class IRedisDriver
{
public:
	//################################## 键(key) ########################################

	//该命令用于在 key 存在时删除 key
	virtual bool DEL(const std::string& key) = 0;

	//检查给定 key 是否存在
	virtual bool EXISTS(const std::string& key) = 0;

	//为给定 key 设置生存时间，当 key 过期时(生存时间为 0 )，它会被自动删除。
	virtual bool EXPIRE(const std::string& key, const unsigned int secs) = 0;

	//的作用和 EXPIRE 类似，都用于为 key 设置过期时间。 不同在于 EXPIREAT 命令接受的时间参数是 UNIX 时间戳(unix timestamp)
	virtual bool EXPIREAT(const std::string& key, const int64_t unixTime) = 0;

	//移除 key 的过期时间，key 将持久保持。
	virtual bool PERSIST(const std::string& key) = 0;

	/**
	以秒为单位，返回给定 key 的剩余生存时间(TTL, time to live)。
	当 key 不存在时，返回 -2 。
	当 key 存在但没有设置剩余生存时间时，返回 - 1 。
	否则，以秒为单位，返回 key 的剩余生存时间。
	*/
	virtual int TTL(const std::string& key) = 0;

	/**
	返回 key 所储存的值的类型
	none(key不存在), string(字符串), list(列表), set(集合), set(有序集), hash(哈希表)
	*/
	virtual std::string TYPE(const std::string& key) = 0;

	/**
	迭代遍历键
	返回 迭代器
	it 迭代器 0 开始新的迭代
	match 模糊匹配
	count 每次返回最大数量 0使用默认
	*/
	virtual int SCAN(int it, std::vector<std::string>& output, const std::string& match = "", int count = 0) = 0;

	//################################## Hash（哈希表）########################################

	/**
	删除哈希表 key 中的一个或多个指定域，不存在的域将被忽略
	被成功移除的域的数量，不包括被忽略的域
	*/
	virtual int HDEL(const std::string& key, const std::string& field) = 0;
	/**
	删除哈希表 key 中的一个或多个指定域，不存在的域将被忽略
	被成功移除的域的数量，不包括被忽略的域
	*/
	virtual int HDEL(const std::string& key, const std::vector<std::string>& fields) = 0;

	/**
	查看哈希表 key 中，给定域 field 是否存在。
	如果哈希表含有给定域，返回 1 。
	如果哈希表不含有给定域，或 key 不存在，返回 0
	*/
	virtual bool HEXISTS(const std::string& key, const std::string& field) = 0;

	/**
	返回哈希表 key 中给定域 field 的值。
	当给定域不存在或是给定 key 不存在时，返回 nil
	*/
	virtual bool HGET(const std::string& key, const std::string& field, std::string& value) = 0;

	/**
	返回哈希表 key 中，所有的域和值
	若 key 不存在，返回空列表。
	*/
	virtual bool HGETALL(const std::string& key, std::vector<std::pair<std::string, std::string>>& values) = 0;

	/**
	* 返回哈希表 key 中的所有域。
	* 当 key 不存在时，返回一个空表
	*/
	virtual bool HKEYS(const std::string& key, std::vector<std::string>& fields) = 0;

	/**
	返回哈希表 key 中域的数量
	当 key 不存在时，返回 0
	*/
	virtual bool HLEN(const std::string& key, int& number) = 0;

	/**
	返回哈希表 key 中，一个或多个给定域的值
	*/
	virtual bool HMGET(const std::string& key, const std::vector<std::string>& fields, std::vector<std::string>& values) = 0;

	/**
	同时将多个 field-value (域-值)对设置到哈希表 key 中
	*/
	virtual bool HMSET(const std::string& key, const std::vector<std::pair<std::string, std::string>>& values) = 0;

	/**
	同时将多个 field-value (域-值)对设置到哈希表 key 中
	*/
	virtual bool HMSET(const std::string& key, const std::vector<std::string>& fields, const std::vector<std::string>& values) = 0;

	/**
	将哈希表 key 中的域 field 的值设为 value
	*/
	virtual bool HSET(const std::string& key, const std::string& field, const std::string& value) = 0;

//################################## Set（集合）########################################
	/**
	向集合添加一个成员
	*/
	virtual int SADD(const std::string& key, const std::string& member) = 0;

	/**
	移除集合中一个成员
	*/
	virtual int SREM(const std::string& key, const std::vector<std::string>& members) = 0;

	/**
	返回集合中的所有成员
	*/
	virtual bool SMEMBERS(const std::string& key, std::vector<std::string>& output) = 0;

//################################## 字符串 ########################################
	/**
	设置字符串
	*/
	virtual bool SET(const std::string& key, const std::string& value) = 0;
	/**
	获取字符串
	*/
	virtual bool GET(const std::string& key, std::string& value) = 0;
};

#endif // __H_IRedisDriver_H__