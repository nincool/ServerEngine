///--------------------------------------------------------------------
/// 文件名:		NFPropertyManager.cpp
/// 内  容:		属性管理
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFProperty.h"
#include "NFPropertyManager.h"
#include "RawProperty.h"

NFPropertyManager::~NFPropertyManager()
{
    ClearAll();
}

bool NFPropertyManager::RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
    NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strProperty);
    if (pProperty)
    {
        pProperty->RegisterCallback(cb);
        return true;
    }

    return false;
}

NF_SHARE_PTR<NFIProperty> NFPropertyManager::AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty)
{
    const std::string& strProperty = pProperty->GetName();
    NF_SHARE_PTR<NFIProperty> pNewProperty = this->GetElement(strProperty);
    if (pNewProperty == nullptr)
    {
        pNewProperty = NF_MAKE_SPTR<NFProperty>(self, strProperty, pProperty->GetTypeEx());
        this->AddElement(strProperty, pNewProperty);
    }

	pNewProperty->SetPublic(pProperty->GetPublic());
	pNewProperty->SetPrivate(pProperty->GetPrivate());
	pNewProperty->SetSave(pProperty->GetSave());

    return pNewProperty;
}

NF_SHARE_PTR<NFIProperty> NFPropertyManager::AddProperty(const NFGUID& self, const std::string& strPropertyName, const NFDATA_TYPE varType)
{
    NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
    if (pProperty == nullptr)
    {
        pProperty = NF_MAKE_SPTR<NFProperty>(self, strPropertyName, varType);
        this->AddElement(strPropertyName, pProperty);
    }

    return pProperty;
}

NF_SHARE_PTR<NFIProperty> NFPropertyManager::AddRawProperty(
	const std::string& strPropertyName, 
	NFINT64& data, 
	EditType eEdit/* = EDIT_OK*/)
{
	if (mSelf.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " failed. object not match";
		return nullptr;
	}

	NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
	if (pProperty != nullptr)
	{
		this->RemoveElement(strPropertyName);
		pProperty = nullptr;
	}

	pProperty = NF_MAKE_SPTR<CRawPropertyInt64>(mSelf, strPropertyName, data, eEdit);
	this->AddElement(strPropertyName, pProperty);

	return pProperty;
}

NF_SHARE_PTR<NFIProperty> NFPropertyManager::AddRawProperty(
	const std::string& strPropertyName, 
	double& data, 
	EditType eEdit/* = EDIT_OK*/)
{
	if (mSelf.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " failed. object not match";
		return nullptr;
	}

	NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
	if (pProperty != nullptr)
	{
		this->RemoveElement(strPropertyName);
		pProperty = nullptr;
	}

	pProperty = NF_MAKE_SPTR<CRawPropertyDouble>(mSelf, strPropertyName, data, eEdit);
	this->AddElement(strPropertyName, pProperty);

	return pProperty;
}

NF_SHARE_PTR<NFIProperty> NFPropertyManager::AddRawProperty(
	const std::string& strPropertyName, 
	std::string& data, 
	EditType eEdit/* = EDIT_OK*/)
{
	if (mSelf.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " failed. object not match";
		return nullptr;
	}

	NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
	if (pProperty != nullptr)
	{
		this->RemoveElement(strPropertyName);
		pProperty = nullptr;
	}

	pProperty = NF_MAKE_SPTR<CRawPropertyString>(mSelf, strPropertyName, data, eEdit);
	this->AddElement(strPropertyName, pProperty);

	return pProperty;
}

NF_SHARE_PTR<NFIProperty> NFPropertyManager::AddRawProperty(
	const std::string& strPropertyName, 
	NFGUID& data, 
	EditType eEdit/* = EDIT_OK*/)
{
	if (mSelf.IsNull())
	{
		QLOG_ERROR << __FUNCTION__ << " failed. object not match";
		return nullptr;
	}

	NF_SHARE_PTR<NFIProperty> pProperty = this->GetElement(strPropertyName);
	if (pProperty != nullptr)
	{
		this->RemoveElement(strPropertyName);
		pProperty = nullptr;
	}

	pProperty = NF_MAKE_SPTR<CRawPropertyObject>(mSelf, strPropertyName, data, eEdit);
	this->AddElement(strPropertyName, pProperty);

	return pProperty;
}

bool NFPropertyManager::SetRawPropertyEdit(EditType eEdit)
{
	auto pProp = First();
	while (pProp != nullptr)
	{
		auto* pRawProp = dynamic_cast<CRawProperty*>(pProp.get());
		if (pRawProp != nullptr)
		{
			pRawProp->SetEdit(eEdit);
		}

		pProp = Next();
	}

	return true;
}

bool NFPropertyManager::RestoreRawPropertyEdit()
{
	auto pProp = First();
	while (pProp != nullptr)
	{
		auto* pRawProp = dynamic_cast<CRawProperty*>(pProp.get());
		if (pRawProp != nullptr)
		{
			pRawProp->RestoreEdit();
		}

		pProp = Next();
	}

	return true;
}

bool NFPropertyManager::SetProperty(const std::string& strPropertyName, const NFData& TData)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        pProperty->SetValue(TData);

        return true;
    }

    return false;
}

const NFGUID& NFPropertyManager::Self()
{
    return mSelf;
}

std::string NFPropertyManager::ToString()
{
	std::string s;
	std::stringstream stream;
	NF_SHARE_PTR<NFIProperty> pProperty = First(s);
	while (pProperty)
	{
		stream << s << ":" << pProperty->ToString() << "|";
		pProperty = Next(s);
	}

	return stream.str();
}

bool NFPropertyManager::SetPropertyInt(const std::string& strPropertyName, const NFINT64 nValue)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetInt(nValue);
    }

    return false;
}

bool NFPropertyManager::SetPropertyFloat(const std::string& strPropertyName, const double dwValue)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetFloat(dwValue);
    }

    return false;
}

bool NFPropertyManager::SetPropertyString(const std::string& strPropertyName, const std::string& strValue)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetString(strValue);
    }

    return false;
}

bool NFPropertyManager::SetPropertyObject(const std::string& strPropertyName, const NFGUID& obj)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->SetObject(obj);
    }

    return false;
}

NFINT64 NFPropertyManager::GetPropertyInt(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
	return pProperty ? pProperty->GetInt() : 0;
}

double NFPropertyManager::GetPropertyFloat(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
	return pProperty ? pProperty->GetFloat() : 0.0;
}

const std::string& NFPropertyManager::GetPropertyString(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->GetString();
    }

    return NULL_STR;
}

const NFGUID& NFPropertyManager::GetPropertyObject(const std::string& strPropertyName)
{
    NF_SHARE_PTR<NFIProperty> pProperty = GetElement(strPropertyName);
    if (pProperty)
    {
        return pProperty->GetObject();
    }

    return NULL_OBJECT;
}