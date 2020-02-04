///--------------------------------------------------------------------
/// 文件名:		NetCallback.h
/// 内  容:		络络相关回调
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetCallback_H__
#define __H_NetCallback_H__
#include <vector>
#include <functional>
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NetDefine.h"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "NFComm/NFUtils/QLOG.h"
#include "NFComm/NFKernelPlugin/PerformanceCountModule.h"
#include "../NFPluginModule/KConstDefine.h"
#include "../NFPluginModule/NFIPluginManager.h"

//网络消息回调
typedef std::function<void(const NFSOCK nSockIndex, const uint16_t nMsgID, const char* msg,
	const uint32_t nLen)> NetMsgFunCB;

//连接服务器网络回调
typedef std::function<void(const NFSOCK nSockIndex, const NET_EVENT nEvent)> NetEventFunCB;
typedef void(NFIModule::* NetEventFun)(const NFSOCK nSockIndex, const NET_EVENT nEvent);


class NetCallback
{
public:
	void AddReceiveCallBack(NFIModule* pBase, NetMsgFun fun)
	{
		NetMsgFunCB funcb = std::bind(fun, pBase, std::placeholders::_1,
			std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		mMsgDefaultCB.push_back(funcb);
	}

	void AddReceiveCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun)
	{
		NetMsgFunCB funcb = std::bind(fun, pBase, std::placeholders::_1,
			std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		if (nMsgID >= static_cast<uint16_t>(mMsgCB.size()))
		{
			mMsgCB.resize(static_cast<size_t>(nMsgID) + 10);
		}

		mMsgCB[nMsgID].push_back(funcb);
	}

	void AddEventCallBack(NFIModule* pBase, NetEventFun fun)
	{
		NetEventFunCB funcb = std::bind(fun, pBase, std::placeholders::_1, std::placeholders::_2);
		mEventCB.push_back(funcb);
	}

	void RunMsgCB(const NFSOCK nSockIndex, const uint16_t nMsgID, const char* msg, const uint32_t nLen)
	{
		NFPerformance performance;

		if (nMsgID >= static_cast<uint16_t>(mMsgCB.size()) || mMsgCB[nMsgID].size() == 0)
		{
			size_t size = mMsgDefaultCB.size();
			for (size_t i = 0; i < size; ++i)
			{
				mMsgDefaultCB[i](nSockIndex, nMsgID, msg, nLen);
			}
		}
		else
		{
			size_t size = mMsgCB[nMsgID].size();
			for (size_t i = 0; i < size; ++i)
			{
				mMsgCB[nMsgID][i](nSockIndex, nMsgID, msg, nLen);
			}
		}

		//性能检查警告
		if (performance.CheckTimePoint(10))
		{
			QLOG_WARING_S << "[Net CallBack performance]"
						<< " consume:" << performance.TimeScope()
						<< " MsgID:" << nMsgID;
		}
		
		if (m_pPluginManager)
		{
			CATCH_BEGIN
			auto& histogram_family = m_pPluginManager->GetMetricsHistogram(3);
			vector<double> vecTemp;
			if (m_pPluginManager->GetHistogramConfig(3, vecTemp))
			{
				auto& second_histogram = histogram_family.Add(
					{ {"Type", "Tcp"}, {"ID", lexical_cast<std::string>(nMsgID)} }, vecTemp);
				second_histogram.Observe((double)performance.TimeScope());
			}
			CATCH_END
		}

		if (nullptr != m_pPerformanceCountModule)
		{
			m_pPerformanceCountModule->Recording(nMsgID, "Net CallBack", performance.TimeScope());
		}
	}

	void RunEventCB(const NFSOCK nSockIndex, const NET_EVENT nEvent)
	{
		size_t size = mEventCB.size();
		for (size_t i = 0; i < size; ++i)
		{
			mEventCB[i](nSockIndex, nEvent);
		}
	}

	void OpenLog(PerformanceCountModule* pPer, NFIPluginManager* pPluginManager)
	{
		m_pPerformanceCountModule = pPer;
		m_pPluginManager = pPluginManager;
	}

private:
	//接收没有指定注册的消息
	std::vector<NetMsgFunCB> mMsgDefaultCB;
	//指定消息ID接收
	std::vector<std::vector<NetMsgFunCB>> mMsgCB;

	//事件
	std::vector<NetEventFunCB> mEventCB;

	PerformanceCountModule* m_pPerformanceCountModule = nullptr;
	NFIPluginManager* m_pPluginManager = nullptr;
};

#endif //__H_NetCallback_H__