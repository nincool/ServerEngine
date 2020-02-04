///--------------------------------------------------------------------
/// 文件名:		QLOG.h
/// 内  容:		极速日志
/// 说  明:		对外接口
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __Q_LOG_H__
#define __Q_LOG_H__

#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFGUID.h"

#ifndef QUICK_LOG_EXPORT
#ifdef QUICK_LOG_DLL
#define QUICK_LOG_EXPORT __declspec(dllexport)
#else
#define QUICK_LOG_EXPORT __declspec(dllimport)
#endif // QUICK_LOG_DLL
#endif // QUICK_LOG_EXPORT

// 日志等级
enum LogLevel
{
	LOG_LEVEL_NONE = 0,
	LOG_LEVEL_SYSTEM,	// 系统日志(流程状态等需要强制输出的日志)
	LOG_LEVEL_FATAL,	// 严重错误
	LOG_LEVEL_ERROR,	// 错误
	LOG_LEVEL_WARING,	// 警告
	LOG_LEVEL_INFO,		// 正常
	LOG_LEVEL_DEBUG,	// 调试
	LOG_LEVEL_ALL,
};

class QUICK_LOG_EXPORT QWriter final
{
private:
	QWriter(const QWriter&) = delete;
	QWriter& operator=(const QWriter&) = delete;
public:
	QWriter(LogLevel eLevel, const char* szFile, int nLine, bool bEcho = true);
	~QWriter();

	template<class T>
	QWriter& operator << (const T& t)
	{
		if (m_eLevel <= m_eLogLevelMax)
		{
			m_strStream << t;
		}
		return *this;
	}

	QWriter& operator << (char* szBuffer);
	QWriter& operator << (const char* szBuffer);
	QWriter& operator << (const NFGUID& self);

private:
	std::ostringstream& m_strStream;
	LogLevel m_eLevel;
	LogLevel m_eLogLevelMax;
	LogLevel m_eEchoLevelMax;
	const char* m_szFile;
	int m_nLine;
};

// 带有文件行数信息
#define QLOG_DEBUG QWriter(LOG_LEVEL_DEBUG, __FILE__, __LINE__)
#define QLOG_INFO QWriter(LOG_LEVEL_INFO, __FILE__, __LINE__)
#define QLOG_WARING QWriter(LOG_LEVEL_WARING, __FILE__, __LINE__)
#define QLOG_ERROR QWriter(LOG_LEVEL_ERROR, __FILE__, __LINE__)
#define QLOG_FATAL QWriter(LOG_LEVEL_FATAL, __FILE__, __LINE__)
#define QLOG_SYSTEM QWriter(LOG_LEVEL_SYSTEM, __FILE__, __LINE__)

// 不带文件行数信息
#define QLOG_DEBUG_S QWriter(LOG_LEVEL_DEBUG, "", -1)
#define QLOG_INFO_S QWriter(LOG_LEVEL_INFO, "", -1)
#define QLOG_WARING_S QWriter(LOG_LEVEL_WARING, "", -1)
#define QLOG_ERROR_S QWriter(LOG_LEVEL_ERROR, "", -1)
#define QLOG_FATAL_S QWriter(LOG_LEVEL_FATAL, "", -1)
#define QLOG_SYSTEM_S QWriter(LOG_LEVEL_SYSTEM, "", -1)

// 带有文件行数信息 不打印到屏幕
#define QLOG_DEBUG_N QWriter(LOG_LEVEL_DEBUG, __FILE__, __LINE__, false)
#define QLOG_INFO_N QWriter(LOG_LEVEL_INFO, __FILE__, __LINE__, false)
#define QLOG_WARING_N QWriter(LOG_LEVEL_WARING, __FILE__, __LINE__, false)
#define QLOG_ERROR_N QWriter(LOG_LEVEL_ERROR, __FILE__, __LINE__, false)
#define QLOG_FATAL_N QWriter(LOG_LEVEL_FATAL, __FILE__, __LINE__, false)
#define QLOG_SYSTEM_N QWriter(LOG_LEVEL_SYSTEM, __FILE__, __LINE__, false)

#endif // __Q_LOG_H__
