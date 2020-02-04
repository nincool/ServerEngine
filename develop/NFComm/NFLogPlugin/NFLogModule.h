///--------------------------------------------------------------------
/// 文件名:		NFLogModule.h
/// 内  容:		日志
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_LOG_MODULE_H
#define NF_LOG_MODULE_H

#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "NFComm/NFUtils/QLOG.h"

class NFLogModule
    : public NFILogModule
{
public:

    NFLogModule(NFIPluginManager* p);
    virtual ~NFLogModule() {}

    virtual bool Awake();
    virtual bool Init();
	virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute();

	virtual bool StartLogModule(int nGroupID);
	void StartLogServer();

    ///////////////////////////////////////////////////////////////////////
    virtual void LogStack();

    virtual bool LogElement(const LogLevel eLevel, const NFGUID ident, const std::string& strElement, const std::string& strDesc, const char* func = "", int line = 0);
    virtual bool LogProperty(const LogLevel eLevel, const NFGUID ident, const std::string& strProperty, const std::string& strDesc, const char* func = "", int line = 0);
    virtual bool LogRecord(const LogLevel eLevel, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func = "", int line = 0);
    virtual bool LogRecord(const LogLevel eLevel, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const char* func = "", int line = 0);
    virtual bool LogObject(const LogLevel eLevel, const NFGUID ident, const std::string& strDesc, const char* func = "", int line = 0);

    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const char* func = "", int line = 0);
    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const int64_t nDesc, const char* func = "", int line = 0);
    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const std::string& strDesc, const char* func = "", int line = 0);
    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);

    virtual bool LogDebug(const std::string& strLog, const char* func = "", int line = 0);
    virtual bool LogInfo(const std::string& strLog, const  char* func = "", int line = 0);
    virtual bool LogWarning(const std::string& strLog, const char* func = "", int line = 0);
    virtual bool LogError(const std::string& strLog, const char* func = "", int line = 0);
    virtual bool LogFatal(const std::string& strLog, const char* func = "", int line = 0);

    virtual bool LogDebug(const std::ostringstream& stream, const char* func = "", int line = 0);
    virtual bool LogInfo(const std::ostringstream& stream, const  char* func = "", int line = 0);
    virtual bool LogWarning(const std::ostringstream& stream, const char* func = "", int line = 0);
    virtual bool LogError(const std::ostringstream& stream, const char* func = "", int line = 0);
    virtual bool LogFatal(const std::ostringstream& stream, const char* func = "", int line = 0);

    virtual bool LogDebug(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);
    virtual bool LogInfo(const NFGUID ident, const std::string& strLog, const  char* func = "", int line = 0);
    virtual bool LogWarning(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);
    virtual bool LogError(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);
    virtual bool LogFatal(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0);

    virtual bool LogDebug(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);
    virtual bool LogInfo(const NFGUID ident, const std::ostringstream& stream, const  char* func = "", int line = 0);
    virtual bool LogWarning(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);
    virtual bool LogError(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);
    virtual bool LogFatal(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0);

    virtual bool LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg, const char* func = "", const int line = 0);
    virtual bool ChangeLogLevel(const std::string& strLevel);
    
    virtual void StackTrace();

	virtual void SetLogLauncher(const std::string& launcher);

protected:
    virtual bool Log(const LogLevel eLevel, const char* format, ...);

    static bool CheckLogFileExist(const char* filename);
    static void rolloutHandler(const char* filename, std::size_t size);

private:
	std::list<NFPerformance> mxPerformanceList;
	std::string m_strLogLauncher;

};

#endif
