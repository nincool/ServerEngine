///--------------------------------------------------------------------
/// �ļ���:		ProxyServerState.h
/// ��  ��:		�����״̬��
/// ˵  ��:		
/// ��������:	2019��9��29��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _PROXY_SERVER_STATE_H
#define _PROXY_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CProxyServer;
class CProxyServerLogicModule;
class CProxyServerState : public NFIModule
{
public:
	enum PROXY_STATE_ENUM
	{
		PROXYSTATE_UNKNOWN,	// ��ʼ״̬
		PROXYSTATE_WAITINGOPEND,// �ȴ�����״̬
		PROXYSTATE_PAUSED,		// ��ͣ״̬
		PROXYSTATE_OPENED,		// ������״̬
		PROXYSTATE_CLOSING,	// ����׼���ر�
		PROXYSTATE_CLOSED,		// ����ر�״̬
	};

	enum PROXY_INPUT_ENUM
	{
		PROXYINPUT_TIMER,		// ��ʱ��ˢ��
		PROXYINPUT_WAITINGOPEN,// �ȴ�����
		PROXYINPUT_PAUSED,		// ��ͣ����
		PROXYINPUT_OPEN,		// ����������
		PROXYINPUT_CLOSE,		// �رշ�����
	};

	enum PROXY_CAN_OPEN
	{
		CANT_OPEN = 0,
		CAN_OPEN = 1,
	};
public:
	CProxyServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetProxyState() { return m_nProxyState; }
	int ProxyStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int ProxyStateUnknown(int input, const char* msg, const uint32_t nLen);
	int ProxyStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int ProxyStatePaused(int input, const char* msg, const uint32_t nLen);
	int ProxyStateOpened(int input, const char* msg, const uint32_t nLen);
	int ProxyStateClosed(int input, const char* msg, const uint32_t nLen);
	int ProxyStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nProxyState = PROXYSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	CProxyServerLogicModule* m_pProxyServerLogicModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CProxyServer* m_pProxyServer = nullptr;
};

#endif
