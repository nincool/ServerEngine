///--------------------------------------------------------------------
/// 文件名:		PBConvert.hpp
/// 内  容:		NF与PB的数据转换和搜集
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __PB_CONVERT_H__
#define __PB_CONVERT_H__

#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIProperty.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFRecord.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "NFComm/NFUtils/QLOG.h"

#ifdef _MSC_VER
#pragma warning(disable: 4244 4267)
#endif

class PBConvert //: public LPBConvert
{
public:
	static bool ToNF(const OuterMsg::GUID& xID, NFGUID& ident)
	{
		ident.nHead64 = xID.data1();
		ident.nData64 = xID.data2();

		return true;
	}

	static bool ToPB(const NFGUID& xID, OuterMsg::GUID& ident)
	{
		ident.set_data1(xID.nHead64);
		ident.set_data2(xID.nData64);

		return true;
	}

	static RECORD_EVENT_DATA::RecordOptype ToNF(OuterMsg::RecordOperatorType eType)
	{
		switch (eType)
		{
		case OuterMsg::RecordOperatorType::RECORD_OT_ADD:
			return RECORD_EVENT_DATA::RecordOptype::Add;
			break;
		case OuterMsg::RecordOperatorType::RECORD_OT_REMOVE:
			return RECORD_EVENT_DATA::RecordOptype::Del;
			break;
		case OuterMsg::RecordOperatorType::RECORD_OT_MODIFY:
			return RECORD_EVENT_DATA::RecordOptype::Update;
			break;
		case OuterMsg::RecordOperatorType::RECORD_OT_CLEAR:
			return RECORD_EVENT_DATA::RecordOptype::Cleared;
			break;
		}
		return RECORD_EVENT_DATA::RecordOptype::UNKNOW;
	}

	static OuterMsg::RecordOperatorType ToPB(RECORD_EVENT_DATA::RecordOptype eType)
	{
		switch (eType)
		{
		case RECORD_EVENT_DATA::RecordOptype::Add:
			return OuterMsg::RecordOperatorType::RECORD_OT_ADD;
			break;
		case RECORD_EVENT_DATA::RecordOptype::Del:
			return OuterMsg::RecordOperatorType::RECORD_OT_REMOVE;
			break;
		case RECORD_EVENT_DATA::RecordOptype::Update:
			return OuterMsg::RecordOperatorType::RECORD_OT_MODIFY;
			break;
		case RECORD_EVENT_DATA::RecordOptype::Cleared:
			return OuterMsg::RecordOperatorType::RECORD_OT_CLEAR;
			break;
		}
		return OuterMsg::RecordOperatorType::RECORD_OT_UNKNOW;
	}

	static OuterMsg::ObjectType ToObjectType(std::string& strType)
	{
		if (strType.find("TYPE_PLAYER") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_PLAYER;
		}
		else if (strType.find("TYPE_NPC") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_NPC;
		}
		else if (strType.find("TYPE_ITEM") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_ITEM;
		}
		else if (strType.find("TYPE_HELPER") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_HELPER;
		}
		else if (strType.find("TYPE_CONTAINER") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_CONTAINER;
		}
		else if (strType.find("TYPE_PUB") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_PUB;
		}
		else if (strType.find("TYPE_GUILD") != std::string::npos)
		{
			return OuterMsg::ObjectType::OBJECT_TYPE_GUILD;
		}

		return OuterMsg::ObjectType::OBJECT_TYPE_UNKNOW;
	}

	// 打包属性数据
	static bool ToPBPropertyManager(NFIPropertyManager* pPropertyManager,
		OuterMsg::PropertyList* pPropertyList,
		std::function<bool(NFIProperty&)> pf = [](NFIProperty&) { return true; })
	{
		if (pPropertyManager == nullptr || pPropertyList == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pPropertyManager or pPropertyList == NULL";
			return false;
		}

		NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->First();
		while (pProperty != nullptr)
		{
			if (pf(*pProperty))
			{
				switch (pProperty->GetType())
				{
				case TDATA_INT:
				{
					OuterMsg::PropertyInt* pPropertyData = pPropertyList->add_property_int();
					if (pPropertyData != nullptr)
					{
						pPropertyData->set_name(pProperty->GetName());
						pPropertyData->set_data(pProperty->GetInt());
					}
				}
				break;

				case TDATA_FLOAT:
				{
					OuterMsg::PropertyFloat* pPropertyData = pPropertyList->add_property_float();
					if (pPropertyData != nullptr)
					{
						pPropertyData->set_name(pProperty->GetName());
						pPropertyData->set_data(pProperty->GetFloat());
					}
				}
				break;

				case TDATA_STRING:
				{
					OuterMsg::PropertyString* pPropertyData = pPropertyList->add_property_string();
					if (pPropertyData != nullptr)
					{
						pPropertyData->set_name(pProperty->GetName());
						pPropertyData->set_data(pProperty->GetString());
					}
				}
				break;

				case TDATA_OBJECT:
				{
					OuterMsg::PropertyObject* pPropertyData = pPropertyList->add_property_object();
					if (pPropertyData != nullptr)
					{
						pPropertyData->set_name(pProperty->GetName());
						PBConvert::ToPB(pProperty->GetObject(), *(pPropertyData->mutable_data()));
					}
				}
				break;

				default:
					break;
				}
			}

			pProperty = pPropertyManager->Next();
		}

		return true;
	}
	
	// 解包属性数据 
	// bDoSave 只处理保存属性
	static bool ToNFPropertyManager(const OuterMsg::PropertyList* pPropertyList, NFIPropertyManager* pPropertyManager, bool bDoSave = false)
	{
		if (pPropertyManager == nullptr || pPropertyList == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pPropertyManager or pPropertyList == NULL";
			return false;
		}

		// 设置所有内部属性可以被初始化
		pPropertyManager->SetRawPropertyEdit(EDIT_OK);

		// 清空所有保存的属性
		if (bDoSave)
		{
			auto pProp = pPropertyManager->First();
			while (pProp != nullptr)
			{
				if (pProp->GetSave())
				{
					pProp->Clear();
				}
				pProp = pPropertyManager->Next();
			}
		}
		
		for (int i = 0; i < pPropertyList->property_int_size(); ++i)
		{
			const OuterMsg::PropertyInt& xData = pPropertyList->property_int(i);

			auto pProp = pPropertyManager->GetElement(xData.name());
			if (bDoSave)
			{
				if (pProp == nullptr || !pProp->GetSave())
				{
					continue;
				}
			}
			else
			{
				if (pProp == nullptr)
				{
					pProp = pPropertyManager->AddProperty(pPropertyManager->Self(), xData.name(), NFDATA_TYPE::TDATA_INT);
				}
			}

			pProp->SetInt(xData.data());
		}

		for (int i = 0; i < pPropertyList->property_float_size(); ++i)
		{
			const OuterMsg::PropertyFloat& xData = pPropertyList->property_float(i);

			auto pProp = pPropertyManager->GetElement(xData.name());
			if (bDoSave)
			{
				if (pProp == nullptr || !pProp->GetSave())
				{
					continue;
				}
			}
			else
			{
				if (pProp == nullptr)
				{
					pProp = pPropertyManager->AddProperty(pPropertyManager->Self(), xData.name(), NFDATA_TYPE::TDATA_FLOAT);
				}
			}

			pProp->SetFloat(xData.data());
		}

		for (int i = 0; i < pPropertyList->property_string_size(); ++i)
		{
			const OuterMsg::PropertyString& xData = pPropertyList->property_string(i);

			auto pProp = pPropertyManager->GetElement(xData.name());
			if (bDoSave)
			{
				if (pProp == nullptr || !pProp->GetSave())
				{
					continue;
				}
			}
			else
			{
				if (pProp == nullptr)
				{
					pProp = pPropertyManager->AddProperty(pPropertyManager->Self(), xData.name(), NFDATA_TYPE::TDATA_STRING);
				}
			}

			pProp->SetString(xData.data());
		}

		for (int i = 0; i < pPropertyList->property_object_size(); ++i)
		{
			const OuterMsg::PropertyObject& xData = pPropertyList->property_object(i);

			auto pProp = pPropertyManager->GetElement(xData.name());
			if (bDoSave)
			{
				if (pProp == nullptr || !pProp->GetSave())
				{
					continue;
				}
			}
			else
			{
				if (pProp == nullptr)
				{
					pProp = pPropertyManager->AddProperty(pPropertyManager->Self(), xData.name(), NFDATA_TYPE::TDATA_OBJECT);
				}
			}

			NFGUID ident;
			PBConvert::ToNF(xData.data(), ident);
			pProp->SetObject(ident);
		}

		// 还原所有内部属性是否可以被修改
		pPropertyManager->RestoreRawPropertyEdit();

		return true;
	}

	// 打包单个表数据
	static bool ToPBRecord(const NFIRecord* pRecord, OuterMsg::Record* pOuterRecord)
	{
		if (pRecord == nullptr || pOuterRecord == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pRecord or pOuterRecord == NULL";
			return false;
		}

		pOuterRecord->set_name(pRecord->GetName().c_str());

		for (int i = 0; i < pRecord->GetRows(); i++)
		{
			OuterMsg::RecordRow* pRowData = pOuterRecord->add_row_data();
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
					const NFGUID& ident = pRecord->GetObject(i, j);
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

	// 解包单个表数据
	// bDoSave 只处理保存数据
	static bool ToNFRecord(const OuterMsg::Record* pOuterRecord, NFIRecord* pRecord, bool bDoSave = false)
	{
		NFRecord* pRec = dynamic_cast<NFRecord*>(pRecord);
		if (pRec == nullptr || pOuterRecord == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pRec or pOuterRecord == NULL";
			return false;
		}

		if (bDoSave && !pRecord->GetSave())
		{
			return false;
		}

		pRecord->Clear();
		for (int i = 0; i < pOuterRecord->row_data_size(); i++)
		{
			const OuterMsg::RecordRow& xRowData = pOuterRecord->row_data(i);
			int nRow = xRowData.row();

			pRec->PrefillRow(nRow);

			for (int i = 0; i < xRowData.record_int_size(); ++i)
			{
				const OuterMsg::RecordInt& xColData = xRowData.record_int(i);
				const int nCol = xColData.col();
				pRec->SetInt(nRow, nCol, xColData.data());
			}

			for (int i = 0; i < xRowData.record_float_size(); ++i)
			{
				const OuterMsg::RecordFloat& xColData = xRowData.record_float(i);
				const int nCol = xColData.col();
				pRec->SetFloat(nRow, nCol, xColData.data());
			}

			for (int i = 0; i < xRowData.record_string_size(); ++i)
			{
				const OuterMsg::RecordString& xColData = xRowData.record_string(i);
				const int nCol = xColData.col();
				pRec->SetString(nRow, nCol, xColData.data());
			}

			for (int i = 0; i < xRowData.record_object_size(); ++i)
			{
				const OuterMsg::RecordObject& xColData = xRowData.record_object(i);
				const int nCol = xColData.col();
				NFGUID ident;
				PBConvert::ToNF(xColData.data(), ident);
				pRec->SetObject(nRow, nCol, ident);
			}
		}

		return true;
	}

	// 打包表数据
	static bool ToPBRecordManager(NFIRecordManager* pRecordManager,
		OuterMsg::RecordList* pRecordList,
		std::function<bool(NFIRecord&)> pf = [](NFIRecord&) { return true; })
	{
		if (pRecordManager == nullptr || pRecordList == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pRecordManager or pRecordList == NULL";
			return false;
		}

		NF_SHARE_PTR<NFIRecord> pRecord = pRecordManager->First();
		while (pRecord != nullptr)
		{
			if (pf(*pRecord))
			{
				if (!ToPBRecord(pRecord.get(), pRecordList->add_record()))
				{
					QLOG_ERROR << __FUNCTION__ << " error record:" << pRecord->GetName();
				}
			}

			pRecord = pRecordManager->Next();
		}

		return true;
	}

	// 解包表数据
	static bool ToNFRecordManager(const OuterMsg::RecordList* pRecordList, NFIRecordManager* pRecordManager, bool bDoSave = false)
	{
		if (pRecordManager == nullptr || pRecordList == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pRecordManager or pRecordList == NULL";
			return false;
		}

		for (int i = 0; i < pRecordList->record_size(); ++i)
		{
			const OuterMsg::Record& xRecordBase = pRecordList->record(i);
			NF_SHARE_PTR<NFIRecord> xRecord = pRecordManager->GetElement(xRecordBase.name());
			if (xRecord)
			{
				if (!PBConvert::ToNFRecord(&xRecordBase, xRecord.get(), bDoSave))
				{
					QLOG_ERROR << __FUNCTION__ << " error record:" << xRecord->GetName();
				}
			}
		}

		return true;
	}

	// 打包对象数据
	static bool ToPBObject(NFIKernelModule* pKernel, const NFGUID& self,
		OuterMsg::ObjectDataPack* pObjPack,
		std::function<bool(NFIProperty&)> pfp = [](NFIProperty&) { return true; },
		std::function<bool(NFIRecord&)> pfr = [](NFIRecord&) { return true; })
	{
		if (pKernel == nullptr || pObjPack == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pKernel or pObjPack == NULL";
			return false;
		}

		NFObject* pObj = dynamic_cast<NFObject*>(pKernel->GetObject(self).get());
		if (pObj == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " not find obj:" << self;
			return false;
		}

		PBConvert::ToPB(pObj->Self(), *pObjPack->mutable_ident());

		NF_SHARE_PTR<NFIPropertyManager> pPropManager = pObj->GetPropertyManager();
		NF_SHARE_PTR<NFIRecordManager> pRecordManager = pObj->GetRecordManager();
		if (pPropManager != nullptr)
		{
			PBConvert::ToPBPropertyManager(pPropManager.get(), pObjPack->mutable_property_list(), pfp);
		}
		if (pRecordManager != nullptr)
		{
			PBConvert::ToPBRecordManager(pRecordManager.get(), pObjPack->mutable_record_list(), pfr);
		}

		const vector<NFGUID>& children = dynamic_cast<NFObject*>(pObj)->GetChildList();
		for (auto& it : children)
		{
			if (it.IsNull())
			{
				continue;
			}

			if (!ToPBObject(pKernel, it, pObjPack->add_child(), pfp, pfr))
			{
				QLOG_ERROR << __FUNCTION__ << " error object:" << it;
				continue;
			}
		}

		return true;
	}

	// 解包对象数据
	static NF_SHARE_PTR<NFIObject> ToNFObject(NFKernelModule* pKernel, 
		const NFGUID& parent, 
		const OuterMsg::ObjectDataPack* pObjPack, 
		const NFDataList& args,
		bool bPlayer = false,
		bool bClone = false/*克隆对象，不使用真实的NFGUID*/,
		bool bDoSave = false/*只处理保存数据*/)
	{
		if (pKernel == nullptr || pObjPack == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " pKernel or pObjPack == NULL";
			return nullptr;
		}

		NFGUID obj;
		PBConvert::ToNF(pObjPack->ident(), obj);
		NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(pKernel->GetObject(obj));
		if (pObj != nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " not find obj:" << obj;
			return nullptr;
		}

		// 先找出逻辑类名和配置ID
		std::string strClassName = "";
		std::string strConfig = "";
		if (!bPlayer)
		{
			int bBreak = false;
			const OuterMsg::PropertyList& propList = pObjPack->property_list();
			for (int i = 0; i < propList.property_string_size(); ++i)
			{
				const OuterMsg::PropertyString& xData = propList.property_string(i);

				if (xData.name() == LC::IObject::ClassName())
				{
					strClassName = xData.data();
					if (bBreak) break;
					bBreak = true;
				}
				else if (xData.name() == LC::IObject::ConfigID())
				{
					strConfig = xData.data();
					if (bBreak) break;
					bBreak = true;
				}
			}
		}
		else
		{
			strClassName = LC::Player::ThisName();
		}

		auto pf = [&](NF_SHARE_PTR<NFIObject> pObject)
		{
			NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(pObject);
			if (pObj == nullptr)
			{
				return;
			}

			NF_SHARE_PTR<NFIPropertyManager> pPropManager = pObj->GetPropertyManager();
			NF_SHARE_PTR<NFIRecordManager> pRecordManager = pObj->GetRecordManager();
			if (pPropManager != nullptr)
			{
				PBConvert::ToNFPropertyManager(&pObjPack->property_list(), pPropManager.get(), bDoSave);
			}
			if (pRecordManager != nullptr)
			{
				PBConvert::ToNFRecordManager(&pObjPack->record_list(), pRecordManager.get(), bDoSave);
			}
			// 在容器中的位置
			int nIndex = pPropManager->GetPropertyInt(LC::IObject::Index());

			NF_SHARE_PTR<NFObject> pParent = dynamic_pointer_cast<NFObject>(pKernel->GetObject(parent));
			if (pParent != nullptr)
			{
				if (!pParent->AddChild(pObj->Self(), nIndex))
				{
					QLOG_ERROR << __FUNCTION__ << " error add obj:" << obj
						<< " ClassName:" << strClassName
						<< " Config:" << strConfig
						<< " Index:" << nIndex;
					return;
				}
			}

			for (int i = 0; i < pObjPack->child_size(); ++i)
			{
				const OuterMsg::ObjectDataPack& xChildData = pObjPack->child(i);
				if (ToNFObject(pKernel, obj, &xChildData, NFDataList()) == nullptr)
				{
					QLOG_ERROR << __FUNCTION__ << " object:" << obj;
					continue;
				}
			}

			// 数据恢复完成事件
			pObj->OnEvent(EVENT_OnRecover, NFGUID(), NFDataList());
		};

		// 克隆对象，创建新的GUID
		if (bClone)
		{
			obj = pKernel->CreateGUID();
		}
		return pKernel->CreateObject(obj, strClassName, strConfig, args, pf);
	}

	
	static bool NFDataToPBData(const NFData& src, OuterMsg::PBData& pb)
	{
		pb.set_v_type(src.GetType());

		switch (pb.v_type())
		{
		case TDATA_INT:
			pb.set_v_int(src.GetInt());
			break;

		case TDATA_FLOAT:
			pb.set_v_float(src.GetFloat());
			break;

		case TDATA_STRING:
			pb.set_v_string(src.GetString());
			break;

		case TDATA_OBJECT:

			ToPB(src.GetObject(), *pb.mutable_v_guid());
			break;

		default:
			return false;
			break;
		}

		return true;
	}

	static bool PBDataToNFData(const OuterMsg::PBData& src, NFData& des)
	{
		switch (src.v_type())
		{
		case TDATA_INT:
			des.SetInt(src.v_int());
			break;

		case TDATA_FLOAT:
			des.SetFloat(src.v_float());
			break;

		case TDATA_STRING:
			des.SetString(src.v_string());
			break;

		case TDATA_OBJECT:
		{
			NFGUID uid;
			ToNF(src.v_guid(), uid);
			des.SetObject(uid);
			break;
		}
		default:
			return false;
			break;
		}

		return true;
	}

	static bool NFDataListToPBDataList(const NFDataList& src, OuterMsg::PBDataList& des)
	{
		int nCount = src.GetCount();
		for (int i = 0; i < nCount; ++i)
		{
			NFDataToPBData(src.GetData(i), *des.add_v_data());
		}

		return true;
	}

	static bool PBDataListToNFDataList(const OuterMsg::PBDataList& src, NFDataList& des)
	{
		NFData data;

		int nCount = src.v_data_size();
		for (int i = 0; i < nCount; ++i)
		{
			const OuterMsg::PBData& pbData = src.v_data(i);
			data.Reset();
			PBDataToNFData(pbData, data);
			des.Add(data);
		}

		return true;
	}
};

#endif //  __PB_CONVERT_H__
