///--------------------------------------------------------------------
/// �ļ���:		NFVisible.cpp
/// ��  ��:		���Ӷ���
/// ˵  ��:		
/// ��������:	2019��7��30��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFVisible.h"

NFVisible::NFVisible(NFGUID self, NFIPluginManager* pPluginManager) : NFObject(self, pPluginManager)
{

}

NFVisible::~NFVisible()
{
	// �ر��Ӵ�
	CloseViewers();
}

void NFVisible::AddViewport(NFViewport* pView)
{
	assert(pView != nullptr);

	m_setViewport.insert(pView);
}

void NFVisible::RemoveViewport(NFViewport* pView)
{
	// ������գ������ڹ�����ɾ��
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
