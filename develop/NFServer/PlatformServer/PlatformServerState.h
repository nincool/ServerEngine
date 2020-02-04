///--------------------------------------------------------------------
/// �ļ���:		PlatformServerState.h
/// ��  ��:		ƽ̨��״̬��
/// ˵  ��:		
/// ��������:	2019��9��29��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _PLATFORM_SERVER_STATE_H
#define _PLATFORM_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/IPlatformKernel.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPlatformServer;
class CPlatformServerState : public NFIModule
{
public:
	enum PLATFORM_STATE_ENUM
	{
		PLATFORMSTATE_UNKNOWN,	// ��ʼ״̬
		PLATFORMSTATE_WAITINGALLNET,	// �ȴ�ROOM����
		PLATFORMSTATE_LOADINGDB,		// �������db
		PLATFORMSTATE_WAITINGOPEN,		// ����ȴ�����
		PLATFORMSTATE_OPENED,		// ������״̬
		PLATFORMSTATE_CLOSING,	// ����׼���ر�
		PLATFORMSTATE_CLOSED,		// ����ر�״̬
	};

	enum PLATFORM_INPUT_ENUM
	{
		PLATFORMINPUT_TIMER,		// ��ʱ��ˢ��
		PLATFORMINPUT_WAITINGALLNET,	// �ȴ�ROOM����״̬
		PLATFORMINPUT_ROOMNET,		// ROOM�������
		PLATFORMINPUT_LOADINGDB,		// �������db״̬
		PLATFORMINPUT_WAITINGOPEN,		// ����ȴ�����״̬
		PLATFORMINPUT_ROOMOPEN,		// ROOM����״̬
		PLATFORMINPUT_CLOSE,		// �رշ�����
	};
public:
	CPlatformServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetPlatformState() { return m_nPlatformState; }
	int PlatformStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int PlatformStateUnknown(int input, const char* msg, const uint32_t nLen);
	int PlatformStateWaitingAllNet(int input, const char* msg, const uint32_t nLen);
	int PlatformStateLoadingDB(int input, const char* msg, const uint32_t nLen);
	int PlatformStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int PlatformStateOpened(int input, const char* msg, const uint32_t nLen);
	int PlatformStateClosed(int input, const char* msg, const uint32_t nLen);
	int PlatformStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nPlatformState = PLATFORMSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	CPlatformServer* m_pPlatformServer = nullptr;
	IPlatformKernel* m_pPlatformKernel = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
