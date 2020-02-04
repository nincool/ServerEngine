///--------------------------------------------------------------------
/// 文件名:		NFUpdateViewportModule.cpp
/// 内  容:		视窗同步数据模块
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "UpdateViewportModule.h"
#include "NFComm/NFCore/NFViewport.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/NFVisible.h"
#include "NFComm/NFCore/NFPlayer.h"

CUpdateViewportModule::CUpdateViewportModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

CUpdateViewportModule::~CUpdateViewportModule()
{

}

bool CUpdateViewportModule::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pGameServer = pPluginManager->FindModule<CGameServer>();

	return true;
}

bool CUpdateViewportModule::AfterInit()
{
	NFViewport::SetHook(NFViewport::HOOK_TYPE_INIT, METHOD_ARGS(CUpdateViewportModule::OnViewInit));
	NFViewport::SetHook(NFViewport::HOOK_TYPE_ADD, METHOD_ARGS(CUpdateViewportModule::OnViewAdd));
	NFViewport::SetHook(NFViewport::HOOK_TYPE_REMOVE, METHOD_ARGS(CUpdateViewportModule::OnViewRemove));
	NFViewport::SetHook(NFViewport::HOOK_TYPE_CHANGE, METHOD_ARGS(CUpdateViewportModule::OnViewChange));
	NFViewport::SetHook(NFViewport::HOOK_TYPE_CLOSE, METHOD_ARGS(CUpdateViewportModule::OnViewClose));

	m_pKernelModule->AddEventCallBack(LC::IObject::ThisName(), EVENT_OnComplete, METHOD_ARGS(CUpdateViewportModule::OnComplete));

	return true;
}

int CUpdateViewportModule::OnViewInit(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return 0;
	}

	if (sender.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sender is null";
		return 0;
	}

	NF_SHARE_PTR<NFPlayer> pSelfObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	if (pSelfObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(m_pKernelModule->GetObject(sender));
	if (pContainer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container not find:" << sender;
		return 0;
	}

	// 视窗id
	int nViewId = static_cast<int>(args.Int(0));
	// 同步容器属性
	OuterMsg::ViewCreate msg;
	msg.set_view_id(nViewId);
	msg.set_capacity(pContainer->GetCapacity());
	OuterMsg::PropertyList* pList = msg.mutable_property_list();
	NFObject* pContainerObj = dynamic_cast<NFObject*>(pContainer.get());
	if (!PackAllProperty(pContainerObj, pList))
	{
		QLOG_WARING << __FUNCTION__ << "PackAllProperty error viewid:" << nViewId 
			<< " container:" << pContainer->GetName();
		return 0;
	}

	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_CREATE, msg, self);

	// 同步容器表
	OuterMsg::ViewRecord msgRec;
	msgRec.set_view_id(nViewId);
	msgRec.set_opera_type(OuterMsg::RECORD_OT_ADD);
	OuterMsg::RecordList* pRecList = msgRec.mutable_record_list();
	if (!PackAllRecord(pContainerObj, pRecList))
	{
		QLOG_WARING << __FUNCTION__ << "PackAllRecord error viewid:" << nViewId 
			<< " container:" << pContainer->GetName();
		return 0;
	}

	if (pRecList->record_size() > 0)
	{
		m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_RECORD, msgRec, self);
	}

	return 0;
}

int CUpdateViewportModule::OnViewAdd(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return 0;
	}

	if (sender.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sender is null";
		return 0;
	}

	NF_SHARE_PTR<NFPlayer> pObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	if (pObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	NF_SHARE_PTR<NFObject> pChild = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(sender));
	if (pChild == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " child not find:" << sender;
		return 0;
	}

	// 视窗id
	int nViewId = static_cast<int>(args.Int(0));
	int nIndex = static_cast<int>(args.Int(1));
	// 同步容器子节点的属性
	OuterMsg::ViewAdd msg;
	msg.set_view_id(nViewId);
	msg.set_index(nIndex);
	OuterMsg::PropertyList* pList = msg.mutable_property_list();
	if (!PackAllProperty(pChild.get(), pList))
	{
		QLOG_ERROR << __FUNCTION__ << " PackAllProperty failed, viewid:" << nViewId << " index:" << nIndex << " player:" << self;
		return 0;
	}

	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_ADD, msg, self);

	// 同步容器子节点的表
	OuterMsg::ViewCellRecord msgRec;
	msgRec.set_view_id(nViewId);
	msgRec.set_index(nIndex);
	msgRec.set_opera_type(OuterMsg::RECORD_OT_ADD);
	OuterMsg::RecordList* pRecList = msgRec.mutable_record_list();
	if (!PackAllRecord(pChild.get(), pRecList))
	{
		QLOG_WARING << __FUNCTION__ << " PackAllRecord failed, viewid:" << nViewId 
			<< " index:" << nIndex 
			<< " player:" << self;
		return 0;
	}

	if (pRecList->record_size() > 0)
	{
		m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_CELL_RECORD, msgRec, self);
	}

	return 0;
}

int CUpdateViewportModule::OnViewRemove(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFPlayer> pObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	if (pObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	OuterMsg::ViewRemove msg;
	msg.set_view_id(static_cast<int>(args.Int(0)));
	msg.set_index(static_cast<int>(args.Int(1)));
	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_REMVOE, msg, self);

	return 0;
}

int CUpdateViewportModule::OnViewChange(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFPlayer> pObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	if (pObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	OuterMsg::ViewChange msg;
	msg.set_view_id(static_cast<int>(args.Int(0)));
	msg.set_old_index(static_cast<int>(args.Int(1)));
	msg.set_new_index(static_cast<int>(args.Int(2)));
	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_CHANGE, msg, self);

	return 0;
}

int CUpdateViewportModule::OnViewClose(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFPlayer> pObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pObj == nullptr)
	{
		return 0;
	}

	if (pObj->GetClientID().IsNull())
	{
		// 不在线
		return 0;
	}

	OuterMsg::ViewClose msg;
	msg.set_view_id(static_cast<int>(args.Int(0)));
	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_CLOSE, msg, self);

	return 0;
}

bool CUpdateViewportModule::PackAllProperty(NFObject* pObj, OuterMsg::PropertyList* pPrivateList)
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
		if (!pPropertyInfo->IsNull() &&
			pPropertyInfo->GetPrivate())
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

bool CUpdateViewportModule::PackAllRecord(NFObject* pObj, OuterMsg::RecordList* pRecList)
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

bool CUpdateViewportModule::PackRecord(NF_SHARE_PTR<NFIRecord> pRecord, OuterMsg::Record* pPrivateData)
{
	if (pRecord == nullptr || pPrivateData == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pRecord/pPrivateData == NULL";
		return false;
	}

	pPrivateData->set_name(pRecord->GetName());

	// 第一次下发表结构
	OuterMsg::RecordRow* pRowData = pPrivateData->add_row_data();
	if (pRowData == nullptr)
	{
		return false;
	}

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

	for (int i = 0; i < pRecord->GetRows(); i++)
	{
		OuterMsg::RecordRow* pRowData = pPrivateData->add_row_data();
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

// 监听所有对象的 私有属性和表的变化。以后需要优化为在适当时机添加和删除监听，避免不必要的回调
int CUpdateViewportModule::OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
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
				m_pKernelModule->AddPropertyCallBack(self, pProperty->GetName(), this, &CUpdateViewportModule::OnViewPropChanged);
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
				m_pKernelModule->AddRecordCallBack(self, pRecord->GetName(), this, &CUpdateViewportModule::OnViewRecordChanged);
			}

			pRecord = pRecordManager->Next();
		}
	}

	return 0;
}

int CUpdateViewportModule::OnViewPropChanged(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData)
{
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	// 自身是否是绑定视窗的容器
	if (pObject->IsVisible())
	{
		NFVisible* pVisible = dynamic_cast<NFVisible*>(pObject.get());
		if (pVisible != nullptr && pVisible->GetViewCount() > 0)
		{
			OuterMsg::ViewProperty msg;
			OuterMsg::PropertyList* pList = msg.mutable_property_list();
			if (!PackOneProperty(pList, strName, newData))
			{
				return false;
			}

			auto list = pVisible->GetViewList();
			for (auto& it : list)
			{
				if (it == nullptr)
				{
					continue;
				}

				NFPlayer* pPlayer = it->GetOwner();
				if (pPlayer == nullptr || pPlayer->GetClientID().IsNull())
				{
					continue;
				}

				msg.set_view_id(it->GetId());
				m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_PROPERTY, msg, pPlayer->Self());
			}

			return 0;
		}
	}

	// 自身是绑定视窗容器的元素
	NFObject* pParent = pObject->GetParent();
	if (pParent != nullptr && pParent->IsVisible())
	{
		NFVisible* pVisible = dynamic_cast<NFVisible*>(pParent);
		if (pVisible != nullptr && pVisible->GetViewCount() > 0)
		{
			OuterMsg::ViewCellProperty msg;
			OuterMsg::PropertyList* pList = msg.mutable_property_list();
			if (!PackOneProperty(pList, strName, newData))
			{
				return false;
			}

			// 设置索引
			msg.set_index(pObject->GetIndexInParent());

			auto list = pVisible->GetViewList();
			for (auto& it : list)
			{
				if (it == nullptr)
				{
					continue;
				}

				NFPlayer* pPlayer = it->GetOwner();
				if (pPlayer == nullptr || pPlayer->GetClientID().IsNull())
				{
					continue;
				}

				msg.set_view_id(it->GetId());
				m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_CELL_PROPERTY, msg, pPlayer->Self());
			}
		}
	}
	
	return 0;
}

bool CUpdateViewportModule::PackOneProperty(OuterMsg::PropertyList* pList, const std::string& strName, const NFData& data)
{
	if (pList == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pList == NULL";
		return false;
	}

	switch (data.GetTypeEx())
	{
	case NFDATA_TYPE::TDATA_INT:
	{
		auto* pData = pList->add_property_int();
		if (pData != nullptr)
		{
			pData->set_name(strName);
			pData->set_data(data.GetInt());
		}
	}
	break;
	case NFDATA_TYPE::TDATA_FLOAT:
	{
		auto* pData = pList->add_property_float();
		if (pData != nullptr)
		{
			pData->set_name(strName);
			pData->set_data(data.GetFloat());
		}
	}
	break;
	case NFDATA_TYPE::TDATA_STRING:
	{
		auto* pData = pList->add_property_string();
		if (pData != nullptr)
		{
			pData->set_name(strName);
			pData->set_data(data.GetString());
		}
	}
	break;
	case TDATA_BYTES:
	{
		auto* pData = pList->add_property_bytes();
		if (pData != nullptr)
		{
			pData->set_name(strName);
			pData->set_data(data.GetString());
		}
	}
	break;
	case NFDATA_TYPE::TDATA_OBJECT:
	{
		auto* pData = pList->add_property_object();
		if (pData != nullptr)
		{
			pData->set_name(strName);
			PBConvert::ToPB(data.GetObject(), *(pData->mutable_data()));
		}
	}
	break;
	default:
		break;
	}

	return true;
}

int CUpdateViewportModule::OnViewRecordChanged(const NFGUID& self, const RECORD_EVENT_DATA& event, const NFData& oldData, const NFData& newData)
{
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	auto pRecordData = pObject->GetRecord(event.strRecordName);
	if (pRecordData == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " record not find:" << event.strRecordName;
		return 0;
	}

	// 自身是否是绑定视窗的容器
	if (pObject->IsVisible())
	{
		NFVisible* pVisible = dynamic_cast<NFVisible*>(pObject.get());
		if (pVisible != nullptr && pVisible->GetViewCount() > 0)
		{
			OuterMsg::ViewRecord msg;
			if (event.nOpType == RECORD_EVENT_DATA::Add 
				&& event.nCol > 0)
			{
				// 列大于0的新增,实际为更改行数据
				msg.set_opera_type(OuterMsg::RECORD_OT_MODIFY);
			}
			else
			{
				msg.set_opera_type(PBConvert::ToPB(event.nOpType));
			}
			
			auto* pList = msg.mutable_record_list();
			if (!PackRecordOneData(pList, event, pRecordData.get(), newData))
			{
				return false;
			}

			auto viewlist = pVisible->GetViewList();
			for (auto& it : viewlist)
			{
				if (it == nullptr)
				{
					continue;
				}

				NFPlayer* pPlayer = it->GetOwner();
				if (pPlayer == nullptr || pPlayer->GetClientID().IsNull())
				{
					continue;
				}

				msg.set_view_id(it->GetId());
				m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_RECORD, msg, pPlayer->Self());
			}

			return 0;
		}
	}

	// 自身是绑定视窗容器的元素
	NFObject* pParent = pObject->GetParent();
	if (pParent != nullptr && pParent->IsVisible())
	{
		NFVisible* pVisible = dynamic_cast<NFVisible*>(pParent);
		if (pVisible != nullptr && pVisible->GetViewCount() > 0)
		{
			OuterMsg::ViewCellRecord msg;
			if (event.nOpType == RECORD_EVENT_DATA::Add
				&& event.nCol > 0)
			{
				// 列大于0的新增,实际为更改行数据
				msg.set_opera_type(OuterMsg::RECORD_OT_MODIFY);
			}
			else
			{
				msg.set_opera_type(PBConvert::ToPB(event.nOpType));
			}
		
			auto* pList = msg.mutable_record_list();
			if (!PackRecordOneData(pList, event, pRecordData.get(), newData))
			{
				return false;
			}

			// 设置索引
			msg.set_index(pObject->GetIndexInParent());

			auto viewlist = pVisible->GetViewList();
			for (auto& it : viewlist)
			{
				if (it == nullptr)
				{
					continue;
				}

				NFPlayer* pPlayer = it->GetOwner();
				if (pPlayer == nullptr || pPlayer->GetClientID().IsNull())
				{
					continue;
				}

				msg.set_view_id(it->GetId());
				m_pGameServer->SendMsgPBToGate(OuterMsg::SC_VIEW_CELL_RECORD, msg, pPlayer->Self());
			}
		}
	}

	return 0;
}

bool CUpdateViewportModule::PackRecordOneData(OuterMsg::RecordList* pList, const RECORD_EVENT_DATA& event, const NFIRecord* pRecordData, const NFData& data)
{
	if (pList == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pList == NULL";
		return false;
	}

	auto* pRecord = pList->add_record();
	if (pRecord == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pRecord == nullptr";
		return false;
	}

	pRecord->set_name(event.strRecordName);
	auto* pRowData = pRecord->add_row_data();
	if (pRowData == nullptr)
	{
		return false;
	}

	pRowData->set_row(event.nRow);

	switch (event.nOpType)
	{
	case RECORD_EVENT_DATA::Add:
	{
		if (pRecordData == nullptr)
		{
			break;
		}

		for (int i = 0; i < pRecordData->GetCols(); i++)
		{
			NFDATA_TYPE eType = pRecordData->GetColType(i);
			switch (eType)
			{
			case NFDATA_TYPE::TDATA_INT:
			{
				NFINT64 nValue = pRecordData->GetInt(event.nRow, i);
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
				double dwValue = pRecordData->GetFloat(event.nRow, i);
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
				const std::string& strData = pRecordData->GetString(event.nRow, i);
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
				NFGUID ident = pRecordData->GetObject(event.nRow, i);
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
	case RECORD_EVENT_DATA::Update:
	{
		switch (data.GetType())
		{
		case NFDATA_TYPE::TDATA_INT:
		{
			auto* pData = pRowData->add_record_int();
			if (pData != nullptr)
			{
				pData->set_col(event.nCol);
				pData->set_data(data.GetInt());
			}
		}
		break;
		case NFDATA_TYPE::TDATA_FLOAT:
		{
			auto* pData = pRowData->add_record_float();
			if (pData != nullptr)
			{
				pData->set_col(event.nCol);
				pData->set_data(data.GetFloat());
			}
		}
		break;
		case NFDATA_TYPE::TDATA_STRING:
		{
			auto* pData = pRowData->add_record_string();
			if (pData != nullptr)
			{
				pData->set_col(event.nCol);
				pData->set_data(data.GetString());
			}
		}
		break;
		case NFDATA_TYPE::TDATA_OBJECT:
		{
			auto* pData = pRowData->add_record_object();
			if (pData != nullptr)
			{
				pData->set_col(event.nCol);
				PBConvert::ToPB(data.GetObject(), *(pData->mutable_data()));
			}
		}
		break;
		default:
			break;
		}
	}
	break;
	default:
		break;
	}

	return true;
}