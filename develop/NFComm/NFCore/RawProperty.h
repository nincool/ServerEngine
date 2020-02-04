///--------------------------------------------------------------------
/// 文件名:		RawProperty.h
/// 内  容:		内部属性
/// 说  明:		
/// 创建日期:	2019年11月11日
/// 修改人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __RAW_PROPERTY_H__
#define __RAW_PROPERTY_H__

#include "NFProperty.h"
#include "NFComm/NFUtils/QLOG.h"
#include "NFComm/NFUtils/NFTool.hpp"

class CRawProperty : public NFProperty
{
private:
	CRawProperty();
	CRawProperty(const CRawProperty&) = delete;
	CRawProperty& operator=(const CRawProperty&) = delete;
public:
	CRawProperty(const NFGUID& self, const std::string& strPropertyName, NFDATA_TYPE eType, EditType eEdit)
		: NFProperty(self, strPropertyName, eType)
		, m_eOriEdit(eEdit)
		, m_eEdit(eEdit)
	{
	}

	virtual const bool IsMember() const override
	{
		return true;
	}

	// 设置是否可编辑
	bool SetEdit(EditType eEdit)
	{
		m_eEdit = eEdit;
		return true;
	}

	// 还原是否可编辑
	bool RestoreEdit()
	{
		m_eEdit = m_eOriEdit;
		return true;
	}

	// 是否可编辑
	virtual bool CanEdit() const override
	{
		switch (m_eEdit)
		{
		case EDIT_OK:
			return true;
			break;
		case EDIT_INIT:
			return IsNull();
			break;
		case EDIT_NO:
			return false;
			break;
		}

		return false;
	}

protected:
	EditType m_eOriEdit;
	EditType m_eEdit;
};

class CRawPropertyInt64 final : public CRawProperty
{
private:
	CRawPropertyInt64();
	CRawPropertyInt64(const CRawPropertyInt64&) = delete;
	CRawPropertyInt64& operator=(const CRawPropertyInt64&) = delete;
public:
	CRawPropertyInt64(const NFGUID& self, const std::string& strPropertyName, NFINT64& data, EditType eEdit)
		: CRawProperty(self, strPropertyName, NFDATA_TYPE::TDATA_INT, eEdit)
		, m_data(data)
	{
	}

	virtual bool IsNull() const
	{
		return m_data == 0;
	}

	virtual bool SetInt(const NFINT64 value) override
	{
		if (!CanEdit())
		{
			QLOG_WARING << __FUNCTION__ << " prop:" << msPropertyName << " can't be change";
			return false;
		}

		if (m_data == value)
		{
			return false;
		}

		if (!mtPropertyCallback.empty())
		{
			NFData oldValue;
			oldValue.SetInt(m_data);

			NFData newValue;
			newValue.SetInt(value);

			m_data = value;
			OnEventHandler(oldValue, newValue);

			return true;
		}

		m_data = value;
		return true;
	}

	virtual NFINT64 GetInt() const override
	{
		return m_data;
	}

private:
	NFINT64& m_data;
};

class CRawPropertyDouble final : public CRawProperty
{
private:
	CRawPropertyDouble();
	CRawPropertyDouble(const CRawPropertyDouble&) = delete;
	CRawPropertyDouble& operator=(const CRawPropertyDouble&) = delete;
public:
	CRawPropertyDouble(const NFGUID& self, const std::string& strPropertyName, double& data, EditType eEdit)
		: CRawProperty(self, strPropertyName, NFDATA_TYPE::TDATA_FLOAT, eEdit)
		, m_data(data)
	{
	}

	virtual bool IsNull() const
	{
		return DoubleEqual(m_data, 0.0f);
	}

	virtual bool SetFloat(const double value) override
	{
		if (!CanEdit())
		{
			QLOG_WARING << __FUNCTION__ << " prop:" << msPropertyName << " can't be change";
			return false;
		}

		if (!mtPropertyCallback.empty())
		{
			NFData oldValue;
			oldValue.SetFloat(m_data);

			NFData newValue;
			newValue.SetFloat(value);

			m_data = value;
			OnEventHandler(oldValue, newValue);

			return true;
		}

		m_data = value;
		return true;
	}

	virtual double GetFloat() const override
	{
		return m_data;
	}

private:
	double& m_data;
};

class CRawPropertyString final : public CRawProperty
{
private:
	CRawPropertyString();
	CRawPropertyString(const CRawPropertyString&) = delete;
	CRawPropertyString& operator=(const CRawPropertyString&) = delete;
public:
	CRawPropertyString(const NFGUID& self, const std::string& strPropertyName, std::string& data, EditType eEdit)
		: CRawProperty(self, strPropertyName, NFDATA_TYPE::TDATA_STRING, eEdit)
		, m_data(data)
	{
	}

	virtual bool IsNull() const
	{
		return m_data.empty();
	}

	virtual bool SetString(const std::string& value) override
	{
		if (!CanEdit())
		{
			QLOG_WARING << __FUNCTION__ << " prop:" << msPropertyName << " can't be change";
			return false;
		}

		if (!mtPropertyCallback.empty())
		{
			NFData oldValue;
			oldValue.SetString(m_data);

			NFData newValue;
			newValue.SetString(value);

			m_data = value;
			OnEventHandler(oldValue, newValue);

			return true;
		}

		m_data = value;
		return true;
	}

	virtual const std::string& GetString() const override
	{
		return m_data;
	}

private:
	std::string& m_data;
};

class CRawPropertyObject final : public CRawProperty
{
private:
	CRawPropertyObject();
	CRawPropertyObject(const CRawPropertyObject&) = delete;
	CRawPropertyObject& operator=(const CRawPropertyObject&) = delete;
public:
	CRawPropertyObject(const NFGUID& self, const std::string& strPropertyName, NFGUID& data, EditType eEdit)
		: CRawProperty(self, strPropertyName, NFDATA_TYPE::TDATA_OBJECT, eEdit)
		, m_data(data)
	{
	}

	virtual bool IsNull() const
	{
		return m_data.IsNull();
	}

	virtual bool SetObject(const NFGUID& value) override
	{
		if (!CanEdit())
		{
			QLOG_WARING << __FUNCTION__ << " prop:" << msPropertyName << " can't be change";
			return false;
		}

		if (!mtPropertyCallback.empty())
		{
			NFData oldValue;
			oldValue.SetObject(m_data);

			NFData newValue;
			newValue.SetObject(value);

			m_data = value;
			OnEventHandler(oldValue, newValue);

			return true;
		}

		m_data = value;
		return true;
	}

	virtual const NFGUID& GetObject() const override
	{
		return m_data;
	}

private:
	NFGUID& m_data;
};

#define RAW_INT(name) \
public: \
	void Set##name(int64_t value){ m_dn##name = value ; }\
	int64_t Get##name() const { return m_dn##name; } \
protected: \
	int64_t m_dn##name = NULL_INT; 
#define RAW_DOUBLE(name) \
public: \
	void Set##name(double value){ m_d##name = value ; }\
	double Get##name() const { return m_d##name; } \
protected: \
	double m_d##name = NULL_FLOAT; 
#define RAW_STR(name) \
public: \
	void Set##name(const std::string& value){ m_str##name = value ; }\
	const std::string& Get##name() const { return m_str##name; } \
protected: \
	std::string m_str##name = NULL_STR; 
#define RAW_OBJECT(name) \
public: \
	void Set##name(const NFGUID& value){ m_obj##name = value ; }\
	const NFGUID& Get##name() const { return m_obj##name; } \
protected: \
	NFGUID m_obj##name = NULL_OBJECT; 

#define ADD_RAW_INT(mgr, name, edit, save) \
	auto raw_##name = mgr->AddRawProperty(#name, m_dn##name, edit); \
	if (raw_##name != nullptr) raw_##name->SetSave(save);
#define ADD_RAW_DOUBLE(mgr, name, edit, save) \
	auto raw_##name = mgr->AddRawProperty(#name, m_d##name, edit); \
	if (raw_##name != nullptr) raw_##name->SetSave(save);
#define ADD_RAW_STR(mgr, name, edit, save) \
	auto raw_##name = mgr->AddRawProperty(#name, m_str##name, edit); \
	if (raw_##name != nullptr) raw_##name->SetSave(save);
#define ADD_RAW_OBJECT(mgr, name, edit, save) \
	auto raw_##name = mgr->AddRawProperty(#name, m_obj##name, edit); \
	if (raw_##name != nullptr) raw_##name->SetSave(save);
#define ADD_RAW(mgr, name, raw_name, edit, save) \
	auto raw_##name = mgr->AddRawProperty(#name, raw_name, edit); \
	if (raw_##name != nullptr) raw_##name->SetSave(save);

#endif // __RAW_PROPERTY_H__
