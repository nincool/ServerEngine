///--------------------------------------------------------------------
/// �ļ���:		DBServerState.h
/// ��  ��:		���ݿ�״̬��
/// ˵  ��:		
/// ��������:	2019��9��26��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _DB_SERVER_STATE_H
#define _DB_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CDBServer;
class CDBServerState : public NFIModule
{
public:
	enum DB_STATE_ENUM
	{
		DBSTATE_UNKNOWN,	// ��ʼ״̬
		DBSTATE_WAITINGOPEND,// �ȴ�����״̬
		DBSTATE_OPENED,		// ������״̬
		DBSTATE_CLOSING,	// ����׼���ر�
		DBSTATE_CLOSED,		// ����ر�״̬
	};

	enum DB_INPUT_ENUM
	{
		DBINPUT_TIMER,		// ��ʱ��ˢ��
		DBINPUT_WAITINGOPEN,// �ȴ�����
		DBINPUT_OPEN,		// ����������
		DBINPUT_CLOSE,		// �رշ�����
	};
public:
	CDBServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	const int GetStateEnum() const;
	int DBStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int DBStateUnknown(int input, const char* msg, const uint32_t nLen);
	int DBStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int DBStateOpened(int input, const char* msg, const uint32_t nLen);
	int DBStateClosed(int input, const char* msg, const uint32_t nLen);
	int DBStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nDBState = DBSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CDBServer* m_pDBServer = nullptr;
};

#endif
