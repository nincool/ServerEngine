///--------------------------------------------------------------------
/// 文件名:		NetCilent.cpp
/// 内  容:		基础客户模式网络连接
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NetClient.h"
#include "NetMsgHead.h"
#include "event2/event.h"
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "NFComm/NFUtils/QLOG.h"
#include "../NFPluginModule/KConstDefine.h"

int64_t NetClient::nSockIndex = 0;

NetClient::NetClient(NetCallback* pCB, const size_t bufferMax)
{
	mpNetCallback = pCB;
	mBufferMax = bufferMax;
	m_pPackData = (char*)malloc(bufferMax);

	++nSockIndex;
	if (nSockIndex == INT64_MAX)
	{
		nSockIndex = 1;
	}
	nPrSockIndex = nSockIndex;
}

NetClient::~NetClient()
{
	CLose();

	if (mBase)
	{
		event_base_free(mBase);
		mBase = nullptr;
	}

	free(m_pPackData);
}

NFSOCK NetClient::Connect(const char* strIP, const unsigned short nPort)
{
	if (mBase != nullptr)
	{
		return -1;
	}

	mstrIP = strIP;
	mnPort = nPort;

#ifdef _MSC_VER
	WSADATA wsa_data;
	if (0 != WSAStartup(0x0201, &wsa_data))
	{
		QLOG_ERROR << __FUNCTION__ << " WSAStartup error!";
		return -1;
	}
#endif

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	if (evutil_inet_pton(AF_INET, mstrIP.c_str(), &addr.sin_addr) <= 0)
	{
		QLOG_ERROR << __FUNCTION__ << " evutil_inet_pton ip:" << mstrIP << ":" << nPort;
		return -1;
	}

	mBase = event_base_new();
	if (mBase == NULL)
	{
		QLOG_ERROR << __FUNCTION__ << " event_base_new";
		return -1;
	}
	
	mBev = bufferevent_socket_new(mBase, -1, BEV_OPT_CLOSE_ON_FREE);
	if (mBev == NULL)
	{
		QLOG_ERROR << __FUNCTION__ << " bufferevent_socket_new ";
		return -1;
	}

	int bRet = bufferevent_socket_connect(mBev, (struct sockaddr*) & addr, sizeof(addr));
	if (0 != bRet)
	{
		QLOG_ERROR << __FUNCTION__ << " bufferevent_socket_connect error";
		return -1;
	}

	bufferevent_getfd(mBev);

#if NF_PLATFORM != NF_PLATFORM_WIN
	int optval = 1;
	//int result = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
	int result = ssetsockopt(fd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval));
	if (result < 0)
	{
		std::cout << "setsockopt TCP_NODELAY ERROR !!!" << std::endl;
	}
#endif

	bufferevent_setcb(mBev, conn_readcb, conn_writecb, conn_eventcb, (void*)this);
	bufferevent_enable(mBev, EV_READ | EV_WRITE | EV_CLOSED | EV_TIMEOUT | EV_PERSIST);

	event_set_log_callback(&log_cb);

	bufferevent_set_max_single_read(mBev, mBufferMax);
	bufferevent_set_max_single_write(mBev, mBufferMax);

	bufferevent_setwatermark(mBev, EV_READ, NetMsgHead::HEAD_LENGTH, 0);

	return nPrSockIndex;
}

bool NetClient::CLose()
{
	if (mBev)
	{
		bufferevent_free(mBev);
		mBev = nullptr;
	}

	return true;
}

bool NetClient::Execute()
{
	if (mBase)
	{
		event_base_loop(mBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}

	return true;
}

bool NetClient::SendMsg(const int16_t nMsgID, const char* msg, const uint32_t nLen)
{
	if (!mbWorking)
	{
		QLOG_WARING << __FUNCTION__ << " not working";
		return false;
	}

	if (nullptr != mpLocalServerCallback)
	{
		mpLocalServerCallback->RunMsgCB(mLocalServerSock, nMsgID, msg, nLen);
		return true;
	}

	NetMsgHead xHead;
	xHead.SetMsgID(nMsgID);
	xHead.SetBodyLength(nLen);

	char szHead[NetMsgHead::HEAD_LENGTH] = { 0 };
	xHead.EnCode(szHead);

	std::string strOutData;
	strOutData.append(szHead, NetMsgHead::HEAD_LENGTH);
	strOutData.append(msg, nLen);

	if (0 != bufferevent_write(mBev, strOutData.c_str(), strOutData.length()))
	{
		QLOG_ERROR << "bufferevent_write error";
		return false;
	}
	else
	{
		QLOG_DEBUG_N << "NetClient::send msgid:" << nMsgID << ",IP:" << mstrIP << ",Port:" << mnPort;
	}
	return true;
}

void NetClient::conn_readcb(struct bufferevent* bev, void* user_data)
{
	NetClient* pNet = (NetClient*)user_data;
	if (nullptr == pNet)
	{
		QLOG_ERROR << __FUNCTION__ << " pNet == NULL";
		return;
	}

	while (true)
	{
		if (!pNet->ProcessRead(bev, user_data))
		{
			return;
		}
	}
}

bool NetClient::ProcessRead(struct bufferevent* bev, void* user_data)
{
	NetClient* pNet = (NetClient*)user_data;
	if (nullptr == pNet)
	{
		QLOG_ERROR << __FUNCTION__ << " pNet == NULL";
		return false;
	}

	struct evbuffer* input = bufferevent_get_input(bev);
	size_t recvLen = evbuffer_get_length(input);
	if (recvLen < NetMsgHead::HEAD_LENGTH)
	{
		// 调整最低水位为包头长度，减少无效的输入回调
		bufferevent_setwatermark(bev, EV_READ, NetMsgHead::HEAD_LENGTH, 0);
		return false;
	}

	//包大小（包括头）
	unsigned int pkgSize = 0;
	struct evbuffer_ptr pos;
	//  读取整包大小
	evbuffer_ptr_set(input, &pos, 2, EVBUFFER_PTR_SET);
	evbuffer_copyout_from(input, &pos, &pkgSize, sizeof(unsigned int));

	pkgSize = ntohl(pkgSize);
	if (pkgSize > pNet->mBufferMax)
	{
		QLOG_ERROR << "pkgSize error!. ip:" << pNet->mstrIP
			<< ",sockIndex:" << pNet->nSockIndex;

		pNet->CLose();
		return false;
	}
	if (recvLen < pkgSize)
	{
		// 已经知道整包长度，调整最低水位
		bufferevent_setwatermark(bev, EV_READ, pkgSize, 0);
		return false;
	}

	// 取出整个网络包buff
	//char* data = (char*)malloc(pkgSize);
	char* data = pNet->m_pPackData;
	int buffSize = evbuffer_remove(input, data, pkgSize);

	//消息ID
	uint16_t nMsgID = 0;
	memcpy(&nMsgID, data, sizeof(uint16_t));
	nMsgID = ntohs(nMsgID);
	//内容长度
	unsigned int nMsgBodySize = pkgSize - NetMsgHead::HEAD_LENGTH;

	//回调
	if (pNet->mpNetCallback)
	{
		pNet->mpNetCallback->RunMsgCB(pNet->nPrSockIndex, nMsgID, data + NetMsgHead::HEAD_LENGTH, nMsgBodySize);
	}
	else
	{
		QLOG_WARING << "NetCallback null";
	}
#ifdef NF_DEBUG_MODE
	QLOG_DEBUG_N << "NetClient::read msgid:" << nMsgID << ",IP:" << pNet->mstrIP << ",Port:" << pNet->mnPort;
#endif
	//free(data);

	// 处理下个网络包
	recvLen = evbuffer_get_length(input);
	if (recvLen > 0)
	{
		return true;
	}
	else
	{
		bufferevent_setwatermark(bev, EV_READ, NetMsgHead::HEAD_LENGTH, 0);
	}
	return false;
}

void NetClient::conn_writecb(struct bufferevent* bev, void* user_data)
{

}

void NetClient::conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{
	NetClient* pNet = (NetClient*)user_data;
	if (nullptr == pNet)
	{
		QLOG_ERROR << __FUNCTION__ << " pNet == NULL";
		return;
	}

	if (pNet->mLocalServerSock != 0)
	{
		//本地连接不要事件
		return;
	}

	if (events & BEV_EVENT_CONNECTED)
	{
		pNet->mbWorking = true;
	}
	else
	{
		pNet->CLose();
	}

	if (pNet->mpNetCallback)
	{
		pNet->mpNetCallback->RunEventCB(pNet->nPrSockIndex, NET_EVENT(events));
	}
	else
	{
		QLOG_WARING << "NetCallback null";
	}
}

void NetClient::log_cb(int severity, const char* msg)
{
	QLOG_ERROR << __FUNCTION__ << " severity:" << severity << "," << msg;
}

int64_t NetClient::GetSockIndex()
{
	return nPrSockIndex;
}

void NetClient::SetLocalServer(const int64_t localServerSock, NetCallback* pLocalServerCallback)
{
	mLocalServerSock = localServerSock;
	mpLocalServerCallback = pLocalServerCallback;
}