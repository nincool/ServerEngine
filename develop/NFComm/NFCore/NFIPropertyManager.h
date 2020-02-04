///--------------------------------------------------------------------
/// �ļ���:		NFIPropertyManager.h
/// ��  ��:		���Թ���ӿ�
/// ˵  ��:		
/// ��������:	2019.8.10
/// �޸���:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_PROPERTYMANAGER_H
#define NFI_PROPERTYMANAGER_H

#include "NFMap.hpp"
#include "NFIProperty.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

enum EditType
{
	EDIT_OK,		// ���Ա��޸�
	EDIT_INIT,		// ֻ�ܱ���ʼ��
	EDIT_NO,		// �������޸�
};

class _NFExport NFIPropertyManager : public NFMapEx<std::string, NFIProperty>
{
public:
    virtual ~NFIPropertyManager() {}

    virtual bool RegisterCallback(const std::string& strProperty, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, NF_SHARE_PTR<NFIProperty> pProperty) = 0;

    virtual NF_SHARE_PTR<NFIProperty> AddProperty(const NFGUID& self, const std::string& strPropertyName, const NFDATA_TYPE varType) = 0;
	// ����ڲ�����
	// bEdit �Ƿ������޸�
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, NFINT64& data, EditType eEdit = EDIT_OK) = 0;
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, double& data, EditType eEdit = EDIT_OK) = 0;
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, std::string& data, EditType eEdit = EDIT_OK) = 0;
	virtual NF_SHARE_PTR<NFIProperty> AddRawProperty(const std::string& strPropertyName, NFGUID& data, EditType eEdit = EDIT_OK) = 0;
	// �����ڲ������Ƿ���Ա��޸�
	virtual bool SetRawPropertyEdit(EditType eEdit) = 0;
	// �ָ��ڲ������Ƿ���Ա��޸�
	virtual bool RestoreRawPropertyEdit() = 0;

    virtual const NFGUID& Self() = 0;

	virtual std::string ToString() = 0;
    //////////////////////////////////////////////////////////////////////////
    virtual bool SetProperty(const std::string& strPropertyName, const NFData& TData) = 0;

    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 nValue) = 0;
    virtual bool SetPropertyFloat(const std::string& strPropertyName, const double dwValue) = 0;
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& strValue) = 0;
    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& obj) = 0;

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName) = 0;
    virtual double GetPropertyFloat(const std::string& strPropertyName) = 0;
    virtual const std::string& GetPropertyString(const std::string& strPropertyName) = 0;
    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName) = 0;

};

#endif
