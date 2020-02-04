///--------------------------------------------------------------------
/// 文件名:		NFProperty.cpp
/// 内  容:		属性
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFProperty.h"
#include <complex>
#include "NFComm/NFUtils/QLOG.h"

NFProperty::NFProperty()
{
	mbPublic = false;
	mbPrivate = false;
	mbSave = false;

	mSelf = NFGUID();
	m_eType = TDATA_UNKNOWN;

	msPropertyName = "";
}

NFProperty::NFProperty(const NFGUID& self, const std::string& strPropertyName, const NFDATA_TYPE varType)
{
	mbPublic = false;
	mbPrivate = false;
	mbSave = false;

	mSelf = self;

	msPropertyName = strPropertyName;
	m_eType = varType;
}

NFProperty::~NFProperty()
{
	for (TPROPERTYCALLBACKEX::iterator iter = mtPropertyCallback.begin(); iter != mtPropertyCallback.end(); ++iter)
	{
		iter->reset();
	}

	mtPropertyCallback.clear();
	mxData.reset();
}

void NFProperty::SetValue(const NFData& xData)
{
	NFDATA_TYPE eType = GetType();
	if (eType != xData.GetType()
		|| xData.GetType() == NFDATA_TYPE::TDATA_UNKNOWN)
	{
		QLOG_WARING << __FUNCTION__ << " type different property type:" << eType
			<< " data type:" << xData.GetType();
		return;
	}

	switch (eType)
	{
	case TDATA_INT:
		SetInt(xData.GetInt());
		break;
	case TDATA_FLOAT:
		SetFloat(xData.GetFloat());
		break;
	case TDATA_STRING:
		SetString(xData.GetString());
		break;
	case TDATA_OBJECT:
		SetObject(xData.GetObject());
		break;
	}
}

void NFProperty::SetValue(NFData&& xData)
{
	if (IsMember())
	{
		SetValue((const NFData&)xData);
	}
	else
	{
		*mxData = std::move(xData);
	}
}

void NFProperty::SetValue(const NFIProperty* pProperty)
{
	NFDATA_TYPE eType = GetType();
	if (pProperty == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pProperty == NULL";
		return;
	}

	if (eType != pProperty->GetType())
	{
		QLOG_WARING << __FUNCTION__ << " type different property type:" << eType
			<< " Property type:" << pProperty->GetType();
		return;
	}

	if (pProperty->IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " pProperty IsNull";
		return;
	}

	switch (eType)
	{
	case TDATA_INT:
		SetInt(pProperty->GetInt());
		break;
	case TDATA_FLOAT:
		SetFloat(pProperty->GetFloat());
		break;
	case TDATA_STRING:
		SetString(pProperty->GetString());
		break;
	case TDATA_OBJECT:
		SetObject(pProperty->GetObject());
		break;
	}
}

const std::string&  NFProperty::GetName() const
{
	return msPropertyName;
}

const bool NFProperty::GetSave() const
{
	return mbSave;
}

const bool NFProperty::GetPublic() const
{
	return mbPublic;
}

const bool NFProperty::GetPrivate() const
{
	return mbPrivate;
}

const int NFProperty::GetTCallBackCount() const
{
	return static_cast<int>(mtPropertyCallback.size());
}

const bool NFProperty::IsMember() const
{
	return false;
}

void NFProperty::SetSave(bool bSave)
{
	// 内部属性被设置为保存，则不允许再改为不保存
	if (IsMember() && mbSave && !bSave)
	{
		QLOG_ERROR << __FUNCTION__ << " member property cant set save true to false";
		return;
	}

	mbSave = bSave;
}

void NFProperty::SetPublic(bool bPublic)
{
	mbPublic = bPublic;
}

void NFProperty::SetPrivate(bool bPrivate)
{
	mbPrivate = bPrivate;
}

NFINT64 NFProperty::GetInt() const
{
	if (!mxData)
	{
		return 0;
	}

	return mxData->GetInt();
}

double NFProperty::GetFloat() const
{
	if (!mxData)
	{
		return 0.0;
	}

	return mxData->GetFloat();
}

const std::string& NFProperty::GetString() const
{
	if (!mxData)
	{
		return NULL_STR;
	}

	return mxData->GetString();
}

const NFGUID& NFProperty::GetObject() const
{
	if (!mxData)
	{
		return NULL_OBJECT;
	}

	return mxData->GetObject();
}

void NFProperty::RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
	mtPropertyCallback.push_back(cb);
}

int NFProperty::OnEventHandler(const NFData& oldVar, const NFData& newVar)
{
	if (mtPropertyCallback.size() <= 0)
	{
		return 0;
	}

	TPROPERTYCALLBACKEX::iterator it = mtPropertyCallback.begin();
	TPROPERTYCALLBACKEX::iterator end = mtPropertyCallback.end();
	for (; it != end; ++it)
	{
		PROPERTY_EVENT_FUNCTOR_PTR& pFunPtr = *it;
		PROPERTY_EVENT_FUNCTOR* pFunc = pFunPtr.get();
		pFunc->operator()(mSelf, msPropertyName, oldVar, newVar);
	}

	return 0;
}

bool NFProperty::SetInt(const NFINT64 value)
{
	if (GetType() != TDATA_INT)
	{
		return false;
	}

	if (!mxData)
	{
		if (0 == value)
		{
			return false;
		}

		mxData = NF_SHARE_PTR<NFData>(NF_NEW NFData(GetTypeEx()));
		mxData->SetInt(0);
	}

	if (value == mxData->GetInt())
	{
		return false;
	}

	if (mtPropertyCallback.size() == 0)
	{
		mxData->SetInt(value);
	}
	else
	{
		NFData oldValue;
		oldValue = *mxData;

		mxData->SetInt(value);

		OnEventHandler(oldValue, *mxData);
	}

	return true;
}

bool NFProperty::SetFloat(const double value)
{
	if (GetType() != TDATA_FLOAT)
	{
		return false;
	}

	if (!mxData)
	{
		if (IsZeroDouble(value))
		{
			return false;
		}

		mxData = NF_SHARE_PTR<NFData>(NF_NEW NFData(GetTypeEx()));
		mxData->SetFloat(0.0);
	}

	if (IsZeroDouble(value - mxData->GetFloat()))
	{
		return false;
	}

	if (mtPropertyCallback.size() == 0)
	{
		mxData->SetFloat(value);
	}
	else
	{
		NFData oldValue;
		oldValue = *mxData;

		mxData->SetFloat(value);

		OnEventHandler(oldValue, *mxData);
	}

	return true;
}

bool NFProperty::SetString(const std::string& value)
{
	return InnerSetString(value);
}

bool NFProperty::SetString(std::string&& value)
{
	return InnerSetString(std::move(value));
}

template<class T>
bool NFProperty::InnerSetString(T&& value)
{
	if (GetType() != TDATA_STRING)
	{
		return false;
	}

	if (!mxData)
	{
		if (value.empty())
		{
			return false;
		}

		mxData = NF_SHARE_PTR<NFData>(NF_NEW NFData(GetTypeEx()));
		mxData->SetString(NULL_STR);
	}

	if (value == mxData->GetString())
	{
		return false;
	}

	if (mtPropertyCallback.size() == 0)
	{
		mxData->SetString(std::forward<T>(value));
	}
	else
	{
		NFData oldValue;
		oldValue = *mxData;

		mxData->SetString(std::forward<T>(value));

		OnEventHandler(oldValue, *mxData);
	}

	return true;
}

bool NFProperty::SetObject(const NFGUID& value)
{
	if (GetType() != TDATA_OBJECT)
	{
		return false;
	}

	if (!mxData)
	{
		if (value.IsNull())
		{
			return false;
		}

		mxData = NF_SHARE_PTR<NFData>(NF_NEW NFData(GetTypeEx()));
		mxData->SetObject(NFGUID());
	}

	if (value == mxData->GetObject())
	{
		return false;
	}

	if (mtPropertyCallback.size() == 0)
	{
		mxData->SetObject(value);
	}
	else
	{
		NFData oldValue;
		oldValue = *mxData;

		mxData->SetObject(value);

		OnEventHandler(oldValue, *mxData);
	}

	return true;
}

bool NFProperty::IsNull() const
{
	return mxData == nullptr || mxData->IsNullValue();
}

bool NFProperty::CanEdit() const
{
	return true;
}

void NFProperty::Clear()
{
	if (IsNull())
	{
		return;
	}

	const NFDATA_TYPE eType = GetType();
	switch (eType)
	{
	case TDATA_INT:
		SetInt(NULL_INT);
		break;
	case TDATA_FLOAT:
		SetFloat(NULL_FLOAT);
		break;
	case TDATA_STRING:
		SetString(NULL_STR);
		break;
	case TDATA_OBJECT:
		SetObject(NULL_OBJECT);
		break;
	}
}

const NFDATA_TYPE NFProperty::GetType() const
{
	return (NFDATA_TYPE)short(m_eType);
}

const NFDATA_TYPE NFProperty::GetTypeEx() const
{
	return m_eType;
}

std::string NFProperty::ToString()
{
	std::string strData;
	const NFDATA_TYPE eType = GetType();
	switch (eType)
	{
	case TDATA_INT:
		strData = lexical_cast<std::string>(GetInt());
		break;
	case TDATA_FLOAT:
		strData = lexical_cast<std::string>(GetFloat());
		break;
	case TDATA_STRING:
		strData = GetString();
		break;
	case TDATA_OBJECT:
		strData = GetObject().ToString();
		break;
	default:
		strData = NULL_STR;
		break;
	}

	return std::move(strData);
}

bool NFProperty::FromString(const std::string& strData)
{
	try
	{
		switch (GetType())
		{
		case TDATA_INT:
			SetInt(lexical_cast<int64_t> (strData));
			break;

		case TDATA_FLOAT:
			SetFloat(lexical_cast<float> (strData));
			break;

		case TDATA_STRING:
			SetString(strData);
			break;

		case TDATA_OBJECT:
		{
			NFGUID xID;
			xID.FromString(strData);
			SetObject(xID);
		}
		break;

		default:
			break;
		}

		return true;
	}
	catch (...)
	{
		return false;
	}

	return false;
}