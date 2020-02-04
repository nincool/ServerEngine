///--------------------------------------------------------------------
/// 文件名:		NFClass.cpp
/// 内  容:		逻辑类
/// 说  明:		
/// 创建日期:	2019年8月22日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFClass.h"
#include "NFComm/NFCore/NFPropertyManager.h"
#include "NFComm/NFCore/NFRecordManager.h"
#include "NFComm/NFPublic/Debug.h"
#include "../NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "../NFUtils/QLOG.h"

NFClass::NFClass(NFIPluginManager* pPluginManager, const std::string& strClassName)
{
	m_pPluginManager = pPluginManager;
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pPerformanceCountModule = pPluginManager->FindModule<PerformanceCountModule>();

	m_strClassName = strClassName;

	m_pPropertyManager = NF_SHARE_PTR<NFIPropertyManager>(NF_NEW NFPropertyManager(NFGUID()));
	m_pRecordManager = NF_SHARE_PTR<NFIRecordManager>(NF_NEW NFRecordManager(NFGUID()));

	m_vecEvent.resize(EVENT_CALLBACK_MAX);
	m_vecExtra.resize(NF_SERVER_TYPES::NF_ST_MAX);
}

NFClass::~NFClass()
{
	ClearAll();
}

NF_SHARE_PTR<NFIPropertyManager> NFClass::GetPropertyManager() const
{
	return m_pPropertyManager;
}

NF_SHARE_PTR<NFIRecordManager> NFClass::GetRecordManager() const 
{
	return m_pRecordManager;
}

void NFClass::SetParent(NF_SHARE_PTR<NFIClass> pClass)
{
	m_pParentClass = pClass;
}

NF_SHARE_PTR<NFIClass> NFClass::GetParent() const
{
	return m_pParentClass;
}

void NFClass::SetType(OuterMsg::ObjectType eType)
{
	m_eObjectType = eType;
}

OuterMsg::ObjectType NFClass::GetType() const
{
	return m_eObjectType;
}

const std::string& NFClass::GetClassName() const
{
	return m_strClassName;
}

const bool NFClass::AddId(std::string& strId)
{
	m_IdList.push_back(strId);

	return true;
}

const bool NFClass::RemoveId(std::string& strId)
{
	auto it = std::find(m_IdList.begin(), m_IdList.end(), strId);
	if (it != m_IdList.end())
	{
		m_IdList.erase(it);
		return true;
	}

	return false;
}


const bool NFClass::ClearId()
{
	m_IdList.clear();

	return true;
}

const std::vector<std::string>& NFClass::GetIDList() const
{
	return m_IdList;
}

void NFClass::SetInstancePath(const std::string& strPath)
{
	m_strClassInstancePath = strPath;
}

const std::string& NFClass::GetInstancePath() const
{
	return m_strClassInstancePath;
}

bool NFClass::AddEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior/* = 0*/)
{
	if (eType < EVENT_OnDefault || eType >= EVENT_CALLBACK_MAX)
	{
		QLOG_ERROR << __FUNCTION__ << " out range:" << eType;
		return false;
	}

	if (pOwner == nullptr || pHook == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pOwner or pHook == NULL";
		return false;
	}

	auto pEvent = NF_MAKE_SPTR<LogicCallBack<EVENT_HOOK>>(pOwner, pHook, nPrior);
	if (pEvent == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pEvent == NULL";
		return false;
	}

	// 按照优先级插入
	auto& listCallBack = m_vecEvent[eType];
	for (auto it = listCallBack.begin(); it != listCallBack.end(); ++it)
	{
		auto& pItEvent = *it;
		if (pItEvent == nullptr)
		{
			continue;
		}

		if (pItEvent->GetPrior() > pEvent->GetPrior())
		{
			listCallBack.insert(it, pEvent);
			return true;
		}
	}

	listCallBack.push_back(pEvent);

	return true;
}

bool NFClass::RemoveEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook)
{
	if (pOwner == nullptr || pHook == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pOwner or pHook == NULL";
		return false;
	}

	if (eType < EVENT_OnDefault || eType >= EVENT_CALLBACK_MAX)
	{
		QLOG_ERROR << __FUNCTION__ << " out range:" << eType;
		return false;
	}

	m_vecEvent[eType].remove_if(
		[&](auto& pListEvent)
		{
			return pListEvent->GetOwner() == pOwner && pListEvent->GetHook() == pHook;
		});

	return true;
}

bool NFClass::RemoveEventCallBack(EventType eType, NFIModule* pOwner)
{
	if (pOwner == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pOwner == NULL";
		return false;
	}

	if (eType < EVENT_OnDefault || eType >= EVENT_CALLBACK_MAX)
	{
		QLOG_ERROR << __FUNCTION__ << " out range:" << eType;
		return false;
	}

	m_vecEvent[eType].remove_if(
		[&](auto& pListEvent)
		{
			return pListEvent->GetOwner() == pOwner;
		});

	return true;
}

bool NFClass::RemoveEventCallBack(NFIModule* pOwner)
{
	if (pOwner == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pOwner == NULL";
		return false;
	}

	for (auto& it : m_vecEvent)
	{
		it.remove_if(
			[&](auto& pListEvent)
			{
				return pListEvent->GetOwner() == pOwner;
			});
	}

	return true;
}

bool NFClass::FindEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook)
{
	if (pOwner == nullptr || pHook == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pOwner or pHook == NULL";
		return false;
	}

	if (eType < EVENT_OnDefault || eType >= EVENT_CALLBACK_MAX)
	{
		QLOG_ERROR << __FUNCTION__ << " out range:" << eType;
		return false;
	}

	for (auto& it : m_vecEvent[eType])
	{
		if (it == nullptr)
		{
			continue;
		}

		if (it->GetOwner() == pOwner && it->GetHook() == pHook)
		{
			return true;
		}
	}
	
	return false;
}

int NFClass::RunEventCallBack(EventType eType, 
	const NFGUID& self, 
	const NFGUID& sender, 
	const NFDataList& args)
{
	if (!m_pClassModule->IsOpenEvent())
	{
		return 0;
	}

	if (eType < EVENT_OnDefault || eType >= EVENT_CALLBACK_MAX)
	{
		QLOG_ERROR << __FUNCTION__ << " out range:" << eType;
		return 0;
	}

	NFPerformance performance;

	// 所有事件都执行默认事件回调
	if (!m_vecEvent[EVENT_OnDefault].empty())
	{
		NFDataList argsEx;
		argsEx << eType << GetClassName() << args;
		for (auto& it : m_vecEvent[EVENT_OnDefault])
		{
			if (it == nullptr)
			{
				continue;
			}

			it->Invoke(self, sender, argsEx);
		}
	}

	for (auto& it : m_vecEvent[eType])
	{
		if (it == nullptr)
		{
			continue;
		}

		int nRet = it->Invoke(self, sender, args);
		if (nRet > 0)
		{
			return nRet;
		}
	}

	//性能检查警告
	if (performance.CheckTimePoint(10))
	{
		QLOG_WARING_S << "[Event CallBack performance]"
			<< " consume:" << performance.TimeScope()
			<< " eventType:" << eType
			<< " class:" << m_strClassName;
	}

	CATCH_BEGIN
	auto& histogram_family = m_pPluginManager->GetMetricsHistogram(5);
	vector<double> vecTemp;
	if (m_pPluginManager->GetHistogramConfig(5, vecTemp))
	{
		auto& second_histogram = histogram_family.Add(
			{ {"ClassName", m_strClassName}, {"EventID", lexical_cast<std::string>(eType)} }, vecTemp);
		second_histogram.Observe((double)performance.TimeScope());
	}
	CATCH_END

	m_pPerformanceCountModule->Recording(eType, "Event CallBack", performance.TimeScope());

	if (m_pParentClass != nullptr)
	{
		return m_pParentClass->RunEventCallBack(eType, self, sender, args);
	}

	return 0;
}

bool NFClass::AddCommandCallBack(int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior /*= 0*/)
{
	if (nMsgId < 0)
	{
		QLOG_WARING << __FUNCTION__ << " MsgID below zero:" << nMsgId;
		return false;
	}

	if (pOwner == nullptr || pHook == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pOwner or pHook == NULL";
		return false;
	}

	if (nMsgId >= static_cast<int>(m_vecCommand.size()))
	{
		m_vecCommand.resize(static_cast<size_t>(nMsgId) + COMMAND_SIZE);
	}

	auto pEvent = NF_MAKE_SPTR<LogicCallBack<EVENT_HOOK>>(pOwner, pHook, nPrior);
	if (pEvent == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pEvent == NULL";
		return false;
	}

	// 按照优先级插入
	auto& listCallBack = m_vecCommand[nMsgId];
	for (auto it = listCallBack.begin(); it != listCallBack.end(); ++it)
	{
		auto& pItEvent = *it;
		if (pItEvent == nullptr)
		{
			continue;
		}

		if (pItEvent->GetPrior() > pEvent->GetPrior())
		{
			listCallBack.insert(it, pEvent);
			return true;
		}
	}

	listCallBack.push_back(pEvent);

	return true;
}

int NFClass::RunCommandCallBack(int nMsgId,
	const NFGUID& self,
	const NFGUID& sender,
	const NFDataList& args)
{
	if (nMsgId >= 0 && nMsgId < static_cast<int>(m_vecCommand.size()))
	{
		NFPerformance performance;

		// 默认消息回调
		if (!m_vecCommand[0].empty())
		{
			NFDataList argsEx;
			argsEx << nMsgId << GetClassName() << args;
			for (auto& it : m_vecCommand[0])
			{
				if (it == nullptr)
				{
					continue;
				}

				it->Invoke(self, sender, argsEx);
			}
		}

		for (auto& it : m_vecCommand[nMsgId])
		{
			if (it == nullptr)
			{
				continue;
			}

			int nRet = it->Invoke(self, sender, args);
			if (nRet > 0)
			{
				return nRet;
			}
		}

		//性能检查警告
		if (performance.CheckTimePoint(10))
		{
			QLOG_WARING_S << "[Command CallBack performance]"
				<< ",consume:" << performance.TimeScope()
				<< ",MsgID:" << nMsgId
				<< ",class:" << m_strClassName;
		}

		CATCH_BEGIN
		auto& histogram_family = m_pPluginManager->GetMetricsHistogram(7);
		vector<double> vecTemp;
		if (m_pPluginManager->GetHistogramConfig(7, vecTemp))
		{
			auto& second_histogram = histogram_family.Add(
				{ {"ClassName", m_strClassName}, {"MsgID", lexical_cast<std::string>(nMsgId)} }, vecTemp);
			second_histogram.Observe((double)performance.TimeScope());
		}
		CATCH_END

		m_pPerformanceCountModule->Recording(nMsgId, "Command CallBack", performance.TimeScope());
	}

	if (m_pParentClass != nullptr)
	{
		return m_pParentClass->RunCommandCallBack(nMsgId, self, sender, args);
	}

	return 0;
}

bool NFClass::AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod)
{
	if (nMsgId < 0)
	{
		QLOG_WARING << __FUNCTION__ << " MsgID below zero:" << nMsgId;
		return false;
	}

	if (pMethod == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pMethod == NULL";
		return false;
	}

	if (nMsgId >= static_cast<int>(m_vecCustom.size()))
	{
		m_vecCustom.resize(static_cast<size_t>(nMsgId) + CUSTOM_SIZE);
	}

	// 按照优先级插入
	int nPrior = pMethod->GetPrior();
	auto& listCallBack = m_vecCustom[nMsgId];
	for (auto it = listCallBack.begin(); it != listCallBack.end(); ++it)
	{
		auto& pItMethod = *it;
		if (pItMethod == nullptr)
		{
			continue;
		}

		if (pItMethod->GetPrior() > nPrior)
		{
			listCallBack.insert(it, pMethod);
			return true;
		}
	}

	listCallBack.push_back(pMethod);

	return true;
}

int NFClass::RunCustomCallBack(int nMsgId,
	const NFGUID& self,
	const std::string& strMsg)
{
	if (nMsgId < 0 || nMsgId >= static_cast<int>(m_vecCustom.size()))
	{
		QLOG_WARING << __FUNCTION__ << " MsgID out range:" << nMsgId;
		return 0;
	}

	if (m_vecCustom[nMsgId].empty())
	{
		// 没有接受消息的回调，直接跳出，默认消息也不执行
		return 0;
	}

	auto& pMethod = m_vecCustom[nMsgId].front();
	if (pMethod == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pMethod == NULL";
		return 0;
	}

	google::protobuf::Message& msg = pMethod->GetMsg();
	if (!msg.ParseFromString(strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " google::protobuf::Message ParseFromString failed";
		return 0;
	}

	// 默认消息回调
	for (auto& it : m_vecCustom[0])
	{
		if (it == nullptr)
		{
			continue;
		}

		it->Invoke(nMsgId, self, msg);
	}

	for (auto& it : m_vecCustom[nMsgId])
	{
		if (it == nullptr)
		{
			continue;
		}

		int nRet = it->Invoke(nMsgId, self, msg);
		if (nRet > 0)
		{
			return nRet;
		}
	}

	return 0;
}

bool NFClass::AddExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod)
{
	if (eServerType <= NF_ST_NONE || eServerType >= NF_ST_MAX)
	{
		QLOG_WARING << __FUNCTION__ << " ServerType out range:" << eServerType;
		return false;
	}

	if (pMethod == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pMethod == NULL";
		return false;
	}

	auto& vecMsgList = m_vecExtra[eServerType];
	if (nMsgId >= static_cast<int>(vecMsgList.size()))
	{
		vecMsgList.resize(static_cast<size_t>(nMsgId) + EXTRA_SIZE);
	}

	// 按照优先级插入
	int nPrior = pMethod->GetPrior();
	auto& listCallBack = vecMsgList[nMsgId];
	for (auto it = listCallBack.begin(); it != listCallBack.end(); ++it)
	{
		auto& pItMethod = *it;
		if (pItMethod == nullptr)
		{
			continue;
		}

		if (pItMethod->GetPrior() > nPrior)
		{
			listCallBack.insert(it, pMethod);
			return true;
		}
	}

	listCallBack.push_back(pMethod);

	return true;
}

int NFClass::RunExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg)
{
	if (eServerType <= NF_ST_NONE || eServerType >= NF_ST_MAX)
	{
		QLOG_WARING << __FUNCTION__ << " ServerType out range:" << eServerType;
		return false;
	}

	auto& vecMsgList = m_vecExtra[eServerType];
	if (nMsgId < 0 || nMsgId >= static_cast<int>(vecMsgList.size()))
	{
		return 0;
	}

	if (vecMsgList[nMsgId].empty())
	{
		// 没有接受消息的回调，直接跳出，默认消息也不执行
		return 0;
	}

	auto& pMethod = vecMsgList[nMsgId].front();
	if (pMethod == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " pMethod == NULL";
		return 0;
	}

	google::protobuf::Message& msg = pMethod->GetMsg();
	if (!msg.ParseFromString(strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " google::protobuf::Message ParseFromString failed";
		return 0;
	}

	NFPerformance performance;

	// 默认消息回调
	for (auto& it : vecMsgList[0])
	{
		if (it == nullptr)
		{
			continue;
		}

		it->Invoke(nMsgId, self, msg);
	}

	for (auto& it : vecMsgList[nMsgId])
	{
		if (it == nullptr)
		{
			continue;
		}

		int nRet = it->Invoke(nMsgId, self, msg);
		if (nRet > 0)
		{
			return nRet;
		}
	}

	//性能检查警告
	if (performance.CheckTimePoint(10))
	{
		QLOG_WARING_S << "[Extra CallBack performance]"
			<< " consume:" << performance.TimeScope()
			<< " servertype:" << eServerType
			<< " msgid:" << nMsgId
			<< " class:" << m_strClassName;
	}

	CATCH_BEGIN
	auto& histogram_family = m_pPluginManager->GetMetricsHistogram(8);
	vector<double> vecTemp;
	if (m_pPluginManager->GetHistogramConfig(8, vecTemp))
	{
		auto& second_histogram = histogram_family.Add(
			{ {"ClassName", m_strClassName}, 
			{"ServerType", lexical_cast<std::string>(eServerType)},
			{"MsgID", lexical_cast<std::string>(nMsgId)} }, vecTemp);
		second_histogram.Observe((double)performance.TimeScope());
	}
	CATCH_END

	m_pPerformanceCountModule->Recording(nMsgId, "Extra CallBack", performance.TimeScope());
	return 0;
}
