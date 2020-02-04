///--------------------------------------------------------------------
/// 文件名:		NFIProperty.h
/// 内  容:		属性接口
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_PROPERTY_H
#define NFI_PROPERTY_H

#include "NFDataList.hpp"
#include "NFList.hpp"
#include "NFComm/NFPluginModule/NFPlatform.h"

typedef std::function<int(const NFGUID&, const std::string&, const NFData&, const NFData&)> PROPERTY_EVENT_FUNCTOR;
typedef NF_SHARE_PTR<PROPERTY_EVENT_FUNCTOR> PROPERTY_EVENT_FUNCTOR_PTR;

class _NFExport NFIProperty :public NFMemoryCounter<NFIProperty>
{
public:
	NFIProperty() : NFMemoryCounter(GET_CLASS_NAME(NFIProperty))
	{
	}

	virtual ~NFIProperty() {}

	virtual void SetValue(const NFData& xData) = 0;
	virtual void SetValue(NFData&& xData) = 0;
	virtual void SetValue(const NFIProperty* pProperty) = 0;

	virtual bool SetInt(const NFINT64 value) = 0;
	virtual bool SetFloat(const double value) = 0;
	virtual bool SetString(const std::string& value) = 0;
	virtual bool SetString(std::string&& value) = 0;
	virtual bool SetObject(const NFGUID& value) = 0;

	virtual const NFDATA_TYPE GetType() const = 0;
	virtual const NFDATA_TYPE GetTypeEx() const = 0;
	virtual const std::string& GetName() const = 0;
	virtual const bool GetSave() const = 0;
	virtual const bool GetPublic() const = 0;
	virtual const bool GetPrivate() const = 0;
	virtual const int GetTCallBackCount() const = 0;

	virtual void SetSave(bool bSave) = 0;
	virtual void SetPublic(bool bPublic) = 0;
	virtual void SetPrivate(bool bPrivate) = 0;

	virtual NFINT64 GetInt() const = 0;
	virtual double GetFloat() const = 0;
	virtual const std::string& GetString() const = 0;
	virtual const NFGUID& GetObject() const = 0;

	virtual bool IsNull() const = 0;
	virtual bool CanEdit() const = 0;
	virtual void Clear() = 0;

	virtual std::string ToString() = 0;
	virtual bool FromString(const std::string& strData) = 0;

	virtual void RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
};

#endif
