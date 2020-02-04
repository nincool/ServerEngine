///--------------------------------------------------------------------
/// 文件名:		NFObject.h
/// 内  容:		基础对象
/// 说  明:		
/// 创建日期:	2019年8月9日
/// 修改人:		谢宇
/// 版权所有:	血帆海盗团
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

	// 设置所在容器
	void SetParent(NFObject* pParent);
	// 获取所在容器
	NFObject* GetParent() const;
	// 设置在容器中的位置
	void SetIndexInParent(int nIndex);
	// 获取在容器中的位置
	int GetIndexInParent() const;

	// 设置容器容量
	virtual bool SetCapacity(int nCap);
	// 获取容器容量
	virtual int GetCapacity() const;
	// 查找未被使用的位置
	virtual int GetUnusedIndex() const;

	// 添加子对象
	virtual bool AddChild(const NFGUID& ident, int index, bool bTest = false);
	// 移除子对象
	virtual bool RemoveChild(const NFGUID& ident, bool bTest = false);
	// 移除指定索引值的子对象
	virtual bool RemoveChildByIndex(int index, bool bTest = false);
	// 移动子对象
	virtual bool MoveChild(const NFGUID& ident, NFObject* pOtherContainer, int index);
	// 移动子对象在容器中的位置
	virtual bool ChangeChild(const NFGUID& ident, int index);
	// 清空子对象
	virtual void ClearChild();
	// 获取对象根据索引
	virtual const NFGUID& GetChildByIndex(int index);
	virtual NF_SHARE_PTR<NFIObject> GetChildObjByIndex(int index);
	// 获取对象根据名称
	virtual const NFGUID& GetChildByName(const std::string& strName);
	virtual NF_SHARE_PTR<NFIObject> GetChildObjByName(const std::string& strName);
	// 获取对象数量
	virtual int GetChildNum() const;
	//  获取容器大小
	virtual int GetChildSize() const;
	// 获取列表
	virtual const std::vector<NFGUID>& GetChildList() const;

	// 是否可见
	virtual bool IsVisible() const { return false; }
	// 是否是玩家
	virtual bool IsPlayer() const { return false; }

	// 通知视窗添加子对象
	virtual void NotifyViewerAdd(const NFGUID& ident, int index) {};
	// 通知视窗移除子对象
	virtual void NotifyViewerRemove(int index) {};
	// 通知视窗改变子对象位置
	virtual void NotifyViewerChange(int oldIndex, int newIndex) {};

	// 索引是否有效
	virtual bool IsVaildIndex(int index);

	// 触发事件
	int OnEvent(EventType eType, const NFGUID& sender, const NFDataList& args);
	// 触发内部消息
	int OnCommand(int nMsgId, const NFGUID& sender, const NFDataList& args);
	// 触发客户端消息
	int OnCustom(int nMsgId, const NFGUID& self, const std::string& strMsg);
	// 触发附加服务器消息
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

	// 对应的逻辑类
	NF_SHARE_PTR<NFIClass> m_pClass = nullptr;

	// 所在容器及位置
	NFObject* m_pParent = nullptr;
	int64_t m_nIndexInParent = 0;
	// 逻辑类名
	std::string m_strClassName = "";
	// 配置ID
	std::string m_strConfigID = "";
	// 类型
	int64_t m_nType = (int64_t)OuterMsg::ObjectType::OBJECT_TYPE_UNKNOW;

	// 容器中的对象
	std::vector<NFGUID> m_vecChildren;
	// 容器中对象数量
	int m_nChildNum = 0;
	// 容器容量
	int64_t m_nCapacity = 0;

	// 内部属性
	RAW_STR(Name); // 名称
	RAW_STR(ID); // 自身GUID的字符串形式
};

#endif