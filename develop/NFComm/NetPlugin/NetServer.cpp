///--------------------------------------------------------------------
/// 文件名:		NetServer.cpp
/// 内  容:		基础服务模式网络连接
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NetServer.h"
#include "event2/event.h"
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include "event2/thread.h"
#include "event2/listener.h"
#include "NetMsgHead.h"
#include "NFComm/NFUtils/QLOG.h"
#include "../NFUtils/QuickLog.h"

NetServer::NetServer(NetCallback* pCB, const size_t maxSendBufferSize, const size_t maxReadBufferSize,
	const int nMaxClient, const bool threads)
{
	mpNetCallback = pCB;
	mMaxSendBufferSize = maxSendBufferSize;
	mMaxReadBufferSize = maxReadBufferSize;
	mMaxConnect = nMaxClient;
	mThreads = threads;
}

NetServer::~NetServer()
{
	Final();
}

bool NetServer::InitServerNet(const std::string& ip, const int port)
{
	struct sockaddr_in sin;

	WSADATA wsa_data;
	if (0 != WSAStartup(0x0201, &wsa_data))
	{
		QLOG_ERROR << __FUNCTION__ << " WSAStartup error!";
		return false;
	}

	struct event_config* cfg = event_config_new();

	if (mThreads)
	{
		event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
		event_config_set_num_cpus_hint(cfg, 1);
		evthread_use_windows_threads();
	}
	mxBase = event_base_new_with_config(cfg);

	event_config_free(cfg);

	if (!mxBase)
	{
		QLOG_ERROR << __FUNCTION__ << " Could not initialize libevent!";
		Final();
		return false;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	sin.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	
	listener = evconnlistener_new_bind(mxBase, listener_cb, (void*)this,
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*) & sin,
		sizeof(sin));
	if (!listener)
	{
		QLOG_ERROR << __FUNCTION__ << " Could not create a listener! ip:" << ip 
			<< " port:" << port;
		Final();
		return false;
	}

	QuickLog::GetRef().OpenNextSystemEchoGreen();
	QLOG_SYSTEM_S << "Open tcp net ip:" << ip
		<< " port:" << port;

	event_set_log_callback(&log_cb);

	return true;
}

void NetServer::listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
	struct sockaddr* sa, int socklen, void* user_data)
{
	NetServer* pNet = (NetServer*)user_data;
	
	//接入对象满
	if (pNet->mmObject.size() >= pNet->mMaxConnect)
	{
		evutil_closesocket(fd);
		QLOG_ERROR << __FUNCTION__ << " Connect full！";
		return;
	}

	struct event_base* mxBase = pNet->mxBase;
	struct bufferevent* bev = bufferevent_socket_new(mxBase, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		QLOG_ERROR << __FUNCTION__ << " bufferevent_socket_new failed!";
		return;
	}

	struct sockaddr_in* pSin = (sockaddr_in*)sa;

	NetObj* pObject = new NetObj(pNet, fd, *pSin, bev, pNet->mMaxReadBufferSize);
	pObject->GetNet()->AddNetObject(pObject->GetSockIndex(), pObject);

#ifndef NF_DEBUG_MODE

	struct timeval readTimeout;
	readTimeout.tv_sec = 100;
	readTimeout.tv_usec = 0;
	bufferevent_set_timeouts(bev, &readTimeout, NULL);

#endif // !NF_DEBUG_MODE

	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, (void*)pObject);
	bufferevent_enable(bev, EV_READ | EV_WRITE | EV_CLOSED | EV_TIMEOUT | EV_PERSIST);
	event_set_fatal_callback(event_fatal_cb);
	conn_eventcb(bev, BEV_EVENT_CONNECTED, (void*)pObject);

	bufferevent_set_max_single_read(bev, pNet->mMaxReadBufferSize);
	bufferevent_set_max_single_write(bev, pNet->mMaxSendBufferSize);

	bufferevent_setwatermark(bev, EV_READ, NetMsgHead::HEAD_LENGTH, NetMsgHead::HEAD_LENGTH);
}

void NetServer::conn_readcb(struct bufferevent* bev, void* user_data)
{
	NetObj* pObject = (NetObj*)user_data;
	if (nullptr == pObject)
	{
		QLOG_WARING << __FUNCTION__ << " pObject == NULL";
		return;
	}

	NetServer* pNet = (NetServer*)pObject->GetNet();
	if (nullptr == pNet)
	{
		QLOG_WARING << __FUNCTION__ << " pNet == NULL";
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

bool NetServer::ProcessRead(struct bufferevent* bev, void* user_data)
{
	NetObj* pObject = (NetObj*)user_data;
	if (nullptr == pObject)
	{
		QLOG_WARING << __FUNCTION__ << " pObject == NULL";
		return false;
	}

	NetServer* pNet = (NetServer*)pObject->GetNet();
	if (nullptr == pNet)
	{
		QLOG_WARING << __FUNCTION__ << " pNet == NULL";
		return false;
	}

	struct evbuffer* input = bufferevent_get_input(bev);
	size_t recvLen = evbuffer_get_length(input);
	if (recvLen < NetMsgHead::HEAD_LENGTH)
	{
		// 调整最低水位为包头长度，减少无效的输入回调
		bufferevent_setwatermark(bev, EV_READ, NetMsgHead::HEAD_LENGTH, NetMsgHead::HEAD_LENGTH);
		return false;
	}

	//包大小（包括头）
	unsigned int pkgSize = 0;
	struct evbuffer_ptr pos;
	//  读取整包大小
	evbuffer_ptr_set(input, &pos, 2, EVBUFFER_PTR_SET);
	evbuffer_copyout_from(input, &pos, &pkgSize, sizeof(unsigned int));
	evbuffer_ptr_set(input, &pos, 0, EVBUFFER_PTR_SET);

	pkgSize = ntohl(pkgSize);
	if (pkgSize >= pNet->mMaxReadBufferSize)
	{
		QLOG_ERROR << "pkgSize error!. ip:" << pObject->GetIP()
			<< ",sockIndex:" << pObject->GetSockIndex()
			<< ",account:" << pObject->GetAccount();

		pNet->CloseNetObj(pObject->GetSockIndex());
		return false;
	}
	if (recvLen < pkgSize)
	{
		// 已经知道整包长度，调整最低水位
		bufferevent_setwatermark(bev, EV_READ, pkgSize, pkgSize);
		return false;
	}

	// 取出整个网络包buff
	//char* data = (char*)malloc(pkgSize);
	char* data = pObject->GetPackData();
	int buffSize = evbuffer_remove(input, data, pkgSize);

	//消息ID
	uint16_t nMsgID = 0;
	memcpy(&nMsgID, data, sizeof(uint16_t));
	nMsgID = ntohs(nMsgID);
	//内容长度
	unsigned int nMsgBodySize = pkgSize - NetMsgHead::HEAD_LENGTH;

	if (pNet->mpNetCallback)
	{
		if (pNet->mThreads)
		{
			NetMsgObj* msg = pNet->mNetMsgQueue.NewMsg();
			msg->fd = pObject->GetSockIndex();
			msg->msgId = nMsgID;
			if (nMsgBodySize > 0)
			{
				msg->msgData.append(data + NetMsgHead::HEAD_LENGTH, nMsgBodySize);
			}
			pNet->mNetMsgQueue.Push(msg);
		}
		else
		{
			pNet->mpNetCallback->RunMsgCB(pObject->GetSockIndex(), nMsgID, data + NetMsgHead::HEAD_LENGTH, nMsgBodySize);
#ifdef NF_DEBUG_MODE
			QLOG_DEBUG_N << "NetServer::read msgid:" << nMsgID << ",ip:" << pObject->GetIP() << ":" << pObject->GetPort();
#endif
		}
	}

	//free(data);

	// 处理下个网络包
	recvLen = evbuffer_get_length(input);
	if (recvLen > 0)
	{
		return true;
	}
	else
	{
		bufferevent_setwatermark(bev, EV_READ, NetMsgHead::HEAD_LENGTH, NetMsgHead::HEAD_LENGTH);
	}
	return false;
}

void NetServer::conn_writecb(struct bufferevent* bev, void* user_data)
{

}

void NetServer::conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{
	NetObj* pObject = (NetObj*)user_data;
	NetServer* pNet = (NetServer*)pObject->GetNet();

	if (pObject->IsLocalClient())
	{
		//本地连接不要事件
		return;
	}

	int64_t nSockIndex = pObject->GetSockIndex();

	if (events & BEV_EVENT_CONNECTED)
	{
		//连接成功
		NFGUID xClientIdent;
		Port_CreateGuid(xClientIdent);
		pObject->SetClientID(xClientIdent);

	}
	else
	{
		pObject->SetWaitDelete(true);
	}

	if (pNet->mpNetCallback)
	{
		if (pNet->mThreads)
		{
			NetMsgObj* msg = pNet->mNetMsgQueue.NewMsg();
			msg->fd = nSockIndex;
			msg->event = events;
			pNet->mNetMsgQueue.Push(msg);
		}
		else
		{
			pNet->mpNetCallback->RunEventCB(nSockIndex, NET_EVENT(events));
			if (!(events & BEV_EVENT_CONNECTED))
			{
				pNet->CloseNetObj(pObject->GetSockIndex());
			}
		}
	}
}

void NetServer::log_cb(int severity, const char* msg)
{
	QLOG_ERROR << __FUNCTION__ << " severity:" << severity << "," << msg;
}

void NetServer::event_fatal_cb(int err)
{
	QLOG_ERROR << __FUNCTION__ << " error:" << err;
}

void NetServer::CloseNetObj(const NFSOCK nSockIndex)
{
	std::map<NFSOCK, NetObj*>::iterator it = mmObject.find(nSockIndex);
	if (it != mmObject.end())
	{
		NetObj* pObject = it->second;
		if (nullptr != pObject)
		{
			struct bufferevent* bev = (bufferevent*)pObject->GetUserData();
			if (nullptr != bev)
			{
				bufferevent_free(bev);
				bev = nullptr;
			}

			delete pObject;
			pObject = nullptr;
		}
		mmObject.erase(it);
	}
}

bool NetServer::CloseNetObjAll()
{
	auto it = mmObject.begin();
	for (; it != mmObject.end(); ++it)
	{
		NetObj* pObject = it->second;
		if (nullptr == pObject)
		{
			continue;
		}

		struct bufferevent* bev = (bufferevent*)pObject->GetUserData();
		if (nullptr != bev)
		{
			bufferevent_free(bev);
			bev = nullptr;
		}

		delete pObject;
		pObject = nullptr;
	}
	mmObject.clear();

	return true;
}

bool NetServer::Execute()
{
	if (mxBase)
	{
		event_base_loop(mxBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	}

	if (mThreads)
	{
		if (mpNetCallback)
		{
			size_t nReadNum = mMaxPerExecute;
			size_t nQueueSize = mNetMsgQueue.Size();
			if (nReadNum > nQueueSize)
			{
				nReadNum = nQueueSize;
			}

			for (int i = 0; i < nReadNum; ++i)
			{
				NetMsgObj* msg = mNetMsgQueue.Pop();
				if (nullptr == msg)
				{
					QLOG_ERROR << "NetMsgObj null";
					return true;
				}
				if (-1 == msg->event)
				{
					mpNetCallback->RunMsgCB(msg->fd, msg->msgId, msg->msgData.data(), msg->msgData.length());
#ifdef NF_DEBUG_MODE
					NetObj* pNetObj = GetNetObject(msg->fd);
					if (pNetObj)
					{
						QLOG_DEBUG_N << "NetServer::read msgid:" << msg->msgId << ",ip:" << pNetObj->GetIP() << ":" << pNetObj->GetPort();
					}
#endif
				}
				else
				{
					mpNetCallback->RunEventCB(msg->fd, (NET_EVENT)msg->event);
#ifdef NF_DEBUG_MODE
					NetObj* pNetObj = GetNetObject(msg->fd);
					if (pNetObj)
					{
						QLOG_DEBUG_N << "NetServer::event id:" << msg->event << ",ip:" << pNetObj->GetIP() << ":" << pNetObj->GetPort();
					}
#endif
					if (!((int)msg->event & (int)NET_EVENT::NET_EVENT_CONNECTED))
					{
						Mtx.lock();
						CloseNetObj(msg->fd);
						Mtx.unlock();
					}
				}

				mNetMsgQueue.Free(msg);
			}
		}
	}

	return true;
}

bool NetServer::Final()
{
	CloseNetObjAll();

	if (listener)
	{
		evconnlistener_free(listener);
		listener = nullptr;
	}

	if (mxBase)
	{
		event_base_free(mxBase);
		mxBase = nullptr;
	}

	return true;
}

bool NetServer::SendMsg(const int16_t nMsgID, const char* msg, const size_t nLen, const NFSOCK nSockIndex)
{
	auto it = mmObject.find(nSockIndex);
	if (it == mmObject.end())
	{
		return false;
	}
	if (it->second->SendLocalClient(nMsgID, msg, nLen))
	{
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

	bool r = SendMsg(strOutData.c_str(), strOutData.length(), nSockIndex);
#ifdef NF_DEBUG_MODE
	if (r)
	{
		NetObj* pNetObj = GetNetObject(nSockIndex);
		if (pNetObj)
		{
			QLOG_DEBUG_N << "NetServer::send msgid:" << nMsgID << ",ip:" << pNetObj->GetIP() << ":" << pNetObj->GetPort();
		}
	}
#endif
	return r;
}

bool NetServer::SendMsg(const int16_t nMsgID, const char* msg, const size_t nLen, const std::list<NFSOCK>& fdList)
{
	auto it = fdList.begin();
	auto it_end = fdList.end();
	for (; it != it_end; ++it)
	{
		SendMsg(nMsgID, msg, nLen, *it);
	}

	return true;
}
bool NetServer::SendMsgToAll(const int16_t nMsgID, const char* msg, const size_t nLen)
{
	NetMsgHead xHead;
	xHead.SetMsgID(nMsgID);
	xHead.SetBodyLength(nLen);
	char szHead[NetMsgHead::HEAD_LENGTH] = { 0 };
	xHead.EnCode(szHead);
	std::string strOutData;
	strOutData.append(szHead, NetMsgHead::HEAD_LENGTH);
	strOutData.append(msg, nLen);

	auto it = mmObject.begin();
	auto it_end = mmObject.end();
	for (; it != it_end; ++it)
	{
		NetObj* pObject = it->second;
		if (nullptr == pObject)
		{
			continue;
		}

		if (pObject->SendLocalClient(nMsgID, msg, nLen))
		{

		}
		else
		{
			bufferevent* bev = (bufferevent*)pObject->GetUserData();
			if (NULL != bev)
			{
				if (-1 == bufferevent_write(bev, msg, nLen))
				{
					QLOG_ERROR << __FUNCTION__ << " bufferevent_write failed";
				}
				else
				{
					QLOG_DEBUG_N << "NetServer::send ToAll msgid:" << nMsgID << ",ip:" << pObject->GetIP() << ":" << pObject->GetPort();
				}
			}
		}
		
	}
	return true;
}

bool NetServer::SendMsg(const char* msg, const size_t nLen, const NFSOCK nSockIndex)
{
	if (nLen <= 0)
	{
		return false;
	}

	auto it = mmObject.find(nSockIndex);
	if (it != mmObject.end())
	{
		NetObj* pNetObj = (NetObj*)it->second;
		if (pNetObj && !pNetObj->GetWaitDelete())
		{
			bufferevent* bev = (bufferevent*)pNetObj->GetUserData();
			if (NULL != bev)
			{
				if (-1 == bufferevent_write(bev, msg, nLen))
				{
					QLOG_ERROR << __FUNCTION__ << " bufferevent_write failed";
					return false;
				}
				return true;
			}
		}
	}
	return false;
}

NetObj* NetServer::GetNetObject(const NFSOCK nSockIndex)
{
	std::map<NFSOCK, NetObj*>::iterator it = mmObject.find(nSockIndex);
	if (it != mmObject.end())
	{
		return it->second;
	}

	return nullptr;
}

bool NetServer::AddNetObject(const NFSOCK nSockIndex, NetObj* pObject)
{
	//lock
	if (mThreads)
	{
		Mtx.lock();

		mmObject.insert(std::map<NFSOCK, NetObj*>::value_type(nSockIndex, pObject));
		Mtx.unlock();
	}
	else
	{
		mmObject.insert(std::map<NFSOCK, NetObj*>::value_type(nSockIndex, pObject));
	}

	return true;
}
