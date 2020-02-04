///--------------------------------------------------------------------
/// 文件名:		NFViewport.cpp
/// 内  容:		视窗
/// 说  明:		容器数据同步到客户端
/// 创建日期:	2019年7月30日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFViewport.h"
#include "NFPlayer.h"
#include "NFVisible.h"

NF_SHARE_PTR<LogicCallBack<EVENT_HOOK>> NFViewport::m_hook[HOOK_TYPE_TOTAL] = {nullptr};

NFViewport::NFViewport(NFPlayer* pOwn, int id, NFVisible* pContainer, NFIPluginManager* pPluginMgr)
{
	assert(pContainer != nullptr);
	assert(pPluginMgr != nullptr);
	assert(pOwn != nullptr);

	m_pOwn = pOwn;
	m_id = id;
	m_pContainer = pContainer;
	assert(pContainer != nullptr);
	m_pContainer->AddViewport(this);
	m_pPluginManager = pPluginMgr;
	m_pKernelModule = pPluginMgr->FindModule<NFIKernelModule>();
	
	NotifyAll();
}

NFViewport::~NFViewport()
{
	if (m_pContainer != nullptr)
	{
		m_pContainer->RemoveViewport(this);
	}

	// 通知客户端关闭视窗
	SendClose();

	m_pOwn = nullptr;
	m_pContainer = nullptr;
}

int NFViewport::GetId() const
{
	return m_id;
}

NFPlayer* NFViewport::GetOwner() const
{
	return m_pOwn;
}

NFVisible* NFViewport::GetContainer() const
{
	return m_pContainer;
}

void NFViewport::NotifyAdd(const NFGUID& ident, int index)
{
	if (m_pOwn != nullptr
		&& !ident.IsNull()
		&& m_hook[HOOK_TYPE_ADD] != nullptr)
	{
		m_hook[HOOK_TYPE_ADD]->Invoke(m_pOwn->Self(), ident, NFDataList() << m_id << index);
	}
}

void NFViewport::NotifyRemove(int index)
{
	if (m_pOwn != nullptr
		&& m_hook[HOOK_TYPE_REMOVE] != nullptr)
	{
		m_hook[HOOK_TYPE_REMOVE]->Invoke(m_pOwn->Self(), NULL_OBJECT, NFDataList() << m_id << index);
	}
}

void NFViewport::NotifyChange(int old_index, int new_index)
{
	if (m_pOwn != nullptr
		&& m_hook[HOOK_TYPE_REMOVE] != nullptr)
	{
		m_hook[HOOK_TYPE_REMOVE]->Invoke(m_pOwn->Self(), NULL_OBJECT, NFDataList() << m_id << old_index << new_index);
	}
}

void NFViewport::NotifyClose()
{
	if (m_pOwn != nullptr)
	{
		m_pOwn->RemoveViewport(m_id);
	}
}

void NFViewport::NotifyAll()
{
	if (m_pOwn == nullptr
		|| m_pContainer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " m_pOwn or m_pContainer == NULL";
		return;
	}

	if (m_hook[HOOK_TYPE_INIT] != nullptr)
	{
		m_hook[HOOK_TYPE_INIT]->Invoke(m_pOwn->Self(), m_pContainer->Self(), NFDataList() << m_id );
	}

	for (int i = 0; i < m_pContainer->GetChildSize(); ++i)
	{
		NotifyAdd(m_pContainer->GetChildByIndex(i), i);
	}
}

void NFViewport::NotifyReset()
{
	SendClose();
	NotifyAll();
}

void NFViewport::SendClose()
{
	if (m_pOwn != nullptr
		&& m_hook[HOOK_TYPE_CLOSE] != nullptr)
	{
		m_hook[HOOK_TYPE_CLOSE]->Invoke(m_pOwn->Self(), NULL_OBJECT, NFDataList() << m_id);
	}
}