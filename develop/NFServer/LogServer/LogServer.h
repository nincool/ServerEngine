///--------------------------------------------------------------------
/// 文件名:		LogServer.h
/// 内  容:		日志服务器
/// 说  明:		将日志写入文件
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __LOG_SERVER_H__
#define __LOG_SERVER_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFUtils/LogShareMem.h"

class CLogServer : public NFIModule
{
public:

	CLogServer(NFIPluginManager* p);
    virtual ~CLogServer() {}

    virtual bool Init();
    virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute();

private:
	void WriteLog();
	bool CanExit(LogApp* pLogApp);
private:
	CLogShareMem m_LogShareMem;
	std::string m_strCurDir = "";
	std::vector<std::string> m_vecFileName;
	NFDateTime m_timeOld = NFDateTime::Now();

	// 是否正在退出
	bool m_bExiting = false;
};

#endif // __LOG_SERVER_H__
