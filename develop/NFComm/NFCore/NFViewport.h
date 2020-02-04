///--------------------------------------------------------------------
/// �ļ���:		NFViewport.h
/// ��  ��:		�Ӵ�
/// ˵  ��:		��������ͬ�����ͻ���
/// ��������:	2019��7��30��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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

	// ����Ӵ����
	int GetId() const;
	// ����Ӵ�������
	NFPlayer* GetOwner() const;
	// ����Ӵ���Ӧ������
	NFVisible* GetContainer() const;
	// ֪ͨ��Ӷ���
	void NotifyAdd(const NFGUID& ident, int index);
	// ֪ͨ�Ƴ�����
	void NotifyRemove(int index);
	// ����ı�λ��
	void NotifyChange(int old_index, int new_index);
	// ֪ͨ�ر��Ӵ�
	void NotifyClose();
	// ֪ͨ������ж���
	void NotifyAll();
	// ����
	void NotifyReset();

	// ������ӻص�
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
	// ֪ͨ�ر�
	void SendClose();
private:
	// ��������
	NFPlayer* m_pOwn = nullptr;
	// ���
	int m_id = 0;
	// ӳ�������
	NFVisible* m_pContainer = nullptr;

	// �������
	NFIPluginManager* m_pPluginManager = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;

	// �ص�
	static NF_SHARE_PTR< LogicCallBack< EVENT_HOOK > > m_hook[HOOK_TYPE_TOTAL];
};

#endif // __NF_VIEWPORT_H__
