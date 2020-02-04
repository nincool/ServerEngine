///--------------------------------------------------------------------
/// �ļ���:		LogServer.h
/// ��  ��:		��־������
/// ˵  ��:		����־д���ļ�
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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

	// �Ƿ������˳�
	bool m_bExiting = false;
};

#endif // __LOG_SERVER_H__
