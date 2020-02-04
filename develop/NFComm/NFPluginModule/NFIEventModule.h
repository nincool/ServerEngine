///--------------------------------------------------------------------
/// �ļ���:		NFIEventModule.h
/// ��  ��:		�¼��ӿ�
/// ˵  ��:		
/// ��������:	2019��8��3��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_EVENT_MODULE_H
#define NFI_EVENT_MODULE_H

#include <iostream>
#include "NFIModule.h"

enum NFEventDefine
{
	NFED_UNKNOW = 0,
};

class NFIEventModule
    : public NFIModule
{
public:
protected:

	typedef std::function<int(const NFGUID&, const NFEventDefine, const NFDataList&)> OBJECT_EVENT_FUNCTOR;
	typedef std::function<int(const NFEventDefine, const NFDataList&)> MODULE_EVENT_FUNCTOR;

	typedef NF_SHARE_PTR<OBJECT_EVENT_FUNCTOR> OBJECT_EVENT_FUNCTOR_PTR;//EVENT
	typedef NF_SHARE_PTR<MODULE_EVENT_FUNCTOR> MODULE_EVENT_FUNCTOR_PTR;//EVENT

public:
	// only be used in module
    virtual bool DoEvent(const NFEventDefine nEventID, const NFDataList& valueList) = 0;

    virtual bool ExistEventCallBack(const NFEventDefine nEventID) = 0;
    
    virtual bool RemoveEventCallBack(const NFEventDefine nEventID) = 0;

	template<typename BaseType>
	bool AddEventCallBack(const NFEventDefine nEventID, BaseType* pBase, int (BaseType::*handler)(const NFEventDefine, const NFDataList&))
	{
		MODULE_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2);
		MODULE_EVENT_FUNCTOR_PTR functorPtr(new MODULE_EVENT_FUNCTOR(functor));
		return AddEventCallBack(nEventID, functorPtr);
	}
    ///////////////////////////////////////////////////////////////////////////////////////////////
	// can be used for object
    virtual bool DoEvent(const NFGUID self, const NFEventDefine nEventID, const NFDataList& valueList) = 0;

    virtual bool ExistEventCallBack(const NFGUID self,const NFEventDefine nEventID) = 0;
    
    virtual bool RemoveEventCallBack(const NFGUID self,const NFEventDefine nEventID) = 0;
	virtual bool RemoveEventCallBack(const NFGUID self) = 0;
	
	template<typename BaseType>
	bool AddEventCallBack(const NFGUID& self, const NFEventDefine nEventID, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const NFEventDefine, const NFDataList&))
	{
		OBJECT_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
		OBJECT_EVENT_FUNCTOR_PTR functorPtr(new OBJECT_EVENT_FUNCTOR(functor));
		return AddEventCallBack(self, nEventID, functorPtr);
	}

protected:

	virtual bool AddEventCallBack(const NFEventDefine nEventID, const MODULE_EVENT_FUNCTOR_PTR cb) = 0;
	virtual bool AddEventCallBack(const NFGUID self, const NFEventDefine nEventID, const OBJECT_EVENT_FUNCTOR_PTR cb) = 0;

};

#endif