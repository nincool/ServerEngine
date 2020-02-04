///--------------------------------------------------------------------
/// 文件名:		NFViewport.h
/// 内  容:		视窗
/// 说  明:		容器数据同步到客户端
/// 创建日期:	2019年7月30日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NF_VIEWPORT_H__
#define __NF_VIEWPORT_H__

#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFDataList.hpp"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFConfigPlugin/NFClass.h"

class NFVisible;
class NFPlayer;
class _NFExport NFViewport final
{
private:
	NFViewport() {};
public:
	enum HookType
	{
		HOOK_TYPE_NULL = 0,
		HOOK_TYPE_INIT,
		HOOK_TYPE_ADD,
		HOOK_TYPE_REMOVE,
		HOOK_TYPE_CHANGE,
		HOOK_TYPE_CLOSE,
		HOOK_TYPE_TOTAL,
	};
public:
	NFViewport(NFPlayer* pOwn, int id, NFVisible* pContainer, NFIPluginManager* pPluginMgr);
    ~NFViewport();

	// 获得视窗编号
	int GetId() const;
	// 获得视窗所有者
	NFPlayer* GetOwner() const;
	// 获得视窗对应的容器
	NFVisible* GetContainer() const;
	// 通知添加对象
	void NotifyAdd(const NFGUID& ident, int index);
	// 通知移除对象
	void NotifyRemove(int index);
	// 对象改变位置
	void NotifyChange(int old_index, int new_index);
	// 通知关闭视窗
	void NotifyClose();
	// 通知添加所有对象
	void NotifyAll();
	// 重置
	void NotifyReset();

	// 设置添加回调
	static bool SetHook(HookType eHookType, NFIModule* pBase, EVENT_HOOK pHook)
	{
		if (eHookType <= HOOK_TYPE_NULL
			|| eHookType >= HOOK_TYPE_TOTAL)
		{
			return false;
		}

		m_hook[eHookType] = NF_MAKE_SPTR<LogicCallBack< EVENT_HOOK >>(pBase, pHook, 0);
		
		return true;
	}
private:
	// 通知关闭
	void SendClose();
private:
	// 所属对象
	NFPlayer* m_pOwn = nullptr;
	// 编号
	int m_id = 0;
	// 映射的容器
	NFVisible* m_pContainer = nullptr;

	// 插件管理
	NFIPluginManager* m_pPluginManager = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;

	// 回调
	static NF_SHARE_PTR< LogicCallBack< EVENT_HOOK > > m_hook[HOOK_TYPE_TOTAL];
};

#endif // __NF_VIEWPORT_H__
