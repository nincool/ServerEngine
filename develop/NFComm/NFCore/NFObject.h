///--------------------------------------------------------------------
/// �ļ���:		NFObject.h
/// ��  ��:		��������
/// ˵  ��:		
/// ��������:	2019��8��9��
/// �޸���:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_OBJECT_H
#define NF_OBJECT_H

#include <string>
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFMessageDefine/NFDefine.pb.h"
#include "NFComm/NFPluginModule/NFIClass.h"
#include "RawProperty.h"

class _NFExport NFObject : public NFIObject
{
private:
    NFObject() : NFIObject(NFGUID())
    {
		mObjectEventState = COE_CREATE_NODATA;
    }

public:
    NFObject(NFGUID self, NFIPluginManager* pPluginManager);
    virtual ~NFObject();

    virtual bool Init();
    virtual bool Shut();

	virtual const NFGUID& Self() const;
	virtual const NFGUID& Parent() const;
	virtual void SetType(OuterMsg::ObjectType eType);
	virtual OuterMsg::ObjectType Type() const;
	virtual void SetConfig(const std::string& strConfig);
	virtual const std::string& Config() const;
	virtual void SetClassName(const std::string& strClassName, const NF_SHARE_PTR<NFIClass>& pClass);
	virtual const std::string& ClassName() const;
	virtual const NF_SHARE_PTR<NFIClass> GetClass() const;
	virtual int Index() const;

	virtual CLASS_OBJECT_EVENT GetState() const;
	virtual bool SetState(const CLASS_OBJECT_EVENT eState);

    virtual bool FindProperty(const std::string& strPropertyName) const;
	virtual NFDATA_TYPE GetPropertyType(const std::string& strPropertyName) const;
	virtual NFDATA_TYPE GetPropertyTypeEx(const std::string& strPropertyName) const;
    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 nValue);
    virtual bool SetPropertyFloat(const std::string& strPropertyName, const double dwValue);
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& strValue);
    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& obj);

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName) const;
    virtual double GetPropertyFloat(const std::string& strPropertyName) const;
    virtual const std::string& GetPropertyString(const std::string& strPropertyName) const;
    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName) const;

    virtual bool FindRecord(const std::string& strRecordName) const;
	virtual NF_SHARE_PTR<NFIRecord> GetRecord(const std::string& strRecordName);

    virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() const;
    virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() const;

	// ������������
	void SetParent(NFObject* pParent);
	// ��ȡ��������
	NFObject* GetParent() const;
	// �����������е�λ��
	void SetIndexInParent(int nIndex);
	// ��ȡ�������е�λ��
	int GetIndexInParent() const;

	// ������������
	virtual bool SetCapacity(int nCap);
	// ��ȡ��������
	virtual int GetCapacity() const;
	// ����δ��ʹ�õ�λ��
	virtual int GetUnusedIndex() const;

	// ����Ӷ���
	virtual bool AddChild(const NFGUID& ident, int index, bool bTest = false);
	// �Ƴ��Ӷ���
	virtual bool RemoveChild(const NFGUID& ident, bool bTest = false);
	// �Ƴ�ָ������ֵ���Ӷ���
	virtual bool RemoveChildByIndex(int index, bool bTest = false);
	// �ƶ��Ӷ���
	virtual bool MoveChild(const NFGUID& ident, NFObject* pOtherContainer, int index);
	// �ƶ��Ӷ����������е�λ��
	virtual bool ChangeChild(const NFGUID& ident, int index);
	// ����Ӷ���
	virtual void ClearChild();
	// ��ȡ�����������
	virtual const NFGUID& GetChildByIndex(int index);
	virtual NF_SHARE_PTR<NFIObject> GetChildObjByIndex(int index);
	// ��ȡ�����������
	virtual const NFGUID& GetChildByName(const std::string& strName);
	virtual NF_SHARE_PTR<NFIObject> GetChildObjByName(const std::string& strName);
	// ��ȡ��������
	virtual int GetChildNum() const;
	//  ��ȡ������С
	virtual int GetChildSize() const;
	// ��ȡ�б�
	virtual const std::vector<NFGUID>& GetChildList() const;

	// �Ƿ�ɼ�
	virtual bool IsVisible() const { return false; }
	// �Ƿ������
	virtual bool IsPlayer() const { return false; }

	// ֪ͨ�Ӵ�����Ӷ���
	virtual void NotifyViewerAdd(const NFGUID& ident, int index) {};
	// ֪ͨ�Ӵ��Ƴ��Ӷ���
	virtual void NotifyViewerRemove(int index) {};
	// ֪ͨ�Ӵ��ı��Ӷ���λ��
	virtual void NotifyViewerChange(int oldIndex, int newIndex) {};

	// �����Ƿ���Ч
	virtual bool IsVaildIndex(int index);

	// �����¼�
	int OnEvent(EventType eType, const NFGUID& sender, const NFDataList& args);
	// �����ڲ���Ϣ
	int OnCommand(int nMsgId, const NFGUID& sender, const NFDataList& args);
	// �����ͻ�����Ϣ
	int OnCustom(int nMsgId, const NFGUID& self, const std::string& strMsg);
	// �������ӷ�������Ϣ
	int OnExtra(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg);

protected:
    virtual bool AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb);

    virtual bool AddPropertyCallBack(const std::string& strCriticalName, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

	virtual void SetRecordManager(NF_SHARE_PTR<NFIRecordManager> xRecordManager);
	virtual void SetPropertyManager(NF_SHARE_PTR<NFIPropertyManager> xPropertyManager);
protected:
	NFIKernelModule* m_pKernelModule = nullptr;

    NFGUID mSelf;
	CLASS_OBJECT_EVENT mObjectEventState;
    NF_SHARE_PTR<NFIRecordManager> m_pRecordManager;
    NF_SHARE_PTR<NFIPropertyManager> m_pPropertyManager;

	// ��Ӧ���߼���
	NF_SHARE_PTR<NFIClass> m_pClass = nullptr;

	// ����������λ��
	NFObject* m_pParent = nullptr;
	int64_t m_nIndexInParent = 0;
	// �߼�����
	std::string m_strClassName = "";
	// ����ID
	std::string m_strConfigID = "";
	// ����
	int64_t m_nType = (int64_t)OuterMsg::ObjectType::OBJECT_TYPE_UNKNOW;

	// �����еĶ���
	std::vector<NFGUID> m_vecChildren;
	// �����ж�������
	int m_nChildNum = 0;
	// ��������
	int64_t m_nCapacity = 0;

	// �ڲ�����
	RAW_STR(Name); // ����
	RAW_STR(ID); // ����GUID���ַ�����ʽ
};

#endif