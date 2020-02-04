///--------------------------------------------------------------------
/// 文件名:		NFProperty.h
/// 内  容:		属性
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_PROPERTY_H
#define NF_PROPERTY_H

#include "NFMap.hpp"
#include "NFList.hpp"
#include "NFDataList.hpp"
#include "NFIProperty.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

class _NFExport NFProperty : public NFIProperty
{
private:
    NFProperty();
	NFProperty(const NFProperty&) = delete;
	NFProperty& operator=(const NFProperty&) = delete;
public:
    NFProperty(const NFGUID& self, const std::string& strPropertyName, const NFDATA_TYPE varType);

    virtual ~NFProperty();

    virtual void SetValue(const NFData& xData);
	virtual void SetValue(NFData&& xData);
    virtual void SetValue(const NFIProperty* pProperty);

    virtual bool SetInt(const NFINT64 value);
    virtual bool SetFloat(const double value);
    virtual bool SetString(const std::string& value);
	virtual bool SetString(std::string&& value);
    virtual bool SetObject(const NFGUID& value);

    virtual const NFDATA_TYPE GetType() const;
	virtual const NFDATA_TYPE GetTypeEx() const;
    virtual const std::string& GetName() const;
    virtual const bool GetSave() const;
    virtual const bool GetPublic() const;
    virtual const bool GetPrivate() const;
	virtual const int GetTCallBackCount() const;
	virtual const bool IsMember() const;

    virtual void SetSave(bool bSave);
    virtual void SetPublic(bool bPublic);
    virtual void SetPrivate(bool bPrivate);

    virtual NFINT64 GetInt() const;
    virtual double GetFloat() const;
    virtual const std::string& GetString() const;
    virtual const NFGUID& GetObject() const;

    virtual bool IsNull() const;
	virtual bool CanEdit() const;
	virtual void Clear();

	virtual std::string ToString();
    virtual bool FromString(const std::string& strData);

    virtual void RegisterCallback(const PROPERTY_EVENT_FUNCTOR_PTR& cb);

protected:
    int OnEventHandler(const NFData& oldVar, const NFData& newVar);

	template<class T>
	bool InnerSetString(T&& value);

protected:
    typedef std::vector<PROPERTY_EVENT_FUNCTOR_PTR> TPROPERTYCALLBACKEX;
    TPROPERTYCALLBACKEX mtPropertyCallback;

    NFGUID mSelf;
    std::string msPropertyName;
    NFDATA_TYPE m_eType;

    NF_SHARE_PTR<NFData> mxData;

    bool mbPublic = false;
    bool mbPrivate = false;
    bool mbSave = false;
};

#endif
