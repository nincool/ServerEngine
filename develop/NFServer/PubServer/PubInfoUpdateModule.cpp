///--------------------------------------------------------------------
/// 文件名:		PubInfoUpdateModule.cpp
/// 内  容:		可视属性和表同步给game
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PubInfoUpdateModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"

CPubInfoUpdateModule::CPubInfoUpdateModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

CPubInfoUpdateModule::~CPubInfoUpdateModule()
{

}

bool CPubInfoUpdateModule::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CPubInfoUpdateModule::AfterInit()
{
	m_pKernelModule->AddEventCallBack(LC::Guild::ThisName(), EVENT_OnComplete, METHOD_ARGS(CPubInfoUpdateModule::OnComplete));
	m_pKernelModule->AddEventCallBack(LC::Pub::ThisName(), EVENT_OnComplete, METHOD_ARGS(CPubInfoUpdateModule::OnComplete));

	return true;
}

int CPubInfoUpdateModule::OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (!pObject)
	{
		QLOG_WARING << __FUNCTION__ << " obj not exist:" << self;
		return 0;
	}

	// 属性添加同步事件
	NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = pObject->GetPropertyManager();
	if (pPropertyManager)
	{
		NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->First();
		while (pProperty)
		{
			if (pProperty->GetPrivate())
			{
				m_pKernelModule->AddPropertyCallBack(self, pProperty->GetName(), this, &CPubInfoUpdateModule::OnVisPropUpdate);
			}

			pProperty = pPropertyManager->Next();
		}
	}

	// 表添加同步事件
	NF_SHARE_PTR<NFIRecordManager> pRecordManager = pObject->GetRecordManager();
	if (pRecordManager)
	{
		NF_SHARE_PTR<NFIRecord> pRecord = pRecordManager->First();
		while (pRecord)
		{
			if (pRecord->GetPrivate())
			{
				m_pKernelModule->AddRecordCallBack(self, pRecord->GetName(), this, &CPubInfoUpdateModule::OnVisRecordUpdate);
			}

			pRecord = pRecordManager->Next();
		}
	}

	return 0;
}

// 可视属性更新
int CPubInfoUpdateModule::OnVisPropUpdate(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData)
{
	OuterMsg::ObjectPropertyList msg;
	OuterMsg::PropertyList* pDataList = msg.mutable_property_list();
	if (pDataList == nullptr)
	{
		return 0;
	}

	PBConvert::ToPB(self, *(msg.mutable_ident()));

	switch (newData.GetType())
	{
	case TDATA_INT:
	{
		OuterMsg::PropertyInt* pPropertyData = pDataList->add_property_int();
		if (pPropertyData)
		{
			pPropertyData->set_name(strName);
			pPropertyData->set_data(newData.GetInt());
		}
	}
	break;

	case TDATA_FLOAT:
	{
		OuterMsg::PropertyFloat* pPropertyData = pDataList->add_property_float();
		if (pPropertyData)
		{
			pPropertyData->set_name(strName);
			pPropertyData->set_data(newData.GetFloat());
		}
	}
	break;

	case TDATA_STRING:
	{
		OuterMsg::PropertyString* pPropertyData = pDataList->add_property_string();
		if (pPropertyData)
		{
			pPropertyData->set_name(strName);
			pPropertyData->set_data(newData.GetString());
		}
	}
	break;

	case TDATA_OBJECT:
	{
		OuterMsg::PropertyObject* pPropertyData = pDataList->add_property_object();
		if (pPropertyData)
		{
			pPropertyData->set_name(strName);
			PBConvert::ToPB(newData.GetObject(), *(pPropertyData->mutable_data()));
		}
	}
	break;

	default:
		break;
	}
	
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_OBJECT_PROPERTY, msg);

	return 0;
}

// 可视表更新
int CPubInfoUpdateModule::OnVisRecordUpdate(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldData, const NFData& newData)
{
	NF_SHARE_PTR<NFIObject> pSelfObj = m_pKernelModule->GetObject(self);
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not exist:" << self;
		return 0;
	}

	NF_SHARE_PTR<NFIRecord> xRecord = pSelfObj->GetRecord(xEventData.strRecordName);
	if (nullptr == xRecord)
	{
		QLOG_WARING << __FUNCTION__ << " record not exist:" << xEventData.strRecordName;
		return 0;
	}

	OuterMsg::ObjectVisRecord msgVisRecord;
	OuterMsg::RecordList* pRecList = msgVisRecord.mutable_record_list();
	if (pRecList == nullptr)
	{
		return 0;
	}

	PBConvert::ToPB(self, *(msgVisRecord.mutable_ident()));

	OuterMsg::Record* pPrivateData = pRecList->add_record();
	if (pPrivateData == nullptr)
	{
		return 0;
	}
	pPrivateData->set_name(xEventData.strRecordName);

	OuterMsg::RecordRow* pRowData = pPrivateData->add_row_data();
	if (pRowData == nullptr)
	{
		return 0;
	}
	pRowData->set_row(xEventData.nRow);

	switch (xEventData.nOpType)
	{
	case RECORD_EVENT_DATA::Add:
	{
		msgVisRecord.set_opera_type(OuterMsg::RECORD_OT_ADD);

		for (int i = 0; i < xRecord->GetCols(); i++)
		{
			NFDATA_TYPE eType = xRecord->GetColType(i);
			switch (eType)
			{
			case NFDATA_TYPE::TDATA_INT:
			{
				NFINT64 nValue = xRecord->GetInt(xEventData.nRow, i);
				OuterMsg::RecordInt* pAddData = pRowData->add_record_int();
				if (pAddData)
				{
					pAddData->set_col(i);
					pAddData->set_data(nValue);
				}
			}
			break;

			case NFDATA_TYPE::TDATA_FLOAT:
			{
				double dwValue = xRecord->GetFloat(xEventData.nRow, i);
				OuterMsg::RecordFloat* pAddData = pRowData->add_record_float();
				if (pAddData)
				{
					pAddData->set_col(i);
					pAddData->set_data(dwValue);
				}
			}
			break;

			case NFDATA_TYPE::TDATA_STRING:
			{
				const std::string& strData = xRecord->GetString(xEventData.nRow, i);
				OuterMsg::RecordString* pAddData = pRowData->add_record_string();
				if (pAddData)
				{
					pAddData->set_col(i);
					pAddData->set_data(strData);
				}
			}
			break;

			case NFDATA_TYPE::TDATA_OBJECT:
			{
				NFGUID ident = xRecord->GetObject(xEventData.nRow, i);
				OuterMsg::RecordObject* pAddData = pRowData->add_record_object();
				if (pAddData)
				{
					pAddData->set_col(i);
					PBConvert::ToPB(ident, *(pAddData->mutable_data()));
				}
			}
			break;

			default:
				break;
			}
		}
	}
	break;

	case RECORD_EVENT_DATA::Del:
	{
		msgVisRecord.set_opera_type(OuterMsg::RECORD_OT_REMOVE);
	}
	break;

	case RECORD_EVENT_DATA::Update:
	{
		msgVisRecord.set_opera_type(OuterMsg::RECORD_OT_MODIFY);
		switch (oldData.GetType())
		{
		case NFDATA_TYPE::TDATA_INT:
		{
			OuterMsg::RecordInt* pAddData = pRowData->add_record_int();
			if (pAddData)
			{
				pAddData->set_col(xEventData.nCol);
				pAddData->set_data(newData.GetInt());
			}
		}
		break;

		case NFDATA_TYPE::TDATA_FLOAT:
		{
			OuterMsg::RecordFloat* pAddData = pRowData->add_record_float();
			if (pAddData)
			{
				pAddData->set_col(xEventData.nCol);
				pAddData->set_data(newData.GetFloat());
			}
		}
		break;

		case NFDATA_TYPE::TDATA_STRING:
		{
			OuterMsg::RecordString* pAddData = pRowData->add_record_string();
			if (pAddData)
			{
				pAddData->set_col(xEventData.nCol);
				pAddData->set_data(newData.GetString());
			}
		}
		break;

		case NFDATA_TYPE::TDATA_OBJECT:
		{
			OuterMsg::RecordObject* pAddData = pRowData->add_record_object();
			if (pAddData)
			{
				pAddData->set_col(xEventData.nCol);
				PBConvert::ToPB(newData.GetObject(), *(pAddData->mutable_data()));
			}
		}
		break;

		default:
			break;
		}
	}
	break;

	case RECORD_EVENT_DATA::Cleared:
	{
		msgVisRecord.set_opera_type(OuterMsg::RECORD_OT_CLEAR);
	}
	break;
	default:
		break;
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_OBJECT_TABLE, msgVisRecord);

	return 0;
}