///--------------------------------------------------------------------
/// 文件名:		NFClass.h
/// 内  容:		逻辑类
/// 说  明:		
/// 创建日期:	2019年8月22日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------


#ifndef __NF_CLASS_H__
#define __NF_CLASS_H__

#include <string>
#include <vector>
#include <list>
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFPluginModule/NFIClass.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "../NFPluginModule/NFIClassModule.h"
#include "NFComm/NFKernelPlugin/PerformanceCountModule.h"

// 事件回调
template<class HOOK_TYPE>
class LogicCallBack final
{
private:
	LogicCallBack() {};
public:

	LogicCallBack(NFIModule* pOwner, HOOK_TYPE pf, int nPrior)
	{
		assert(pOwner != nullptr);
		assert(pf != nullptr);

		m_pOwner = pOwner;
		m_pfHook = pf;
		m_nPrior = nPrior;
	}

	int Invoke(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
	{
		return (m_pOwner->*m_pfHook)(self, sender, args);
	}

	int GetPrior() const
	{
		return m_nPrior;
	}

	const EVENT_HOOK GetHook() const
	{
		return m_pfHook;
	}

	const NFIModule* GetOwner() const
	{
		return m_pOwner;
	}

private:
	// 优先级
	int m_nPrior = 0;
	// 回调函数
	HOOK_TYPE m_pfHook = nullptr;
	// 回调所属对象
	NFIModule* m_pOwner = nullptr;
};

// 逻辑类
class NFClass final : public NFIClass
{
private:
	NFClass() {};
	enum 
	{
		COMMAND_SIZE = 100,
		CUSTOM_SIZE = 1000,
		EXTRA_SIZE = 100,
	};
public:

	NFClass(NFIPluginManager* pPluginManager, const std::string& strClassName);
	virtual ~NFClass();

	// 逻辑类定义的属性和表
	virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() const;
	virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() const;

	// 父逻辑类
	virtual void SetParent(NF_SHARE_PTR<NFIClass> pClass);
	virtual NF_SHARE_PTR<NFIClass> GetParent() const;

	// 逻辑类类型
	virtual void SetType(OuterMsg::ObjectType eType);
	virtual OuterMsg::ObjectType GetType() const;

	// 获取逻辑类名
	virtual const std::string& GetClassName() const;

	// 逻辑类对应的数据Config信息
	virtual const bool AddId(std::string& strId);
	virtual const bool RemoveId(std::string& strId);
	virtual const bool ClearId();
	virtual const std::vector<std::string>& GetIDList() const;

	// 逻辑类对应的数据(暂时只用于服务器列表和sql信息配置)
	virtual void SetInstancePath(const std::string& strPath);
	virtual const std::string& GetInstancePath() const;

	// 添加事件
	virtual bool AddEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);
	// 移除事件
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner);
	virtual bool RemoveEventCallBack(NFIModule* pOwner);
	
	// 查找事件
	virtual bool FindEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	// 执行事件
	virtual int RunEventCallBack(EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// 添加内部消息回调
	virtual bool AddCommandCallBack(int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);
	// 执行内部消息回调
	virtual int RunCommandCallBack(int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 添加客户端消息回调
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);
	// 执行客户端消息回调
	virtual int RunCustomCallBack(int nMsgId, const NFGUID& self, const std::string& strMsg);
	// 添加附加服务器消息回调
	virtual bool AddExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);
	// 执行附加服务器消息回调
	virtual int RunExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg);

private:
	// 数据 属性和表
    NF_SHARE_PTR<NFIPropertyManager> m_pPropertyManager = nullptr;
    NF_SHARE_PTR<NFIRecordManager> m_pRecordManager = nullptr;

	// 父逻辑类
    NF_SHARE_PTR<NFIClass> m_pParentClass = nullptr;
	// 对象类型
	OuterMsg::ObjectType m_eObjectType = OuterMsg::ObjectType::OBJECT_TYPE_UNKNOW;
    // 逻辑类名
	std::string m_strClassName = "";
    // 配置数据路径
	std::string m_strClassInstancePath = "";
	// 配置数据ID集合
    std::vector<std::string> m_IdList;

	// 事件回调集合
	std::vector<std::list<NF_SHARE_PTR<LogicCallBack<EVENT_HOOK>>>> m_vecEvent;
	std::vector<std::list<NF_SHARE_PTR<LogicCallBack<EVENT_HOOK>>>> m_vecCommand;
	std::vector<std::list<NF_SHARE_PTR<IMethodMsg>>> m_vecCustom;
	std::vector<std::vector<std::list<NF_SHARE_PTR<IMethodMsg>>>> m_vecExtra;

	NFIPluginManager* m_pPluginManager = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	PerformanceCountModule* m_pPerformanceCountModule = nullptr;
};

#endif // __NF_CLASS_H__
