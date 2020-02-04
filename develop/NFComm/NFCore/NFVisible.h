///--------------------------------------------------------------------
/// 文件名:		NFVisible.h
/// 内  容:		可视对象
/// 说  明:		
/// 创建日期:	2019年7月30日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
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

	// 添加视窗
	void AddViewport(NFViewport* pView);
	// 删除视窗
	void RemoveViewport(NFViewport* pView);
	// 关闭所有视窗
	void CloseViewers();
	// 获取视窗数量
	int GetViewCount();
	// 获取视窗列表
	const std::set<NFViewport*>& GetViewList();

	// 通知视窗添加子对象
	virtual void NotifyViewerAdd(const NFGUID& ident, int index);
	// 通知视窗移除子对象
	virtual void NotifyViewerRemove(int index);
	// 通知视窗改变子对象位置
	virtual void NotifyViewerChange(int oldIndex, int newIndex);

protected:

	 // 视窗
	 std::set<NFViewport*> m_setViewport;

	 // 是否正在清空
	 bool m_bClearing = false;
 };

#endif // __NF_VISIBLE_H__