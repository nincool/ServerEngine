///--------------------------------------------------------------------
/// �ļ���:		NFIClass.h
/// ��  ��:		�߼���ӿ�
/// ˵  ��:		
/// ��������:	2019��8��26��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------


#ifndef __NF_I_CLASS_H__
#define __NF_I_CLASS_H__

#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFPluginModule/KConstDefine.h"

// �߼���
class NFIClass : public NFList<std::string>
{
public:
	virtual ~NFIClass() {};

	virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() const = 0;
	virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() const = 0;

	virtual void SetParent(NF_SHARE_PTR<NFIClass> pClass) = 0;
	virtual NF_SHARE_PTR<NFIClass> GetParent() const = 0;

	virtual void SetType(OuterMsg::ObjectType eType) = 0;
	virtual OuterMsg::ObjectType GetType() const = 0;

	virtual const std::string& GetClassName() const = 0;

	virtual const bool AddId(std::string& strId) = 0;
	virtual const bool RemoveId(std::string& strId) = 0;
	virtual const bool ClearId() = 0;
	virtual const std::vector<std::string>& GetIDList() const = 0;

	virtual void SetInstancePath(const std::string& strPath) = 0;
	virtual const std::string& GetInstancePath() const = 0;

	// ����¼�
	virtual bool AddEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;
	// �Ƴ��¼�
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner) = 0;
	virtual bool RemoveEventCallBack(NFIModule* pOwner) = 0;
	// �����¼�
	virtual bool FindEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;
	// ִ���¼�
	virtual int RunEventCallBack(EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;

	// ����ڲ���Ϣ�ص�
	virtual bool AddCommandCallBack(int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;
	// ִ���ڲ���Ϣ�ص�
	virtual int RunCommandCallBack(int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;
	// ��ӿͻ�����Ϣ�ص�
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;
	// ִ�пͻ�����Ϣ�ص�
	virtual int RunCustomCallBack(int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;
	// ��Ӹ��ӷ�������Ϣ�ص�
	virtual bool AddExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;
	// ִ�и��ӷ�������Ϣ�ص�
	virtual int RunExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;
};

#endif // __NF_I_CLASS_H__
