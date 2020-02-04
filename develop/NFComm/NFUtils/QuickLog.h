///--------------------------------------------------------------------
/// �ļ���:		QuickLog.h
/// ��  ��:		������־
/// ˵  ��:		
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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
		// ��־�������ֵ(��־���)
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

	// ��־�ȼ�ת��־�ȼ��ַ���
	const char* GetLogLevelName(LogLevel eLevel);
	// ��־�ȼ��ַ���ת��־�ȼ�
	LogLevel GetLogLevel(const char* szLevel);

public:
	// ��ȡ��־����
	static QuickLog& GetRef();
	// ����������־
	bool StartShareMem(int nGroupId, const std::string& strPrefix);
	const char* GetLogServerProfix();
	// ���������־�ȼ�
	bool SetMaxLevel(LogLevel eLevel);
	// ��ȡ�����־�ȼ�
	LogLevel GetMaxLevel();
	// ������Ļ��������־�ȼ�
	bool SetEchoMaxLevel(LogLevel eLevel);
	// ��ȡ��Ļ��������־�ȼ�
	LogLevel GetEchoMaxLevel();
	// ֡ѭ��
	bool LogExcute();
	// ��ȡ�ַ�������
	std::ostringstream& GetStream();
	// ��ȡ��־�����С
	int GetLogCacheSize();
	// �����´�System��־��ɫΪ��ɫ
	void OpenNextSystemEchoGreen();
	// ��ӡ��־
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
