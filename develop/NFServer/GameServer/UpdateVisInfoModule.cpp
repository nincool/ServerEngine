///--------------------------------------------------------------------
/// 文件名:		UpdateVisInfoModule.cpp
/// 内  容:		属性和表同步客户端
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "UpdateVisInfoModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFCore/NFPlayer.h"

CUpdateVisInfoModule::CUpdateVisInfoModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

CUpdateVisInfoModule::~CUpdateVisInfoModule()
{

}

bool CUpdateVisInfoModule::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pGameServer = pPluginManager->FindModule<CGameServer>();

	return true;
}

bool CUpdateVisInfoModule::AfterInit()
{
	m_pKernelModule->AddEventCallBack(LC::Player::ThisName(), EVENT_OnComplete, METHOD_ARGS(CUpdateVisInfoModule::OnComplete));
	m_pKernelModule->AddCommondCallBack(LC::Player::ThisName(), OuterMsg::SS_REQ_UPDATE_PLAYER_DATA, METHOD_ARGS(CUpdateVisInfoModule::UpdateAllPlayerData));

	return true;
}

int CUpdateVisInfoModule::OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (nullptr == pObject)
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << self;
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
				m_pKernelModule->AddPropertyCallBack(self, pProperty->GetName(), this, &CUpdateVisInfoModule::OnVisPropUpdate);
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
				m_pKernelModule->AddRecordCallBack(self, pRecord->GetName(), this, &CUpdateVisInfoModule::OnVisRecordUpdate);
			}

			pRecord = pRecordManager->Next();
		}
	}

	UpdateAllPlayerData(self, NULL_OBJECT, NFDataList());

	return 0;
}

void CUpdateVisInfoModule::UpdateAllPlayerData(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	// 同步所有可视属性
	VisAllPropUpdate(self);

	// 同步所有可视表
	VisAllRecordUpdate(self);
}

// 可视属性更新
int CUpdateVisInfoModule::OnVisPropUpdate(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData)
{
	NF_SHARE_PTR<NFPlayer> pSelfObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << self;
		return 0;
	}

	if (pSelfObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	OuterMsg::ObjectPropertyList msg;
	OuterMsg::PropertyList* pDataList = msg.mutable_property_list();
	if (pDataList == nullptr)
	{
		return 0;
	}

	PBConvert::ToPB(self, *(msg.mutable_ident()));

	switch (newData.GetTypeEx())
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

	case TDATA_BYTES:
	{
		auto* pPropertyData = pDataList->add_property_bytes();
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

	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_OBJECT_PROPERTY, msg, self);

	return 0;
}

// 可视表更新
int CUpdateVisInfoModule::OnVisRecordUpdate(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldData, const NFData& newData)
{
	NF_SHARE_PTR<NFPlayer> pSelfObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << self;
		return 0;
	}

	if (pSelfObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	NF_SHARE_PTR<NFIRecord> xRecord = pSelfObj->GetRecord(xEventData.strRecordName);
	if (nullptr == xRecord)
	{
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
		// 列数大于0则为更新行，否则为新增行
		msgVisRecord.set_opera_type(xEventData.nCol > 0 ? OuterMsg::RECORD_OT_MODIFY : OuterMsg::RECORD_OT_ADD);

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

	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_OBJECT_TABLE, msgVisRecord, self);
	return 0;
}

// 同步所有可视属性
int CUpdateVisInfoModule::VisAllPropUpdate(const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer> pSelfObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << self;
		return 0;
	}

	if (pSelfObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	OuterMsg::ObjectPropertyList msg;
	OuterMsg::PropertyList* pDataList = msg.mutable_property_list();
	if (pDataList == nullptr)
	{
		return 0;
	}
	PBConvert::ToPB(self, *(msg.mutable_ident()));

	// 设置类型
	msg.set_type(pSelfObj->Type());

	if (!PackAllProperty(pSelfObj.get(), pDataList))
	{
		QLOG_WARING << __FUNCTION__ << "PackAllProperty error obj:" << pSelfObj->Self();
		return 0;
	}

	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_PROPERTY_TABLE, msg, self);
	return 0;
}

// 同步所有可视表
int CUpdateVisInfoModule::VisAllRecordUpdate(const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer> pSelfObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << self;
		return 0;
	}

	if (pSelfObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	OuterMsg::ObjectRecordList msg;
	OuterMsg::RecordList* pDataList = msg.mutable_record_list();
	if (pDataList == nullptr)
	{
		return 0;
	}

	PBConvert::ToPB(self, *(msg.mutable_ident()));
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (!PackAllRecord(pObj.get(), pDataList))
	{
		QLOG_WARING << __FUNCTION__ << " PackAllRecord error " << pObj->Self();
		return 0;
	}

	if (pDataList->record_size() <= 0)
	{
		return 0;
	}

	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_RECORD_TABLE, msg, self);
	return 0;
}

bool CUpdateVisInfoModule::PackAllProperty(NFObject* pObj, OuterMsg::PropertyList* pPrivateList)
{
	if (pObj == nullptr || pPrivateList == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pObj/pPrivateList == NULL";
		assert(false);
		return false;
	}

	NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = pObj->GetPropertyManager();
	NF_SHARE_PTR<NFIProperty> pPropertyInfo = pPropertyManager->First();
	while (pPropertyInfo)
	{
		if (pPropertyInfo->GetPrivate())
		{
			switch (pPropertyInfo->GetTypeEx())
			{
			case TDATA_INT:
			{
				OuterMsg::PropertyInt* pPropertyData = pPrivateList->add_property_int();
				if (pPropertyData)
				{
					pPropertyData->set_name(pPropertyInfo->GetName());
					pPropertyData->set_data(pPropertyInfo->GetInt());
				}
			}
			break;

			case TDATA_FLOAT:
			{
				OuterMsg::PropertyFloat* pPropertyData = pPrivateList->add_property_float();
				if (pPropertyData)
				{
					pPropertyData->set_name(pPropertyInfo->GetName());
					pPropertyData->set_data(pPropertyInfo->GetFloat());
				}
			}
			break;

			case TDATA_STRING:
			{
				OuterMsg::PropertyString* pPropertyData = pPrivateList->add_property_string();
				if (pPropertyData)
				{
					pPropertyData->set_name(pPropertyInfo->GetName());
					pPropertyData->set_data(pPropertyInfo->GetString());
				}
			}
			break;

			case TDATA_BYTES:
			{
				auto* pPropertyData = pPrivateList->add_property_bytes();
				if (pPropertyData)
				{
					pPropertyData->set_name(pPropertyInfo->GetName());
					pPropertyData->set_data(pPropertyInfo->GetString());
				}
			}
			break;

			case TDATA_OBJECT:
			{
				OuterMsg::PropertyObject* pPropertyData = pPrivateList->add_property_object();
				if (pPropertyData)
				{
					pPropertyData->set_name(pPropertyInfo->GetName());
					PBConvert::ToPB(pPropertyInfo->GetObject(), *(pPropertyData->mutable_data()));
				}
			}
			break;

			default:
				break;
			}
		}

		pPropertyInfo = pPropertyManager->Next();
	}

	return true;
}

bool CUpdateVisInfoModule::PackAllRecord(NFObject* pObj, OuterMsg::RecordList* pRecList)
{
	if (pObj == nullptr || pRecList == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pObj/pRecList == NULL";
		assert(false);
		return false;
	}

	NF_SHARE_PTR<NFIRecordManager> pRecordManager = pObj->GetRecordManager();
	NF_SHARE_PTR<NFIRecord> pRecord = pRecordManager->First();
	while (pRecord)
	{
		if (pRecord->GetPrivate())
		{
			OuterMsg::Record* pPrivateData = pRecList->add_record();
			if (pPrivateData)
			{
				PackRecord(pRecord, pPrivateData);
			}
		}

		pRecord = pRecordManager->Next();
	}

	return true;
}

bool CUpdateVisInfoModule::PackRecord(NF_SHARE_PTR<NFIRecord> pRecord, OuterMsg::Record* pPrivateData)
{
	if (pRecord == nullptr || pPrivateData == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pRecord/pPrivateData == NULL";
		return false;
	}

	pPrivateData->set_name(pRecord->GetName());

	OuterMsg::RecordRow* pRowData = pPrivateData->add_row_data();
	if (pRowData == nullptr)
	{
		return false;
	}

	// 第一次下发表结构
	pRowData->set_row(-1);
	for (int i = 0; i < pRecord->GetCols(); i++)
	{
		NFDATA_TYPE eType = pRecord->GetColType(i);
		switch (eType)
		{
		case NFDATA_TYPE::TDATA_INT:
		{
			OuterMsg::RecordInt* pAddData = pRowData->add_record_int();
			pAddData->set_col(i);
			pAddData->set_data(NULL_INT);
		}
		break;
		case NFDATA_TYPE::TDATA_FLOAT:
		{
			OuterMsg::RecordFloat* pAddData = pRowData->add_record_float();
			pAddData->set_col(i);
			pAddData->set_data(NULL_FLOAT);
		}
		break;
		case NFDATA_TYPE::TDATA_STRING:
		{
			OuterMsg::RecordString* pAddData = pRowData->add_record_string();
			if (dynamic_cast<NFRecord*>(pRecord.get())->IsBytes(i))
			{
				pAddData->set_col(i);
				pAddData->set_data("b");
			}
			else
			{
				pAddData->set_col(i);
				pAddData->set_data(NULL_STR);
			}
		}
		break;

		case NFDATA_TYPE::TDATA_OBJECT:
		{
			OuterMsg::RecordObject* pAddData = pRowData->add_record_object();
			pAddData->set_col(i);
			PBConvert::ToPB(NULL_OBJECT, *(pAddData->mutable_data()));
		}
		break;

		default:
			break;
		}
	}

	// 填充表内容
	for (int i = 0; i < pRecord->GetRows(); i++)
	{
		pRowData = pPrivateData->add_row_data();
		if (pRowData == nullptr)
		{
			continue;
		}

		pRowData->set_row(i);

		for (int j = 0; j < pRecord->GetCols(); j++)
		{
			NFDATA_TYPE eType = pRecord->GetColType(j);
			switch (eType)
			{
			case NFDATA_TYPE::TDATA_INT:
			{
				NFINT64 nValue = pRecord->GetInt(i, j);
				OuterMsg::RecordInt* pAddData = pRowData->add_record_int();
				pAddData->set_col(j);
				pAddData->set_data(nValue);
			}
			break;
			case NFDATA_TYPE::TDATA_FLOAT:
			{
				double dwValue = pRecord->GetFloat(i, j);
				OuterMsg::RecordFloat* pAddData = pRowData->add_record_float();
				pAddData->set_col(j);
				pAddData->set_data(dwValue);
			}
			break;
			case NFDATA_TYPE::TDATA_STRING:
			{
				const std::string& strData = pRecord->GetString(i, j);
				OuterMsg::RecordString* pAddData = pRowData->add_record_string();
				pAddData->set_col(j);
				pAddData->set_data(strData);
			}
			break;

			case NFDATA_TYPE::TDATA_OBJECT:
			{
				NFGUID ident = pRecord->GetObject(i, j);
				OuterMsg::RecordObject* pAddData = pRowData->add_record_object();
				pAddData->set_col(j);
				PBConvert::ToPB(ident, *(pAddData->mutable_data()));
			}
			break;

			default:
				break;
			}
		}
	}

	return true;
}