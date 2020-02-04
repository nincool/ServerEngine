///--------------------------------------------------------------------
/// �ļ���:		NFVisible.h
/// ��  ��:		���Ӷ���
/// ˵  ��:		
/// ��������:	2019��7��30��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_VISIBLE_H__
#define __NF_VISIBLE_H__

#include "NFObject.h"
#include "NFViewport.h"
#include <set>

 class _NFExport NFVisible : public NFObject
 {
 private:
	 NFVisible() : NFObject(NFGUID(), nullptr) {};

 public:
	 NFVisible(NFGUID self, NFIPluginManager* pPluginManager);
	virtual ~NFVisible();

	virtual bool IsVisible() const { return true; }

	// ����Ӵ�
	void AddViewport(NFViewport* pView);
	// ɾ���Ӵ�
	void RemoveViewport(NFViewport* pView);
	// �ر������Ӵ�
	void CloseViewers();
	// ��ȡ�Ӵ�����
	int GetViewCount();
	// ��ȡ�Ӵ��б�
	const std::set<NFViewport*>& GetViewList();

	// ֪ͨ�Ӵ�����Ӷ���
	virtual void NotifyViewerAdd(const NFGUID& ident, int index);
	// ֪ͨ�Ӵ��Ƴ��Ӷ���
	virtual void NotifyViewerRemove(int index);
	// ֪ͨ�Ӵ��ı��Ӷ���λ��
	virtual void NotifyViewerChange(int oldIndex, int newIndex);

protected:

	 // �Ӵ�
	 std::set<NFViewport*> m_setViewport;

	 // �Ƿ��������
	 bool m_bClearing = false;
 };

#endif // __NF_VISIBLE_H__