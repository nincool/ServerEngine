///--------------------------------------------------------------------
/// 文件名:		ExceptMsg.h
/// 内  容:		异常信息
/// 说  明:		
/// 创建日期:	2019.8.20
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PUBLIC_EXCEPTMSG_H
#define _PUBLIC_EXCEPTMSG_H

#include "Debug.h"
#include <stdarg.h>
#include <string>
#include <time.h>

class NFException
{
public:
	NFException();

	~NFException()
	{
	}

	NFException(const char* format, ...)
	{
		char buf[1024] = { 0 };
		va_list args;
		va_start(args, format);
		vsprintf(buf, format, args);
		_msg = buf;
		va_end(args);
	}

	NFException(const std::string& msg)
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char temp[128] = { 0 };
		sprintf(temp, "%d-%02d-%02d %02d:%02d:%02d\n",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);

		_msg.append(temp);
		_msg.append(" NFException ");
		_msg.append(msg);
	}

	NFException(const std::string& msg, const char* file, const int line)
	{
		time_t tt = time(NULL);
		tm* t = localtime(&tt);
		char temp[128] = { 0 };
		sprintf(temp, "%d-%02d-%02d %02d:%02d:%02d\n",
			t->tm_year + 1900,
			t->tm_mon + 1,
			t->tm_mday,
			t->tm_hour,
			t->tm_min,
			t->tm_sec);

		_msg.append(temp);
		_msg.append(" NFException ");
		_msg.append(msg);
		_msg.append(file);
		_msg.append(std::to_string(line));
	}

	char const* what() const noexcept { return _msg.c_str(); }
private:
	std::string _msg;
};

#endif // _PUBLIC_EXCEPTMSG_H
