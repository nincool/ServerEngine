///--------------------------------------------------------------------
/// �ļ���:		RedisCommand.h
/// ��  ��:		Redis������װ
/// ˵  ��:		
/// ��������:	2019��10��17��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_RedisCommand_H__
#define __H_RedisCommand_H__

#include <iostream>
#include <vector>
#include <sstream>

class RedisCommand
{
public:
	RedisCommand()
	{
	}

	~RedisCommand()
	{
		Reset();
	}

	void Reset()
	{
		mxParam.clear();
		outStr.clear();
	}

	template <typename T>
	RedisCommand& operator<<(const T& t)
	{
		mxParam.push_back(std::to_string(t));
		return *this;
	}

	RedisCommand& operator<<(const std::string& value)
	{
		mxParam.push_back(value);
		return *this;
	}

	RedisCommand& operator<<(const char* value)
	{
		mxParam.push_back(value);
		return *this;
	}

	const char* GetCStr()
	{
		if (outStr.empty())
		{
			Compile();
		}

		return outStr.c_str();
	}

	const size_t GetLength()
	{
		if (outStr.empty())
		{
			Compile();
		}

		return outStr.length();
	}

private:
	bool Compile()
	{
		if (mxParam.empty())
		{
			return false;
		}
		outStr.clear();
		outStr += '*';
		outStr.append(std::to_string(mxParam.size()));
		outStr.append("\r\n");
		std::vector<std::string>::iterator it = mxParam.begin();
		for (; it != mxParam.end(); ++it)
		{
			outStr += '$';
			outStr.append(std::to_string(it->size()));
			outStr.append("\r\n");
			outStr.append(*it);
			outStr.append("\r\n");
		}

		return true;
	}

private:
	std::vector<std::string> mxParam;
	std::string outStr;
};

#endif // __H_RedisCommand_H__