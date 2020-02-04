///--------------------------------------------------------------------
/// 文件名:		QuickLog.h
/// 内  容:		极速日志
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __QUICK_LOG_H__
#define __QUICK_LOG_H__

#include "NFComm/NFPluginModule/NFPlatform.h"
#include <list>
#include <string>
#include <sstream>
#include "LogShareMem.h"
#include "QLOG.h"

class QUICK_LOG_EXPORT QuickLog final
{
private:
	QuickLog() {};
	QuickLog(const QuickLog&) = delete;
	QuickLog& operator=(const QuickLog&) = delete;
private:

	enum
	{
		// 日志缓存最大值(日志最大)
		LOG_CACHE_MAX_SIZE = 204800,
	};

	const std::string m_strLogLevel[LOG_LEVEL_ALL] =
	{
		"NONE",
		"SYSTEM",
		"FATAL",
		"ERROR",
		"WARING",
		"INFO",
		"DEBUG",
	};

public:

	// 日志等级转日志等级字符串
	const char* GetLogLevelName(LogLevel eLevel);
	// 日志等级字符串转日志等级
	LogLevel GetLogLevel(const char* szLevel);

public:
	// 获取日志对象
	static QuickLog& GetRef();
	// 开启共享日志
	bool StartShareMem(int nGroupId, const std::string& strPrefix);
	const char* GetLogServerProfix();
	// 设置最大日志等级
	bool SetMaxLevel(LogLevel eLevel);
	// 获取最大日志等级
	LogLevel GetMaxLevel();
	// 设置屏幕输出最大日志等级
	bool SetEchoMaxLevel(LogLevel eLevel);
	// 获取屏幕输出最大日志等级
	LogLevel GetEchoMaxLevel();
	// 帧循环
	bool LogExcute();
	// 获取字符流对象
	std::ostringstream& GetStream();
	// 获取日志缓存大小
	int GetLogCacheSize();
	// 开启下次System日志颜色为绿色
	void OpenNextSystemEchoGreen();
	// 打印日志
	bool Log(LogLevel eLevel, int64_t dnTime, const char* szFile, int nLine, std::string& strBuffer);
	bool Echo(LogLevel eLevel, int64_t dnTime, const char* szFile, int nLine, std::string& strBuffer);
private:

#ifndef NF_DEBUG_MODE
	LogLevel m_eLogLevelMax = LOG_LEVEL_INFO;
	LogLevel m_eEchoLogLevelMax = LOG_LEVEL_ERROR;
#else
	LogLevel m_eLogLevelMax = LOG_LEVEL_ALL;
	LogLevel m_eEchoLogLevelMax = LOG_LEVEL_ALL;
#endif

	CLogShareMem m_sharelog;
	
	std::list<std::tuple<LogLevel, int64_t, const char*, int, std::string>> m_listLogCache;
	std::ostringstream m_strStream;

	static QuickLog m_inst;

	bool m_bEchoGreen = false;
};

#endif // __QUICK_LOG_H__
