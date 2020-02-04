///--------------------------------------------------------------------
/// 文件名:		NFObject.cpp
/// 内  容:		基础对象
/// 说  明:		
/// 创建日期:	2019年8月9日
/// 修改人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFObject.h"
#include "NFRecordManager.h"
#include "NFPropertyManager.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFUtils/QLOG.h"

NFObject::NFObject(NFGUID self, NFIPluginManager* pPluginManager) : NFIObject(self)
{
	mObjectEventState = COE_CREATE_NODATA;

    mSelf = self;
	SetID(self.ToString());

    m_pPluginManager = pPluginManager;

    m_pRecordManager = NF_SHARE_PTR<NFRecordManager>(NF_NEW NFRecordManager(self));
    m_pPropertyManager = NF_SHARE_PTR<NFPropertyManager>(NF_NEW NFPropertyManager(self));

	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();

	// 添加内部属性
	ADD_RAW_STR(m_pPropertyManager, Name, EDIT_OK, true);
	ADD_RAW_STR(m_pPropertyManager, ID, EDIT_NO, false);
	ADD_RAW(m_pPropertyManager, Type, m_nType, EDIT_NO, false);
	ADD_RAW(m_pPropertyManager, ClassName, m_strClassName, EDIT_NO, true);
	ADD_RAW(m_pPropertyManager, ConfigID, m_strConfigID, EDIT_NO, true);
	ADD_RAW(m_pPropertyManager, Capacity, m_nCapacity, EDIT_NO, true);
	ADD_RAW(m_pPropertyManager, Index, m_nIndexInParent, EDIT_NO, true);
	ADD_RAW(m_pPropertyManager, Guid, mSelf, EDIT_NO, false);

}

NFObject::~NFObject()
{
	ClearChild();
}

bool NFObject::Init()
{
    return true;
}

bool NFObject::Shut()
{
    return true;
}

const NFGUID& NFObject::Self() const
{
	return mSelf;
}

const NFGUID& NFObject::Parent() const
{
	if (nullptr == m_pParent)
	{
		return NULL_OBJECT;
	}
	return m_pParent->Self();
}


void NFObject::SetType(OuterMsg::ObjectType eType)
{
	m_nType = (int64_t)eType;
}

OuterMsg::ObjectType NFObject::Type() const
{
	return (OuterMsg::ObjectType)m_nType;
}

void NFObject::SetConfig(const std::string& strConfig)
{
	m_strConfigID = strConfig;
}

const std::string& NFObject::Config() const
{
	return m_strConfigID;
}

void NFObject::SetClassName(const std::string& strClassName, const NF_SHARE_PTR<NFIClass>& pClass)
{
	m_strClassName = strClassName;
	m_pClass = pClass;
}

const std::string& NFObject::ClassName() const
{
	return m_strClassName;
}

const NF_SHARE_PTR<NFIClass> NFObject::GetClass() const
{
	return m_pClass;
}

int NFObject::Index() const
{
	return GetIndexInParent();
}

bool NFObject::AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetRecordManager()->GetElement(strRecordName);
    if (pRecord)
    {
        pRecord->AddRecordHook(cb);

        return true;
    }

    return false;
}

bool NFObject::AddPropertyCallBack(const std::string& strCriticalName, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strCriticalName);
    if (pProperty)
    {
        pProperty->RegisterCallback(cb);

        return true;
    }

    return false;
}

CLASS_OBJECT_EVENT NFObject::GetState() const
{
	return mObjectEventState;
}

bool NFObject::SetState(const CLASS_OBJECT_EVENT eState)
{
	mObjectEventState = eState;
	return true;
}

bool NFObject::FindProperty(const std::string& strPropertyName) const
{
    if (GetPropertyManager()->GetElement(strPropertyName))
    {
        return true;
    }

    return false;
}

NFDATA_TYPE NFObject::GetPropertyType(const std::string& strPropertyName) const
{
	NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
	if (pProperty == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pProperty == NULL name:" << strPropertyName;
		return TDATA_UNKNOWN;
	}

	return pProperty->GetType();
}

NFDATA_TYPE NFObject::GetPropertyTypeEx(const std::string& strPropertyName) const
{
	NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
	if (pProperty == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pProperty == NULL name:" << strPropertyName;
		return TDATA_UNKNOWN;
	}

	return pProperty->GetTypeEx();
}

bool NFObject::SetPropertyInt(const std::string& strPropertyName, const NFINT64 nValue)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetInt(nValue);
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return false;
}

bool NFObject::SetPropertyFloat(const std::string& strPropertyName, const double dwValue)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetFloat(dwValue);
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return false;
}

bool NFObject::SetPropertyString(const std::string& strPropertyName, const std::string& strValue)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetString(strValue);
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return false;
}

bool NFObject::SetPropertyObject(const std::string& strPropertyName, const NFGUID& obj)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetObject(obj);
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return false;
}

NFINT64 NFObject::GetPropertyInt(const std::string& strPropertyName) const
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->GetInt();
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return 0;
}

double NFObject::GetPropertyFloat(const std::string& strPropertyName) const
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->GetFloat();
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return 0.0;
}

const std::string& NFObject::GetPropertyString(const std::string& strPropertyName) const
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->GetString();
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return NULL_STR;
}

const NFGUID& NFObject::GetPropertyObject(const std::string& strPropertyName) const
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetPropertyManager()->GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->GetObject();
    }
	else
	{
		QLOG_WARING << __FUNCTION__ << " prop:" << strPropertyName << " not exist. Config:" << Config();
	}

    return NULL_OBJECT;
}

bool NFObject::FindRecord(const std::string& strRecordName) const
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetRecordManager()->GetElement(strRecordName);
    if (pRecord)
    {
        return true;
    }

    return false;
}

NF_SHARE_PTR<NFIRecord> NFObject::GetRecord(const std::string& strRecordName)
{
	return GetRecordManager()->GetElement(strRecordName);
}

NF_SHARE_PTR<NFIRecordManager> NFObject::GetRecordManager() const
{
    return m_pRecordManager;
}

NF_SHARE_PTR<NFIPropertyManager> NFObject::GetPropertyManager() const
{
    return m_pPropertyManager;
}

void NFObject::SetRecordManager(NF_SHARE_PTR<NFIRecordManager> xRecordManager)
{
	m_pRecordManager = xRecordManager;
}

void NFObject::SetPropertyManager(NF_SHARE_PTR<NFIPropertyManager> xPropertyManager)
{
	m_pPropertyManager = xPropertyManager;
}

void NFObject::SetParent(NFObject* pParent)
{
	m_pParent = pParent;
}

NFObject* NFObject::GetParent() const
{
	return m_pParent;
}

void NFObject::SetIndexInParent(int nIndex)
{
	m_nIndexInParent = nIndex;
}

int NFObject::GetIndexInParent() const
{
	return m_pParent != nullptr ? m_nIndexInParent : -1;
}

const std::vector<NFGUID>& NFObject::GetChildList() const
{
	return m_vecChildren;
}

int NFObject::OnEvent(EventType eType, const NFGUID& sender, const NFDataList& args)
{
	if (m_pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " m_pClass == NULL";
		return 0;
	}

	return m_pClass->RunEventCallBack(eType, Self(), sender, args);
}

int NFObject::OnCommand(int nMsgId, const NFGUID& sender, const NFDataList& args)
{
	if (m_pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " m_pClass == NULL";
		return 0;
	}

	return m_pClass->RunCommandCallBack(nMsgId, Self(), sender, args);
}

int NFObject::OnCustom(int nMsgId, const NFGUID& self, const std::string& strMsg)
{
	if (m_pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " m_pClass == NULL";
		return 0;
	}

	return m_pClass->RunCustomCallBack(nMsgId, self, strMsg);
}

int NFObject::OnExtra(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg)
{
	if (m_pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " m_pClass == NULL";
		return 0;
	}

	return m_pClass->RunExtraCallBack(eServerType, nMsgId, self, strMsg);
}