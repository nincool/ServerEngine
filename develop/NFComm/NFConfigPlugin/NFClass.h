///--------------------------------------------------------------------
/// �ļ���:		NFClass.h
/// ��  ��:		�߼���
/// ˵  ��:		
/// ��������:	2019��8��22��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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

// �¼��ص�
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
	// ���ȼ�
	int m_nPrior = 0;
	// �ص�����
	HOOK_TYPE m_pfHook = nullptr;
	// �ص���������
	NFIModule* m_pOwner = nullptr;
};

// �߼���
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

	// �߼��ඨ������Ժͱ�
	virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() const;
	virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() const;

	// ���߼���
	virtual void SetParent(NF_SHARE_PTR<NFIClass> pClass);
	virtual NF_SHARE_PTR<NFIClass> GetParent() const;

	// �߼�������
	virtual void SetType(OuterMsg::ObjectType eType);
	virtual OuterMsg::ObjectType GetType() const;

	// ��ȡ�߼�����
	virtual const std::string& GetClassName() const;

	// �߼����Ӧ������Config��Ϣ
	virtual const bool AddId(std::string& strId);
	virtual const bool RemoveId(std::string& strId);
	virtual const bool ClearId();
	virtual const std::vector<std::string>& GetIDList() const;

	// �߼����Ӧ������(��ʱֻ���ڷ������б��sql��Ϣ����)
	virtual void SetInstancePath(const std::string& strPath);
	virtual const std::string& GetInstancePath() const;

	// ����¼�
	virtual bool AddEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);
	// �Ƴ��¼�
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner);
	virtual bool RemoveEventCallBack(NFIModule* pOwner);
	
	// �����¼�
	virtual bool FindEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	// ִ���¼�
	virtual int RunEventCallBack(EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// ����ڲ���Ϣ�ص�
	virtual bool AddCommandCallBack(int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);
	// ִ���ڲ���Ϣ�ص�
	virtual int RunCommandCallBack(int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// ��ӿͻ�����Ϣ�ص�
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);
	// ִ�пͻ�����Ϣ�ص�
	virtual int RunCustomCallBack(int nMsgId, const NFGUID& self, const std::string& strMsg);
	// ��Ӹ��ӷ�������Ϣ�ص�
	virtual bool AddExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);
	// ִ�и��ӷ�������Ϣ�ص�
	virtual int RunExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg);

private:
	// ���� ���Ժͱ�
    NF_SHARE_PTR<NFIPropertyManager> m_pPropertyManager = nullptr;
    NF_SHARE_PTR<NFIRecordManager> m_pRecordManager = nullptr;

	// ���߼���
    NF_SHARE_PTR<NFIClass> m_pParentClass = nullptr;
	// ��������
	OuterMsg::ObjectType m_eObjectType = OuterMsg::ObjectType::OBJECT_TYPE_UNKNOW;
    // �߼�����
	std::string m_strClassName = "";
    // ��������·��
	std::string m_strClassInstancePath = "";
	// ��������ID����
    std::vector<std::string> m_IdList;

	// �¼��ص�����
	std::vector<std::list<NF_SHARE_PTR<LogicCallBack<EVENT_HOOK>>>> m_vecEvent;
	std::vector<std::list<NF_SHARE_PTR<LogicCallBack<EVENT_HOOK>>>> m_vecCommand;
	std::vector<std::list<NF_SHARE_PTR<IMethodMsg>>> m_vecCustom;
	std::vector<std::vector<std::list<NF_SHARE_PTR<IMethodMsg>>>> m_vecExtra;

	NFIPluginManager* m_pPluginManager = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	PerformanceCountModule* m_pPerformanceCountModule = nullptr;
};

#endif // __NF_CLASS_H__
