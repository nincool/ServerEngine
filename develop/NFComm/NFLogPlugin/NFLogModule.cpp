///--------------------------------------------------------------------
/// 文件名:		NFLogModule.cpp
/// 内  容:		日志
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#define ELPP_THREAD_SAFE //开启日志库线程安全
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <stdarg.h>
#include "NFLogModule.h"
#include "NFLogPlugin.h"
#include "NFComm/NFUtils/termcolor.hpp"

#if NF_PLATFORM != NF_PLATFORM_WIN
#include <execinfo.h>
#endif
#include "NFComm/system/WinPortable.h"
#include "NFComm/NFUtils/QuickLog.h"

NFLogModule::NFLogModule(NFIPluginManager* p)
{
    pPluginManager = p;
}

bool NFLogModule::Awake()
{
	return true;
}

bool NFLogModule::Init()
{
    return true;
}

bool NFLogModule::StartLogModule(int nGroupID)
{
	// 初始化共享日志
	const std::string& strPrefix = pPluginManager->GetTitleName();
	if (QuickLog::GetRef().StartShareMem(nGroupID, strPrefix.c_str()))
	{
		// 尝试启动日志服务器
		StartLogServer();
	}

	return true;
}

void NFLogModule::StartLogServer()
{
	// 尝试启动日志服务器
	const char* szProfix = QuickLog::GetRef().GetLogServerProfix();
	if (szProfix == nullptr 
		|| szProfix[0] == 0 
		|| !CMutex::Exists((std::string("hd_mutex_")+ szProfix).c_str()))
	{
		Port_Execute(m_strLogLauncher.c_str());

		QLOG_SYSTEM_S << "start " << m_strLogLauncher << " ...";
		return;
	}
}

bool NFLogModule::Shut()
{
    return true;
}

bool NFLogModule::BeforeShut()
{
    return true;

}

bool NFLogModule::AfterInit()
{

    return true;
}

bool NFLogModule::Execute()
{
	QuickLog::GetRef().LogExcute();

	static int64_t nLastTime = 0;
	if (pPluginManager->GetNowTime() - nLastTime > 60)
	{
		nLastTime = pPluginManager->GetNowTime();
		// 尝试启动日志服务器
		StartLogServer();
	}
	
    return true;

}

bool NFLogModule::Log(const LogLevel eLevel, const char* format, ...)
{
    char szBuffer[1024 * 10] = {0};

    va_list args;
    va_start(args, format);
    vsnprintf(szBuffer, sizeof(szBuffer) - 1, format, args);
    va_end(args);

	QWriter(eLevel, "", -1) << szBuffer;

	return true;
}

bool NFLogModule::LogElement(const LogLevel eLevel, const NFGUID ident, const std::string& strElement, const std::string& strDesc, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "[ELEMENT] Indent[%s] Element[%s] %s %s %d", ident.ToString().c_str(), strElement.c_str(), strDesc.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "[ELEMENT] Indent[%s] Element[%s] %s", ident.ToString().c_str(), strElement.c_str(), strDesc.c_str());
    }

    return true;
}

bool NFLogModule::LogProperty(const LogLevel eLevel, const NFGUID ident, const std::string& strProperty, const std::string& strDesc, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "[PROPERTY] Indent[%s] Property[%s] %s %s %d", ident.ToString().c_str(), strProperty.c_str(), strDesc.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "[PROPERTY] Indent[%s] Property[%s] %s", ident.ToString().c_str(), strProperty.c_str(), strDesc.c_str());
    }

    return true;

}

bool NFLogModule::LogRecord(const LogLevel eLevel, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "[RECORD] Indent[%s] Record[%s] Row[%d] Col[%d] %s %s %d", ident.ToString().c_str(), strRecord.c_str(), nRow, nCol, strDesc.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "[RECORD] Indent[%s] Record[%s] Row[%d] Col[%d] %s", ident.ToString().c_str(), strRecord.c_str(), nRow, nCol, strDesc.c_str());
    }

    return true;

}

bool NFLogModule::LogRecord(const LogLevel eLevel, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "[RECORD] Indent[%s] Record[%s] %s %s %d", ident.ToString().c_str(), strRecord.c_str(), strDesc.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "[RECORD] Indent[%s] Record[%s] %s", ident.ToString().c_str(), strRecord.c_str(), strDesc.c_str());
    }

    return true;
}

bool NFLogModule::LogObject(const LogLevel eLevel, const NFGUID ident, const std::string& strDesc, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "[OBJECT] Indent[%s] %s %s %d", ident.ToString().c_str(), strDesc.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "[OBJECT] Indent[%s] %s", ident.ToString().c_str(), strDesc.c_str());
    }

    return true;

}

void NFLogModule::LogStack()
{

    //To Add
#if NF_PLATFORM == NF_PLATFORM_WIN
    time_t t = time(0);
    char szDmupName[MAX_PATH];
    tm* ptm = localtime(&t);

    sprintf(szDmupName, "%d_%d_%d_%d_%d_%d.dmp",  ptm->tm_year + 1900, ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    
    HANDLE hDumpFile = CreateFileA(szDmupName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    
    MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
    //dumpInfo.ExceptionPointers = pException;
    dumpInfo.ThreadId = GetCurrentThreadId();
    dumpInfo.ClientPointers = TRUE;

    
    MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

    CloseHandle(hDumpFile);
#else
	int size = 16;
	void * array[16];
	int stack_num = backtrace(array, size);
	char ** stacktrace = backtrace_symbols(array, stack_num);
	for (int i = 0; i < stack_num; ++i)
	{
		//printf("%s\n", stacktrace[i]);
        
		LOG(FATAL) << stacktrace[i];
	}

	free(stacktrace);
#endif
 
    
}

bool NFLogModule::LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const std::string& strDesc, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "Indent[%s] %s %s %s %d", ident.ToString().c_str(), strInfo.c_str(), strDesc.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "Indent[%s] %s %s", ident.ToString().c_str(), strInfo.c_str(), strDesc.c_str());
    }

    return true;
}

bool NFLogModule::LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const int64_t nDesc, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "Indent[%s] %s %lld %s %d", ident.ToString().c_str(), strInfo.c_str(), nDesc, func, line);
    }
    else
    {
        Log(eLevel, "Indent[%s] %s %lld", ident.ToString().c_str(), strInfo.c_str(), nDesc);
    }

    return true;
}

bool NFLogModule::LogNormal(const LogLevel eLevel, const NFGUID ident, const std::ostringstream& stream, const char* func, int line)
{
    if (line > 0)
    {
        Log(eLevel, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
    }
    else
    {
        Log(eLevel, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
    }

    return true;
}

bool NFLogModule::LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const char* func /*= ""*/, int line /*= 0*/)
{
    if (line > 0)
    {
        Log(eLevel, "Indent[%s] %s %s %d", ident.ToString().c_str(), strInfo.c_str(), func, line);
    }
    else
    {
        Log(eLevel, "Indent[%s] %s", ident.ToString().c_str(), strInfo.c_str());
    }

    return true;
}

bool NFLogModule::LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg,  const char* func /*= ""*/, const int line /*= 0*/)
{
    //#ifdef NF_DEBUG_MODE
    LogNormal(LOG_LEVEL_WARING, ident, strArg + "MsgID:", nMsg, func, line);
    //#endif
    return true;
}

bool NFLogModule::ChangeLogLevel(const std::string& strLevel)
{
	QuickLog::GetRef().SetMaxLevel(QuickLog::GetRef().GetLogLevel(strLevel.c_str()));
    LogNormal(LOG_LEVEL_INFO, NFGUID(), "[Log] Change log level", strLevel, __FUNCTION__, __LINE__);
    return true;
}

bool NFLogModule::LogDebug(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_DEBUG, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_DEBUG, "%s", strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogInfo(const std::string& strLog, const  char* func, int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_INFO, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_INFO, "%s", strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogWarning(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_WARING, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_WARING, "%s", strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogError(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_ERROR, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_ERROR, "%s", strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogFatal(const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_FATAL, "%s %s %d", strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_FATAL, "%s", strLog.c_str());
     }

     return true;
}


bool NFLogModule::LogDebug(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_DEBUG, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_DEBUG, "%s", stream.str().c_str());
     }

     return true;
}

bool NFLogModule::LogInfo(const std::ostringstream& stream, const  char* func, int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_INFO, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_INFO, "%s", stream.str().c_str());
     }

     return true;
}

bool NFLogModule::LogWarning(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_WARING, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_WARING, "%s", stream.str().c_str());
     }

     return true;
}

bool NFLogModule::LogError(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_ERROR, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_ERROR, "%s", stream.str().c_str());
     }

     return true;
}

bool NFLogModule::LogFatal(const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_FATAL, "%s %s %d", stream.str().c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_FATAL, "%s", stream.str().c_str());
     }

     return true;
}


bool NFLogModule::LogDebug(const NFGUID ident, const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_DEBUG, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_DEBUG, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogInfo(const NFGUID ident, const std::string& strLog, const  char* func, int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_INFO, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_INFO, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogWarning(const NFGUID ident, const std::string& strLog, const char* func , int line)
{
    if (line > 0)
     {
         Log(LOG_LEVEL_WARING, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_WARING, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogError(const NFGUID ident, const std::string& strLog, const char* func , int line)
{
     if (line > 0)
     {
         Log(LOG_LEVEL_ERROR, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_ERROR, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
     }

     return true;
}

bool NFLogModule::LogFatal(const NFGUID ident, const std::string& strLog, const char* func , int line)
{
     if (line > 0)
     {
         Log(LOG_LEVEL_FATAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), strLog.c_str(), func, line);
     }
     else
     {
         Log(LOG_LEVEL_FATAL, "Indent[%s] %s", ident.ToString().c_str(), strLog.c_str());
     }

     return true;
}


bool NFLogModule::LogDebug(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
    {
        Log(LOG_LEVEL_DEBUG, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
    }
    else
    {
        Log(LOG_LEVEL_DEBUG, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
    }

    return true;
}

bool NFLogModule::LogInfo(const NFGUID ident, const std::ostringstream& stream, const  char* func, int line)
{
    if (line > 0)
    {
        Log(LOG_LEVEL_INFO, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
    }
    else
    {
        Log(LOG_LEVEL_INFO, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
    }

    return true;
}

bool NFLogModule::LogWarning(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
    {
        Log(LOG_LEVEL_WARING, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
    }
    else
    {
        Log(LOG_LEVEL_WARING, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
    }

    return true;
}

bool NFLogModule::LogError(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
    {
        Log(LOG_LEVEL_ERROR, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
    }
    else
    {
        Log(LOG_LEVEL_ERROR, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
    }

    return true;
}

bool NFLogModule::LogFatal(const NFGUID ident, const std::ostringstream& stream, const char* func , int line)
{
    if (line > 0)
    {
        Log(LOG_LEVEL_FATAL, "Indent[%s] %s %s %d", ident.ToString().c_str(), stream.str().c_str(), func, line);
    }
    else
    {
        Log(LOG_LEVEL_FATAL, "Indent[%s] %s", ident.ToString().c_str(), stream.str().c_str());
    }

    return true;
}

void NFLogModule::StackTrace(/*const LogLevel eLevel = NFILogModule::eLevel_FATAL_NORMAL*/)
{
#if NF_PLATFORM != NF_PLATFORM_WIN

    int size = 8;
    void * array[8];
    int stack_num = backtrace(array, size);
    char ** stacktrace = backtrace_symbols(array, stack_num);
    for (int i = 0; i < stack_num; ++i)
    {
    	//printf("%s\n", stacktrace[i]);
    	LOG(FATAL) << stacktrace[i];
    }

    free(stacktrace);
#else

	static const int MAX_STACK_FRAMES = 8;
	
	void *pStack[MAX_STACK_FRAMES];
 
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);
 
	QLOG_FATAL_S << "stack traceback: ";
	for (WORD i = 0; i < frames; ++i) {
		DWORD64 address = (DWORD64)(pStack[i]);
 
		DWORD64 displacementSym = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;
 
		DWORD displacementLine = 0;
		IMAGEHLP_LINE64 line;
		//SymSetOptions(SYMOPT_LOAD_LINES);
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
 
		if (SymFromAddr(process, address, &displacementSym, pSymbol)
		 && SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
			QLOG_FATAL_S << "\t" << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")";
		}
		else {
			QLOG_FATAL_S << "\terror: " << GetLastError();
		}
	}
#endif
}

void NFLogModule::SetLogLauncher(const std::string& launcher)
{
	m_strLogLauncher = launcher;
}
