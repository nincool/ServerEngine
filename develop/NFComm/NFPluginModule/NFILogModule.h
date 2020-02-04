///--------------------------------------------------------------------
/// 文件名:		NFILogModule.h
/// 内  容:		日志模块
/// 说  明:		
/// 创建日期:	2019年8月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_LOG_MODULE_H
#define NFI_LOG_MODULE_H

#include "NFIModule.h"
#include "NFComm/NFUtils/QLOG.h"

class NFGUID;
class NFILogModule
    : public NFIModule
{

public:

    virtual bool LogElement(const LogLevel eLevel, const NFGUID ident, const std::string& strElement, const std::string& strDesc, const char* func = "", int line = 0) = 0;
    virtual bool LogProperty(const LogLevel eLevel, const NFGUID ident, const std::string& strProperty, const std::string& strDesc, const char* func = "", int line = 0) = 0;
    virtual bool LogObject(const LogLevel eLevel, const NFGUID ident, const std::string& strDesc, const char* func = "", int line = 0) = 0;
    virtual bool LogRecord(const LogLevel eLevel, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const int nRow, const int nCol, const char* func = "", int line = 0) = 0;
    virtual bool LogRecord(const LogLevel eLevel, const NFGUID ident, const std::string& strRecord, const std::string& strDesc, const char* func = "", int line = 0) = 0;

    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const int64_t nDesc, const char* func = "", int line = 0) = 0;
    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::string& strInfo, const std::string& strDesc, const  char* func = "", int line = 0) = 0;
    virtual bool LogNormal(const LogLevel eLevel, const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

    virtual bool LogDebug(const std::string& strLog, const char* func = "", int line = 0) = 0;
    virtual bool LogInfo(const std::string& strLog, const  char* func = "", int line = 0) = 0;
    virtual bool LogWarning(const std::string& strLog, const char* func = "", int line = 0) = 0;
    virtual bool LogError(const std::string& strLog, const char* func = "", int line = 0) = 0;
    virtual bool LogFatal(const std::string& strLog, const char* func = "", int line = 0) = 0;

    virtual bool LogDebug(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
    virtual bool LogInfo(const std::ostringstream& stream, const  char* func = "", int line = 0) = 0;
    virtual bool LogWarning(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
    virtual bool LogError(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
    virtual bool LogFatal(const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

    virtual bool LogDebug(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
    virtual bool LogInfo(const NFGUID ident, const std::string& strLog, const  char* func = "", int line = 0) = 0;
    virtual bool LogWarning(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
    virtual bool LogError(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;
    virtual bool LogFatal(const NFGUID ident, const std::string& strLog, const char* func = "", int line = 0) = 0;

    virtual bool LogDebug(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
    virtual bool LogInfo(const NFGUID ident, const std::ostringstream& stream, const  char* func = "", int line = 0) = 0;
    virtual bool LogWarning(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
    virtual bool LogError(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;
    virtual bool LogFatal(const NFGUID ident, const std::ostringstream& stream, const char* func = "", int line = 0) = 0;

	virtual void StackTrace() = 0;

	virtual void SetLogLauncher(const std::string& launcher) = 0;
	virtual bool StartLogModule(int nGroupID) = 0;
};

#endif