///--------------------------------------------------------------------
/// �ļ���:		NetClient.h
/// ��  ��:		�����ͻ�ģʽ��������
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_NetClient_H__
#define __H_NetClient_H__

#include "NetDefine.h"
#include "NetCallback.h"
#include <string>

class NetClient
{
public:
	NetClient(NetCallback* pCB, const size_t bufferMax);
	~NetClient();

	int64_t Connect(const char* strIP, const unsigned short nPort);
	bool CLose();
	bool Execute();
	bool SendMsg(const int16_t nMsgID, const char* msg, const uint32_t nLen);
	int64_t GetSockIndex();
	bool GetWorking();
private:

	static void conn_readcb(struct bufferevent* bev, void* user_data);
	static void conn_writecb(struct bufferevent* bev, void* user_data);
	static void conn_eventcb(struct bufferevent* bev, short events, void* user_data);
	static void log_cb(int severity, const char* msg);
private:
	std::string mstrIP;
	int mnPort = 0;
	bool mbWorking = false;
	std::string mstrBuff;
	size_t mBufferMax = 1024 * 1024;

	static int64_t nSockIndex;
	int64_t nPrSockIndex = -1;
	struct event_base* mBase = nullptr;
	struct bufferevent* mBev = nullptr;

	//�ص�
	NetCallback* mpNetCallback;

};


#endif //__H_NetClient_H__