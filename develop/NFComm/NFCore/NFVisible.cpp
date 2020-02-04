///--------------------------------------------------------------------
/// 文件名:		NFVisible.cpp
/// 内  容:		可视对象
/// 说  明:		
/// 创建日期:	2019年7月30日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFVisible.h"

NFVisible::NFVisible(NFGUID self, NFIPluginManager* pPluginManager) : NFObject(self, pPluginManager)
{

}

NFVisible::~NFVisible()
{
	// 关闭视窗
	CloseViewers();
}

void NFVisible::AddViewport(NFViewport* pView)
{
	assert(pView != nullptr);

	m_setViewport.insert(pView);
}

void NFVisible::RemoveViewport(NFViewport* pView)
{
	// 正在清空，不能在过程中删除
	if (m_bClearing)
	{
		return;
	}

	assert(pView != nullptr);

	m_setViewport.erase(pView);
}

void NFVisible::CloseViewers()
{
	m_bClearing = true;
	for (auto& it : m_setViewport)
	{
		it->NotifyClose();
	}

	m_setViewport.clear();

	m_bClearing = false;
}

int NFVisible::GetViewCount()
{
	return static_cast<int>(m_setViewport.size());
}

const std::set<NFViewport*>& NFVisible::GetViewList()
{
	return m_setViewport;
}

void NFVisible::NotifyViewerAdd(const NFGUID& ident, int index)
{
	for (auto& it : m_setViewport)
	{
		it->NotifyAdd(ident, index);
	}
}

void NFVisible::NotifyViewerRemove(int index)
{
	for (auto& it : m_setViewport)
	{
		it->NotifyRemove(index);
	}
}

void NFVisible::NotifyViewerChange(int oldIndex, int newIndex)
{
	for (auto& it : m_setViewport)
	{
		it->NotifyChange(oldIndex, newIndex);
	}
}
