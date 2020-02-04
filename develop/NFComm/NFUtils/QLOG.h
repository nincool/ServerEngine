///--------------------------------------------------------------------
/// �ļ���:		QLOG.h
/// ��  ��:		������־
/// ˵  ��:		����ӿ�
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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

// ��־�ȼ�
enum LogLevel
{
	LOG_LEVEL_NONE = 0,
	LOG_LEVEL_SYSTEM,	// ϵͳ��־(����״̬����Ҫǿ���������־)
	LOG_LEVEL_FATAL,	// ���ش���
	LOG_LEVEL_ERROR,	// ����
	LOG_LEVEL_WARING,	// ����
	LOG_LEVEL_INFO,		// ����
	LOG_LEVEL_DEBUG,	// ����
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

// �����ļ�������Ϣ
#define QLOG_DEBUG QWriter(LOG_LEVEL_DEBUG, __FILE__, __LINE__)
#define QLOG_INFO QWriter(LOG_LEVEL_INFO, __FILE__, __LINE__)
#define QLOG_WARING QWriter(LOG_LEVEL_WARING, __FILE__, __LINE__)
#define QLOG_ERROR QWriter(LOG_LEVEL_ERROR, __FILE__, __LINE__)
#define QLOG_FATAL QWriter(LOG_LEVEL_FATAL, __FILE__, __LINE__)
#define QLOG_SYSTEM QWriter(LOG_LEVEL_SYSTEM, __FILE__, __LINE__)

// �����ļ�������Ϣ
#define QLOG_DEBUG_S QWriter(LOG_LEVEL_DEBUG, "", -1)
#define QLOG_INFO_S QWriter(LOG_LEVEL_INFO, "", -1)
#define QLOG_WARING_S QWriter(LOG_LEVEL_WARING, "", -1)
#define QLOG_ERROR_S QWriter(LOG_LEVEL_ERROR, "", -1)
#define QLOG_FATAL_S QWriter(LOG_LEVEL_FATAL, "", -1)
#define QLOG_SYSTEM_S QWriter(LOG_LEVEL_SYSTEM, "", -1)

// �����ļ�������Ϣ ����ӡ����Ļ
#define QLOG_DEBUG_N QWriter(LOG_LEVEL_DEBUG, __FILE__, __LINE__, false)
#define QLOG_INFO_N QWriter(LOG_LEVEL_INFO, __FILE__, __LINE__, false)
#define QLOG_WARING_N QWriter(LOG_LEVEL_WARING, __FILE__, __LINE__, false)
#define QLOG_ERROR_N QWriter(LOG_LEVEL_ERROR, __FILE__, __LINE__, false)
#define QLOG_FATAL_N QWriter(LOG_LEVEL_FATAL, __FILE__, __LINE__, false)
#define QLOG_SYSTEM_N QWriter(LOG_LEVEL_SYSTEM, __FILE__, __LINE__, false)

#endif // __Q_LOG_H__
