///--------------------------------------------------------------------
/// 文件名:		NFPropertyManager.h
/// 内  容:		属性管理
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_PROPERTYMANAGER_H
#define NF_PROPERTYMANAGER_H

#include "NFMap.hpp"
#include "NFDataList.hpp"
#include "NFIPropertyManager.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

class _NFExport NFPropertyManager : public NFIPropertyManager
{
public:
    NFPropertyManager(const NFGUID& self)
    {
        mSelf = self;
    };

    virtual ~NFPropertyManager();

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb);

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty);
	virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, const std::string& strPropertyName, const NFDATA_TYPE varType);
	// 添加内部属性
    virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, NFINT64& data, EditType eEdit = EDIT_OK);
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, double& data, EditType eEdit = EDIT_OK);
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, std::string& data, EditType eEdit = EDIT_OK);
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, NFGUID& data, EditType eEdit = EDIT_OK);
	// 设置内部属性是否可以被修改
	virtual bool SetRawPropertyEdit(EditType eEdit);
	// 恢复内部属性是否可以被修改
	virtual bool RestoreRawPropertyEdit();

	virtual const NFGUID& Self();

	virtual std::string ToString();

    virtual bool SetProperty(const std::string& strPropertyName, const NFData& TData);

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 nValue);
    virtual bool SetPropertyFloat(const std::string& strPropertyName, const double dwValue);
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& strValue);
    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& obj);

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName);
    virtual double GetPropertyFloat(const std::string& strPropertyName);
    virtual const std::string& GetPropertyString(const std::string& strPropertyName);
    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName);

private:
    NFGUID mSelf;
    std::map<std::string, int> mxPropertyIndexMap;
};


#endif
