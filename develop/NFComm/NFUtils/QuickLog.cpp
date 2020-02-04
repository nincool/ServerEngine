///--------------------------------------------------------------------
/// 文件名:		QuickLog.cpp
/// 内  容:		极速日志
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "QuickLog.h"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "NFComm/NFUtils/termcolor.hpp"
#include "NFComm/NFPluginModule/NFGUID.h"

QuickLog QuickLog::m_inst;

const char* QuickLog::GetLogLevelName(LogLevel eLevel)
{
	if (eLevel < LOG_LEVEL_NONE || LOG_LEVEL_NONE >= LOG_LEVEL_ALL)
	{
		return "";
	}

	return m_strLogLevel[eLevel].c_str();
}

LogLevel QuickLog::GetLogLevel(const char* szLevel)
{
	if (szLevel == nullptr || szLevel[0] == '\0')
	{
		return LOG_LEVEL_NONE;
	}

	for (int i = 0; i < LOG_LEVEL_ALL; ++i)
	{
		if (m_strLogLevel[i] == szLevel)
		{
			return LogLevel(i);
		}
	}

	return LOG_LEVEL_NONE;
}

QuickLog& QuickLog::GetRef()
{
	return m_inst;
}

bool QuickLog::SetMaxLevel(LogLevel eLevel)
{
	m_eLogLevelMax = eLevel;
	return true;
}

LogLevel QuickLog::GetMaxLevel()
{
	return m_eLogLevelMax;
}

bool QuickLog::SetEchoMaxLevel(LogLevel eLevel)
{
	m_eEchoLogLevelMax = eLevel;
	return true;
}
                                                                 
LogLevel QuickLog::GetEchoMaxLevel()
{
	return m_eEchoLogLevelMax;
}

bool QuickLog::LogExcute()
{
	if (!m_sharelog.IsReady())
	{
		return false;
	}

	while (!m_listLogCache.empty())
	{
		auto& it = m_listLogCache.front();

		if (!m_sharelog.LogTrace(GetLogLevelName(std::get<0>(it)),
			std::get<1>(it),
			std::get<2>(it),
			std::get<3>(it),
			std::get<4>(it).c_str()))
		{
			break;
		}

		m_listLogCache.pop_front();
	}

	return m_listLogCache.empty();
}

std::ostringstream& QuickLog::GetStream()
{
	return m_strStream;
}

int QuickLog::GetLogCacheSize()
{
	return m_listLogCache.size();
}

bool QuickLog::Log(LogLevel eLevel, int64_t dnTime, const char* szFile, int nLine, std::string& strBuffer)
{
	if (eLevel > m_eLogLevelMax)
	{
		return false;
	}

	do
	{
		if (!m_sharelog.IsReady())
		{
			break;
		}

		if (!LogExcute())
		{
			break;
		}

		if (!m_sharelog.LogTrace(GetLogLevelName(eLevel), dnTime, szFile, nLine, strBuffer.c_str()))
		{
			break;
		}

		return true;

	} while (false);

	m_listLogCache.push_back(std::move(std::make_tuple(eLevel, dnTime, szFile, nLine, std::move(strBuffer))));

	// 超出最大限制，抛弃早期日志
	if (m_listLogCache.size() > LOG_CACHE_MAX_SIZE)
	{
		m_listLogCache.pop_front();
	}

	return true;
}

bool QuickLog::Echo(LogLevel eLevel, int64_t dnTime, const char* szFile, int nLine, std::string& strBuffer)
{
	if (eLevel > m_eEchoLogLevelMax)
	{
		return false;
	}

	switch (eLevel)
	{
	case LOG_LEVEL_DEBUG:

#ifndef NF_DEBUG_MODE
		// 不是DEBUG模式 不输出DEBUG日志
		return false;
#endif
		std::cout << termcolor::grey;
		break;
	case LOG_LEVEL_INFO:
		std::cout << termcolor::green;
		break;
	case LOG_LEVEL_WARING:
		std::cout << termcolor::yellow;
		break;
	case LOG_LEVEL_ERROR:
		std::cout << termcolor::red;
		break;
	case LOG_LEVEL_FATAL:
		std::cout << termcolor::red;
		break;
	case LOG_LEVEL_SYSTEM:
		if (m_bEchoGreen)
		{
			std::cout << termcolor::green;
			m_bEchoGreen = false;
		}
		else
		{
			std::cout << termcolor::magenta;
		}
		break;
	default:
		std::cout << termcolor::green;
		break;
	}

	std::cout << m_sharelog.LogFormat(GetLogLevelName(eLevel), dnTime, szFile, nLine, strBuffer.c_str()) << std::endl;
	std::cout << termcolor::reset;

	return true;
}

bool QuickLog::StartShareMem(int nGroupId, const std::string& strPrefix)
{
	return m_sharelog.Start(nGroupId, strPrefix);
}

const char* QuickLog::GetLogServerProfix()
{
	return m_sharelog.GetProfix(0);
}

void QuickLog::OpenNextSystemEchoGreen()
{
	m_bEchoGreen = true;
}

QWriter::QWriter(LogLevel eLevel, const char* szFile, int nLine, bool bEcho/* = true*/)
	: m_strStream(QuickLog::GetRef().GetStream())
	, m_eLogLevelMax(QuickLog::GetRef().GetMaxLevel())
	, m_eEchoLevelMax(bEcho ? QuickLog::GetRef().GetEchoMaxLevel() : LOG_LEVEL_NONE)
	, m_eLevel(eLevel)
	, m_szFile(szFile)
	, m_nLine(nLine)
{

#ifndef NF_DEBUG_MODE
	// 不是DEBUG模式 不输出DEBUG日志
	if (m_eLevel == LOG_LEVEL_DEBUG)
	{
		m_eLogLevelMax = LOG_LEVEL_NONE;
	}
#endif

	if (m_eLevel <= m_eLogLevelMax)
	{
		m_strStream.str("");
	}
}

QWriter::~QWriter()
{
	if (m_eLevel <= m_eLogLevelMax)
	{
		QuickLog::GetRef().Log(m_eLevel, NFDateTime::NowTime(), m_szFile, m_nLine, m_strStream.str());
	}

	if (m_eLevel <= m_eEchoLevelMax)
	{
		QuickLog::GetRef().Echo(m_eLevel, NFDateTime::NowTime(), m_szFile, m_nLine, m_strStream.str());
	}
}

QWriter& QWriter::operator << (char* szBuffer)
{
	if (m_eLevel <= m_eLogLevelMax)
	{
		if (szBuffer == nullptr)
		{
			szBuffer = "";
		}
		m_strStream << szBuffer;
	}
	return *this;
}

QWriter& QWriter::operator << (const char* szBuffer)
{
	if (m_eLevel <= m_eLogLevelMax)
	{
		if (szBuffer == nullptr)
		{
			szBuffer = "";
		}
		m_strStream << szBuffer;
	}
	return *this;
}

QWriter& QWriter::operator << (const NFGUID& self)
{
	if (m_eLevel <= m_eLogLevelMax)
	{
		m_strStream << self.ToString();
	}
	return *this;
}
