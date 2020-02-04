///--------------------------------------------------------------------
/// �ļ���:		LogShareMem.h
/// ��  ��:		��־�����ڴ�
/// ˵  ��:		
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __LOG_SHARE_MEM_H__
#define __LOG_SHARE_MEM_H__

#include "NFComm/system/WinShareMem.h"
#include "NFComm/system/WinMutex.h"
#include "NFTool.hpp"
#include "NFComm/NFCore/NFDateTime.hpp"

enum
{
	LOG_DATA_SIZE = 512,	// ÿ����־��С
	LOG_DATA_NUM = 1024,	// ��־���泤��
	APPID_MAX = 128,		// ֧�ֵ�app��Ŀ
	PATH_SIZE = 128,		// ���ƴ�С
	LEVEL_SIZE = 10,		// �ȼ����ִ�С
};

struct LogData
{
	char szLevel[LEVEL_SIZE] = {};
	char szFile[PATH_SIZE] = {};
	char szData[LOG_DATA_SIZE] = {};
	int64_t dnTime = 0;
	int nLine = 0;
};

struct LogBuffer
{
	unsigned int nPut = 0;
	unsigned int nGet = 0;
	char szLogPrefix[PATH_SIZE] = {};
	LogData xLogDatas[LOG_DATA_NUM];
};

struct LogApp
{
	LogBuffer xLogBuffers[APPID_MAX];
};

class CLogShareMem final
{
private:	
	CLogShareMem(const CLogShareMem&) = delete;
	CLogShareMem& operator=(const CLogShareMem&) = delete;

public:
	CLogShareMem()
	{
	}
	~CLogShareMem()
	{
	}

	bool Start(int nGroupId, const std::string& strPrefix, bool bLogServer = false)
	{
		if (strPrefix.empty())
		{
			return false;
		}

		// �Ѿ���������
		if (m_nGroupId >= 0)
		{
			return false;
		}

		char szMutexName[PATH_SIZE] = {};
		SafeSprintf(szMutexName, sizeof(szMutexName), "hd_mutex_lock_%d", nGroupId);
		if (!m_Mutex.Create(szMutexName))
		{
			return false;
		}

		m_Mutex.Lock();
		
		char szShareLogName[PATH_SIZE] = {};
		SafeSprintf(szShareLogName, sizeof(szShareLogName), "hd_share_memory_log_%d", nGroupId);
		if (!m_LogMem.Create(szShareLogName, sizeof(LogApp)))
		{
			return false;
		}

		LogApp* pLogApp = (LogApp*)m_LogMem.GetMem();
		if (pLogApp == nullptr)
		{
			return false;
		}

		if (bLogServer)
		{
			m_nAppId = 0;
			CopyString(pLogApp->xLogBuffers[0].szLogPrefix,
				sizeof(pLogApp->xLogBuffers[0].szLogPrefix),
				strPrefix.c_str());
		}
		else
		{
			for (int i = 1; i < APPID_MAX; ++i)
			{
				if (pLogApp->xLogBuffers[i].szLogPrefix[0] == 0)
				{
					m_nAppId = i;
					CopyString(pLogApp->xLogBuffers[i].szLogPrefix,
						sizeof(pLogApp->xLogBuffers[i].szLogPrefix),
						strPrefix.c_str());
					break;
				}
			}
		}

		m_Mutex.Unlock();

		m_nGroupId = nGroupId;

		m_bReady = true;

		return true;
	}

	void* GetLogMem()
	{
		return m_LogMem.GetMem();
	}

	bool IsReady()
	{
		return m_bReady;
	}

	int GetGoupId()
	{
		return m_nGroupId;
	}

	int GetAppId()
	{
		return m_nAppId;
	}

	const char* GetProfix(int nAppId)
	{
		if (nAppId < 0 || nAppId >= APPID_MAX)
		{
			return "";
		}

		LogApp* pLogApp = (LogApp*)m_LogMem.GetMem();
		if (pLogApp == nullptr)
		{
			return "";
		}

		return pLogApp->xLogBuffers[nAppId].szLogPrefix;
	}

	bool LogTrace(const char* szLevel, int64_t dnTime, const char* szFile, int nLine, const char* szBuffer)
	{
		if (szBuffer == nullptr || szBuffer[0] == '\0')
		{
			return false;
		}

		LogApp* pLogApp = (LogApp*)m_LogMem.GetMem();
		if (pLogApp == nullptr)
		{
			return false;
		}

		auto& buffer = pLogApp->xLogBuffers[m_nAppId];
		unsigned int put = buffer.nPut;
		unsigned int next = (put + 1) & (LOG_DATA_NUM - 1);

		if (next == buffer.nGet)
		{
			// ����������
			return false;
		}

		// ��������
		auto& data = buffer.xLogDatas[put];
		CopyString(data.szLevel, sizeof(data.szLevel), szLevel);
		CopyString(data.szFile, sizeof(data.szFile), szFile);
		CopyString(data.szData, sizeof(data.szData), szBuffer);
		data.dnTime = dnTime;
		data.nLine = nLine;

		buffer.nPut = next;

		return true;
	}

	const std::string LogFormat(const char* szLevel, int64_t dnTime, const char* szFile, int nLine, const char* szBuffer)
	{
		if (szLevel == nullptr)
		{
			szLevel = "";
		}

		if (szFile == nullptr)
		{
			szFile = "";
		}

		if (szBuffer == nullptr)
		{
			szBuffer = "";
		}

		static std::ostringstream stream;
		stream.str("");
		NFDateTime dt(dnTime);
		stream << "(" << dt.ToString() << ")" << "[" << szLevel << "] " << szBuffer;

		if (nLine >= 0)
		{
			const char* strfile = strrchr(szFile, '\\');
			if (strfile != nullptr)
			{
				strfile = strfile + 1;
				if (strfile != nullptr && strfile[0] != '\0')
				{
					szFile = strfile;
				}
			}

			stream << " " << "[" << szFile << ":" << nLine << "]";
		}

		return std::move(stream.str());
	}

private:
	int m_nGroupId = -1;
	int m_nAppId = 0;
	CShareMem m_LogMem;
	CMutex m_Mutex;
	bool m_bReady = false;

	// ��������ּ��뻺���� ͨ����������������ֲ��Ҷ�Ӧ��mutex�Ƿ�������жϽ����Ƿ����
	std::string m_strLogClientOnlyOne = "";
};

#endif // __LOG_SHARE_MEM_H__
