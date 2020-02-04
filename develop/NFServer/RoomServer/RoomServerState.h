///--------------------------------------------------------------------
/// �ļ���:		RoomServerState.h
/// ��  ��:		�����״̬��
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _ROOM_SERVER_STATE_H
#define _ROOM_SERVER_STATE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CRoomServer;
class CRoomServerState : public NFIModule
{
public:
	enum ROOM_STATE_ENUM
	{
		ROOMSTATE_UNKNOWN,	// ��ʼ״̬
		ROOMSTATE_WAITINGOPEND,// �ȴ�����״̬
		ROOMSTATE_OPENED,		// ������״̬
		ROOMSTATE_CLOSING,	// ����׼���ر�
		ROOMSTATE_CLOSED,		// ����ر�״̬
	};

	enum ROOM_INPUT_ENUM
	{
		ROOMINPUT_TIMER,		// ��ʱ��ˢ��
		ROOMINPUT_WAITINGOPEN,// �ȴ�����
		ROOMINPUT_OPEN,		// ����������
		ROOMINPUT_CLOSE,		// �رշ�����
	};
public:
	CRoomServerState(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool Execute();
	virtual bool AfterInit();

	const char* GetState() const;
	int GetRoomState() { return m_nRoomState; }
	int RoomStateMachine(int input, const char* msg = "", const uint32_t nLen = 0);

private:
	void SetState(int nState);
	int RoomStateUnknown(int input, const char* msg, const uint32_t nLen);
	int RoomStateWaitingOpen(int input, const char* msg, const uint32_t nLen);
	int RoomStateOpened(int input, const char* msg, const uint32_t nLen);
	int RoomStateClosed(int input, const char* msg, const uint32_t nLen);
	int RoomStateClosing(int input, const char* msg, const uint32_t nLen);

private:
	NFINT64 m_nLastTime = 0;
	int m_nRoomState = ROOMSTATE_UNKNOWN;

	NetClientModule* m_pNetClientModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	CRoomServer* m_pRoomServer = nullptr;
};

#endif
