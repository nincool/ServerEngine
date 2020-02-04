///--------------------------------------------------------------------
/// 文件名:		NetMsgQue.h
/// 内  容:		多生产者多消费无锁队列 基于concurrentqueue
/// 说  明:		
/// 创建日期:	2019年8月13日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetMsgQue_H__
#define __H_NetMsgQue_H__

#include "NFComm/NFPluginModule/NFPlatform.h"
#include "Dependencies/concurrentqueue/concurrentqueue.h"

#pragma pack(push, 1)

struct NetMsgObj
{
	NFSOCK fd;
	uint16_t msgId;
	std::string msgData;
	short event;

	NetMsgObj() : fd(0), msgId(0), event(-1)
	{

	}

	void Reset()
	{
		fd = 0;
		msgId = 0;
		msgData.clear();
		event = -1;
	}

};

class NetMsgQue
{
	
public:
	NetMsgQue()
	{
		mExpandSize = 100;
	}

	~NetMsgQue()
	{
		while (mQueue.size_approx() > 0)
		{
			NetMsgObj* pNetMsg = nullptr;
			if (mQueue.try_dequeue(pNetMsg))
			{
				delete pNetMsg;
			}
		}

		while (mPool.size_approx() > 0)
		{
			NetMsgObj* pNetMsg = nullptr;
			if (mPool.try_dequeue(pNetMsg))
			{
				delete pNetMsg;
			}
		}
	}

	void Init(size_t nInitPoolSzie, size_t nExpandSize)
	{
		for (size_t i = 0; i < nInitPoolSzie; ++i)
		{
			mPool.enqueue(new NetMsgObj);
		}

		mExpandSize = (unsigned int)nExpandSize;
		if (mExpandSize == 0)
		{
			mExpandSize = 100;
		}
	}

	NetMsgObj* NewMsg()
	{
		NetMsgObj* pNetMsg = nullptr;
		if (mPool.try_dequeue(pNetMsg))
		{
			pNetMsg->Reset();
			return pNetMsg;
		}
		else
		{
			return new NetMsgObj();
		}
	}
	
	void Push(NetMsgObj* msg)
	{
		if (nullptr == msg)
		{
			return;
		}
		mQueue.enqueue(msg);
	}

	NetMsgObj* Pop()
	{
		NetMsgObj* pNetMsg = nullptr;
		if (mQueue.try_dequeue(pNetMsg))
		{
			return pNetMsg;
		}

		return nullptr;
	}

	bool Free(NetMsgObj* pNetMsg)
	{
		if (nullptr == pNetMsg)
		{
			return false;
		}

		return mPool.enqueue(pNetMsg);
	}

	size_t Size()
	{
		return mQueue.size_approx();
	}

private:
	moodycamel::ConcurrentQueue<NetMsgObj*> mQueue;
	moodycamel::ConcurrentQueue<NetMsgObj*> mPool;
	unsigned int mExpandSize;
};
#pragma pack(pop)
#endif; //__H_NetMsgQue_H__