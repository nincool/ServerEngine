///--------------------------------------------------------------------
/// �ļ���:		PubServerState.h
/// ��  ��:		Pub״̬��
/// ˵  ��:		
/// ��������:	2019��9��29��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _PUB_SERVER_STATE_H
#define _PUB_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/IPubKernel.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPubServer;
class CPubServerState : public NFIModule
{
public:
	enum PUB_STATE_ENUM
	{
		PUBSTATE_UNKNOWN,	// ��ʼ״̬
		PUBSTATE_WAITINGOPEND,// �ȴ�����״̬
		PUBSTATE_LOADING,		// �������״̬
		PUBSTATE_OPENED,		// ������״̬
		PUBSTATE_CLOSING,	// ����׼���ر�
		PUBSTATE_CLOSED,		// ����ر�״̬
	};

	enum PUB_INPUT_ENUM
	{
		PUBINPUT_TIMER,		// ��ʱ��ˢ��
		PUBINPUT_WAITINGOPEN,// �ȴ�����
		PUBINPUT_LOADING,		// �������״̬
		PUBINPUT_PUBLOAD,		// ������������
		PUBINPUT_GUILDLOAD,	// ����������
		PUBINPUT_OPEN,		// ����������
		PUBINPUT_CLOSESAVE,		// ���湫������Ϣ
		PUBINPUT_CLOSE,		// �رշ�����
	};
public:
	CPubServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetPubState() { return m_nPubState; }
	int PubStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int PubStateUnknown(int input, const char* msg, const uint32_t nLen);
	int PubStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int PubStateLoading(int input, const char* msg, const uint32_t nLen);
	int PubStateOpened(int input, const char* msg, const uint32_t nLen);
	int PubStateClosed(int input, const char* msg, const uint32_t nLen);
	int PubStateClosing(int input, const char* msg, const uint32_t nLen);

	int StateCheckForOpen();
private:
	NFINT64 m_nLastTime = 0;
	int m_nPubState = PUBSTATE_UNKNOWN;
	bool bCompletePub = false;
	bool bCompleteGuild = false;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	IPubKernel* m_pPubKernel = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CPubServer* m_pPubServer = nullptr;
};

#endif
