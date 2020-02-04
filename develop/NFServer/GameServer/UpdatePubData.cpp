///--------------------------------------------------------------------
/// 文件名:		UpdatePubData.cpp
/// 内  容:		属性和表同步PUB数据
/// 说  明:		
/// 创建日期:	2019年9月16日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "UpdatePubData.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"

CUpdatePubData::CUpdatePubData(NFIPluginManager* p)
{
	pPluginManager = p;
}

CUpdatePubData::~CUpdatePubData()
{

}

bool CUpdatePubData::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();

	return true;
}

bool CUpdatePubData::AfterInit()
{
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_OBJECT_PROPERTY, this, (NetMsgFun)&CUpdatePubData::SynPubDataProperty);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_OBJECT_TABLE, this, (NetMsgFun)&CUpdatePubData::SynPubDataRecord);
	return true;
}

void CUpdatePubData::SynPubDataProperty(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectPropertyList xObjectPropertyList;
	if (!xObjectPropertyList.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectPropertyList ParseFromArray failed";
		return;
	}

	NFGUID id;
	PBConvert::ToNF(xObjectPropertyList.ident(), id);

	//获得玩家
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(id));
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find:" << id;
		return;
	}

	OuterMsg::PropertyList xPropertyList = xObjectPropertyList.property_list();
	NF_SHARE_PTR<NFIPropertyManager> pPropManager = pObject->GetPropertyManager();
	if (pPropManager != nullptr)
	{
		PBConvert::ToNFPropertyManager(&xPropertyList, pPropManager.get());
	}

	return;
}

void CUpdatePubData::SynPubDataRecord(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectVisRecord xObjectRecordList;
	if (!xObjectRecordList.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectVisRecord ParseFromArray failed";
		return;
	}

	NFGUID id;
	PBConvert::ToNF(xObjectRecordList.ident(), id);

	//获得玩家
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(id));
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find:" << id;
		return;
	}

	int nOpType = xObjectRecordList.opera_type();

	OuterMsg::RecordList xRecordList = xObjectRecordList.record_list();
	for (int i = 0; i < xRecordList.record_size(); ++i)
	{
		const OuterMsg::Record& xRecordBase = xRecordList.record(i);
		NF_SHARE_PTR<NFRecord> xRecord = dynamic_pointer_cast<NFRecord>(pObject->GetRecord(xRecordBase.name()));
		if (xRecord != nullptr)
		{
			switch (nOpType)
			{
			case OuterMsg::RECORD_OT_ADD:
			{
				for (int j = 0; j < xRecordBase.row_data_size(); j++)
				{
					const OuterMsg::RecordRow& xRowData = xRecordBase.row_data(j);
					int nRow = xRowData.row();
					NFDataList xdata;
					for (int i = 0; i < xRecord->GetCols(); ++i)
					{
						switch (xRecord->GetColType(i))
						{
						case TDATA_INT:
							xdata.Add(NFINT64(0));
							break;
						case TDATA_FLOAT:
							xdata.Add(0.0f);
							break;
						case TDATA_STRING:
							xdata.Add(NULL_STR.c_str());
							break;
						case TDATA_OBJECT:
							xdata.Add(NFGUID());
							break;
						default:
							break;
						}
					}
					for (int k = 0; k < xRowData.record_int_size(); ++k)
					{
						const OuterMsg::RecordInt& xColData = xRowData.record_int(k);
						const int nCol = xColData.col();
						xdata.SetInt(nCol, xColData.data());
					}

					for (int k = 0; k < xRowData.record_float_size(); ++k)
					{
						const OuterMsg::RecordFloat& xColData = xRowData.record_float(k);
						const int nCol = xColData.col();
						xdata.SetFloat(nCol, xColData.data());
					}

					for (int k = 0; k < xRowData.record_string_size(); ++k)
					{
						const OuterMsg::RecordString& xColData = xRowData.record_string(k);
						const int nCol = xColData.col();
						xdata.SetString(nCol, xColData.data());
					}

					for (int k = 0; k < xRowData.record_object_size(); ++k)
					{
						const OuterMsg::RecordObject& xColData = xRowData.record_object(k);
						const int nCol = xColData.col();
						NFGUID ident;
						PBConvert::ToNF(xColData.data(), ident);
						xdata.SetObject(nCol, ident);
					}

					xRecord->AddRow(nRow, xdata);
				}
			}
			break;

			case OuterMsg::RECORD_OT_REMOVE:
			{
				for (int j = 0; j < xRecordBase.row_data_size(); j++)
				{
					const OuterMsg::RecordRow& xRowData = xRecordBase.row_data(j);
					int nRow = xRowData.row();
					xRecord->Remove(nRow);
				}
			}
			break;

			case OuterMsg::RECORD_OT_MODIFY:
			{
				for (int j = 0; j < xRecordBase.row_data_size(); j++)
				{
					const OuterMsg::RecordRow& xRowData = xRecordBase.row_data(j);
					int nRow = xRowData.row();
					for (int k = 0; k < xRowData.record_int_size(); ++k)
					{
						const OuterMsg::RecordInt& xColData = xRowData.record_int(k);
						const int nCol = xColData.col();
						xRecord->SetInt(nRow, nCol, xColData.data());
					}

					for (int k = 0; k < xRowData.record_float_size(); ++k)
					{
						const OuterMsg::RecordFloat& xColData = xRowData.record_float(k);
						const int nCol = xColData.col();
						xRecord->SetFloat(nRow, nCol, xColData.data());
					}

					for (int k = 0; k < xRowData.record_string_size(); ++k)
					{
						const OuterMsg::RecordString& xColData = xRowData.record_string(k);
						const int nCol = xColData.col();
						xRecord->SetString(nRow, nCol, xColData.data());
					}

					for (int k = 0; k < xRowData.record_object_size(); ++k)
					{
						const OuterMsg::RecordObject& xColData = xRowData.record_object(k);
						const int nCol = xColData.col();
						NFGUID ident;
						PBConvert::ToNF(xColData.data(), ident);
						xRecord->SetObject(nRow, nCol, ident);
					}
				}
			}
			break;

			case OuterMsg::RECORD_OT_CLEAR:
			{
				xRecord->Clear();
			}
			break;
			default:
				break;
			}
		}
	}

	return;
}