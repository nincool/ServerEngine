///--------------------------------------------------------------------
/// �ļ���:		NFIObject.h
/// ��  ��:		����ӿ�
/// ˵  ��:		
/// ��������:	2019.8.10
/// �޸���:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_OBJECT_H
#define NFI_OBJECT_H

#include "NFDataList.hpp"
#include "NFIRecord.h"
#include "NFIRecordManager.h"
#include "NFIPropertyManager.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFMessageDefine/NFDefine.pb.h"

enum CLASS_OBJECT_EVENT
{
	COE_CREATE_NODATA,			  //��������û������
	COE_CREATE_BEFORE_ATTACHDATA, //�ڳ�ʼ��������֮ǰ
	COE_CREATE_LOADDATA,		  //�������ݣ������������ֵ��
	COE_CREATE_AFTER_ATTACHDATA,   //��������֮�� ���߼�������ֵ��
	COE_CREATE_HASDATA,		       //����������
	COE_CREATE_FINISH,			  //���������󴴽����
	COE_CREATE_CLIENT_FINISH,     //�ͻ��˱�ʶ������󴴽����
	COE_BEFOREDESTROY,			  //���ٶ���ǰ
	COE_DESTROY,                  //����
};

class _NFExport NFIObject :public NFMemoryCounter<NFIObject>
{
private:
	NFIObject() : NFMemoryCounter(GET_CLASS_NAME(NFIObject))
	{
	}

public:
    NFIObject(NFGUID self) : NFMemoryCounter(GET_CLASS_NAME(NFIObject))
    {
    }

    virtual ~NFIObject()
	{
	}

	virtual OuterMsg::ObjectType Type() const = 0;
    virtual const NFGUID& Self() const = 0;
	virtual const NFGUID& Parent() const = 0;
	virtual const std::string& Config() const = 0;
	virtual const std::string& ClassName() const = 0;
	virtual int Index() const = 0;

	virtual const std::string& GetName() const = 0;

    template<typename BaseType>
    bool AddPropertyCallBack(const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(NF_NEW PROPERTY_EVENT_FUNCTOR(functor));
        return AddPropertyCallBack(strPropertyName, functorPtr);
    }

    template<typename BaseType>
    bool AddRecordCallBack(const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(NF_NEW RECORD_EVENT_FUNCTOR(functor));
        return AddRecordCallBack(strRecordName, functorPtr);
    }

	virtual CLASS_OBJECT_EVENT GetState() const = 0;

    virtual bool FindProperty(const std::string& strPropertyName) const = 0;
	virtual NFDATA_TYPE GetPropertyType(const std::string& strPropertyName) const = 0;
	virtual NFDATA_TYPE GetPropertyTypeEx(const std::string& strPropertyName) const = 0;
    virtual bool SetPropertyInt(const std::string& strPropertyName, const NFINT64 nValue) = 0;
    virtual bool SetPropertyFloat(const std::string& strPropertyName, const double dwValue) = 0;
    virtual bool SetPropertyString(const std::string& strPropertyName, const std::string& strValue) = 0;
    virtual bool SetPropertyObject(const std::string& strPropertyName, const NFGUID& obj) = 0;

    virtual NFINT64 GetPropertyInt(const std::string& strPropertyName) const = 0;
    virtual double GetPropertyFloat(const std::string& strPropertyName) const = 0;
    virtual const std::string& GetPropertyString(const std::string& strPropertyName) const = 0;
    virtual const NFGUID& GetPropertyObject(const std::string& strPropertyName) const = 0;

    virtual bool FindRecord(const std::string& strRecordName) const = 0;
	virtual NF_SHARE_PTR<NFIRecord> GetRecord(const std::string& strRecordName) = 0;

protected:
    virtual bool AddRecordCallBack(const std::string& strRecordName, const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool AddPropertyCallBack(const std::string& strPropertyName, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;

protected:
    NFIPluginManager* m_pPluginManager;
};

#endif
