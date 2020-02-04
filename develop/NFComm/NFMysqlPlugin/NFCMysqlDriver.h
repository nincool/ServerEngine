///--------------------------------------------------------------------
/// 文件名:		NFCMysqlDriver.h
/// 内  容:		mysql驱动程序
/// 说  明:		
/// 创建日期:	2019年9月24日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NFCMysqlDriver_H__
#define __H_NFCMysqlDriver_H__

#ifdef _MSC_VER
#include <windows.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <stdint.h>
#pragma comment(lib, "ws2_32.lib")

#else
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
//#include <sys/timeb.h>
#include <sys/types.h>

#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#endif

#include <string>
#include <vector>
#include <set>
#include "mysqlpp/mysql++.h"
#include "../NFUtils/QLOG.h"

#define  NFMYSQLTRYBEGIN try {

#define  NFMYSQLTRYEND(msg) }\
    catch (mysqlpp::BadQuery er) \
    { \
		QLOG_ERROR << "BadQuery [" << msg << "] Error: " << er.what(); \
        return false; \
    } \
    catch (const mysqlpp::BadConversion& er)  \
    { \
        QLOG_ERROR << "BadConversion [" << msg << "] Error:" << er.what() << " retrieved data size:" << er.retrieved << ", actual size:" << er.actual_size; \
		return false; \
    } \
    catch (const mysqlpp::Exception& er) \
    { \
        QLOG_ERROR << "mysqlpp::Exception [" << msg << "] Error:" << er.what(); \
        return false; \
    }\
    catch ( ... ) \
    { \
        QLOG_ERROR << "std::exception [" <<msg << "] Error:Unknown "; \
        return false; \
    }

class NFCMysqlDriver
{
public:
    NFCMysqlDriver(const int nReconnectTime = 60, const int nReconnectCount = -1);

    virtual ~NFCMysqlDriver();


    virtual bool Connect(const std::string& strDBName,
						 const std::string& strDBHost,
						 const int nDBPort,
						 const std::string& strDBUser,
						 const std::string& strDBPwd);

	virtual mysqlpp::Connection* GetConnection();

	virtual bool Query(const std::string& qstr, mysqlpp::StoreQueryResult& queryResult);
	virtual bool Query(const std::string& qstr, mysqlpp::UseQueryResult& queryResult);
    virtual bool Query(const std::string& qstr);
	virtual bool Query(mysqlpp::Query& query);
	virtual bool Query(mysqlpp::Query& query, mysqlpp::UseQueryResult& ret);
	
	virtual bool Enable();

    virtual bool CanReconnect();
    virtual bool Reconnect();

    virtual const std::string& GetConnectName();

    virtual void GetAddress(std::string& ip, int& port);
    
protected:
    bool IsNeedReconnect();
    bool Connect();
	void CloseConnection();

private:
    std::string mstrDBName;
    std::string mstrDBHost;
    int mnDBPort;
    std::string mstrDBUser;
    std::string mstrDBPwd;

    mysqlpp::Connection* m_pMysqlConnect; // mysql连接对象
    float mfCheckReconnect;             //检查重连的时间

    int mnReconnectTime;
    int mnReconnectCount;
};

#endif // !__H_NFCMysqlDriver_H__