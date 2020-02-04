///--------------------------------------------------------------------
/// 文件名:		CLogServer.cpp
/// 内  容:		日志服务器
/// 说  明:		将日志写入文件
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "LogServer.h"
#include "NFComm/system/WinFileSys.h"
#include "NFComm/system/WinMutex.h"
#include "NFComm/system/WinPortable.h"
#include "NFComm/NFPluginLoader/NFPluginManager.h"
#include "NFComm/NFUtils/QLOG.h"

CLogServer::CLogServer(NFIPluginManager* p)
{
	pPluginManager = p;

	m_vecFileName.resize(APPID_MAX);
}

bool CLogServer::Init()
{
	do 
	{
		char curdir[MAX_PATH] = {};
		if (!Port_GetCurrentDir(curdir, sizeof(curdir)))
		{
			std::cout << " get cur dir failed " << std::endl;
			break;
		}

		m_strCurDir = curdir;
		m_strCurDir += "\\log";
		if (!Port_DirExists(m_strCurDir.c_str()))
		{
			if (!Port_DirCreate(m_strCurDir.c_str()))
			{
				QLOG_ERROR << " dir create failed " << m_strCurDir;
				continue;
			}
		}

		if (!m_LogShareMem.Start(pPluginManager->GetServerID(), pPluginManager->GetTitleName(), true))
		{
			QLOG_ERROR << " share mem start failed ";
			break;
		}

		std::cout << std::endl;
		QLOG_SYSTEM_S << "ready";
		std::cout << std::endl;

		QLOG_INFO_S << "dir:" << m_strCurDir;

		return true;

	} while (false);
	
	dynamic_cast<NFPluginManager*>(pPluginManager)->SetExit(true);

	return true;
}

bool CLogServer::Shut()
{
    return true;
}

bool CLogServer::BeforeShut()
{
    return true;
}

bool CLogServer::AfterInit()
{
    return true;
}

bool CLogServer::Execute()
{
	std::this_thread::sleep_for(std::chrono::microseconds(1000));
	WriteLog();
    return true;
}

void CLogServer::WriteLog()
{
	auto* pLogApp = (LogApp*)(m_LogShareMem.GetLogMem());
	if (pLogApp == nullptr)
	{
		return;
	}

	bool bChangeFileName = false;
	NFDateTime now = NFDateTime::Now();
	if ((now.Year() != m_timeOld.Year())
		|| (now.Month() != m_timeOld.Month())
		|| (now.Day() != m_timeOld.Day()))
	{
		m_timeOld = now;

		m_vecFileName.clear();
		m_vecFileName.resize(APPID_MAX);
	}

	for (int i = 0; i < APPID_MAX; ++i)
	{
		auto& buffer = pLogApp->xLogBuffers[i];
		if (buffer.nGet == buffer.nPut)
		{
			continue;
		}

		if (m_vecFileName[i].empty())
		{
			char dir[MAX_PATH] = {};
			SafeSprintf(dir, sizeof(dir),
				"%s\\%04d%02d%02d\\", m_strCurDir.c_str(), m_timeOld.Year(), m_timeOld.Month(), m_timeOld.Day());
			if (!Port_DirExists(dir))
			{
				if (!Port_DirCreate(dir))
				{
					QLOG_ERROR << "appid:" << i << " dir create failed " << dir;
					continue;
				}
			}

			char name[MAX_PATH] = {};
			SafeSprintf(name, sizeof(name), "%s%s_%04d%02d%02d_%02d%02d%02d.log",
				dir, buffer.szLogPrefix,
				m_timeOld.Year(), m_timeOld.Month(), m_timeOld.Day(), m_timeOld.Hour(), m_timeOld.Minute(), m_timeOld.Second());

			m_vecFileName[i] = name;
			QLOG_INFO_S << "appid:" << i << " client name " << m_vecFileName[i];
		}

		FILE* fp = Port_FileOpen(m_vecFileName[i].c_str(), "ab");
		if (fp == nullptr)
		{
			QLOG_ERROR << "appid:" << i << " open file failed " << m_vecFileName[i];
			continue;	
		}

		unsigned int get = buffer.nGet;
		unsigned int put = buffer.nPut;
		size_t count = 0;

		while (get != put)
		{
			auto& data = buffer.xLogDatas[get];
			const std::string strData
				= m_LogShareMem.LogFormat(data.szLevel,
					data.dnTime,
					data.szFile,
					data.nLine,
					data.szData);
			if (!strData.empty())
			{
				fprintf(fp, "%s\r\n", strData.c_str());
			}

			get = (get + 1) & (LOG_DATA_NUM - 1);

			if (++count >= 100)
			{
				break;
			}
		}

		buffer.nGet = get;

		fclose(fp);
	}

	// 关闭进程
	static int64_t nLastTime = 0;
	if (pPluginManager->GetNowTime() - nLastTime > 10)
	{
		nLastTime = pPluginManager->GetNowTime();
		if (CanExit(pLogApp))
		{
			if (m_bExiting)
			{
				dynamic_cast<NFPluginManager*>(pPluginManager)->SetExit(true); 
				QLOG_SYSTEM_S << "all app closed." << pPluginManager->GetTitleName() << " exit ";
				return;
			}
			
			QLOG_SYSTEM_S << "all app closed." << pPluginManager->GetTitleName() << " will exit ... ";
			m_bExiting = true;
		}
		else if (m_bExiting)
		{
			m_bExiting = false;
			QLOG_SYSTEM_S << pPluginManager->GetTitleName() << " will restart ";
		}
	}
}

bool CLogServer::CanExit(LogApp* pLogApp)
{
	if (pLogApp == nullptr)
	{
		return false;
	}

	bool bRet = true;
	for (int i = 1; i < APPID_MAX; ++i)
	{
		auto& buffer = pLogApp->xLogBuffers[i];
		if (buffer.szLogPrefix[0] == 0)
		{
			continue;
		}

		std::string strPrefix = "hd_mutex_";
		strPrefix += buffer.szLogPrefix;
		if (CMutex::Exists(strPrefix.c_str()))
		{
			bRet = false;
		}
		else
		{
			QLOG_INFO_S << "appid:" << i << " client exit " << buffer.szLogPrefix;
			// 进程已经不存在，回收appid
			buffer.szLogPrefix[0] = 0;
			m_vecFileName[i] = "";
		}
	}
		
	return bRet;
}
