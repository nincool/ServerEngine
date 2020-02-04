///--------------------------------------------------------------------
/// 文件名:		NFRecord.cpp
/// 内  容:		表
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <exception>
#include "NFDataList.hpp"
#include "NFRecord.h"
#include "NFComm/NFPublic/Debug.h"
#include "../NFUtils/QLOG.h"

#define QLOG_ERROR_REC QLOG_ERROR << __FUNCTION__ << " self:" << Self() << " rec:" << GetName() << " "

NFRecord::NFRecord(
	const NFGUID& self, 
	const std::string& strRecordName, 
	const NF_SHARE_PTR<const NFDataList>& valueList, 
	const int nMaxRow)
	: m_pColData(valueList)
{
	Assert(m_pColData != nullptr);

    mSelf = self;
    mbSave = false;
    mbPublic = false;
    mbPrivate = false;

    m_strRecName = strRecordName;
    m_nMaxRow = nMaxRow;
	m_vRecData.reserve(nMaxRow/5);
}

NFRecord::~NFRecord()
{
    for (TRECORDCALLBACKEX::iterator iter = mtRecordCallback.begin(); iter != mtRecordCallback.end(); ++iter)
    {
        iter->reset();
    }
    mtRecordCallback.clear();

	Clear();
}

std::string NFRecord::ToString()
{
	std::stringstream ss;
	ss << this->GetName() << std::endl;

	for (int i = 0; i < m_vRecData.size(); ++i)
	{
		ss << "ROW:" << i << "==>" << m_vRecData[i]->ToString() << std::endl;
	}

	return std::move(ss.str());
}

int NFRecord::GetCols() const
{
    return m_pColData->GetCount();
}

int NFRecord::GetRows() const
{
    return m_vRecData.size();
}

int NFRecord::GetMaxRows() const
{
	return m_nMaxRow;
}

NFDATA_TYPE NFRecord::GetColType(const int nCol) const
{
    return m_pColData->Type(nCol);
}

const NF_SHARE_PTR<const NFDataList> NFRecord::GetColData() const
{
	return m_pColData;
}

int NFRecord::AddRow(const int nRow, const NFDataList& var)
{
	return InnerAddRow(nRow, var);
}

int NFRecord::AddRow(const int nRow, NFDataList&& var)
{
	return InnerAddRow(nRow, std::move(var));
}

template<class T>
int NFRecord::InnerAddRow(const int nRow, T&& var)
{
	if (m_vRecData.size() >= m_nMaxRow)
	{
		QLOG_ERROR_REC << "rec full nMaxRow:" << m_nMaxRow;
		return -1;
	}

	NFDataList* pRowData = nullptr;
	if (var.IsEmpty())
	{
		pRowData = AddRowData();
		if (pRowData == nullptr)
		{
			QLOG_ERROR_REC << "new NFDataList failed";
			return -1;
		}
		*pRowData = *m_pColData;
	}
	else
	{
		int nSize = var.GetCount();
		if (m_pColData->GetCount() != nSize)
		{
			QLOG_ERROR_REC << "value size not match";
			return -1;
		}

		for (int i = 0; i < nSize; ++i)
		{
			if (m_pColData->Type(i) != var.Type(i))
			{
				QLOG_ERROR_REC << "value type not match index:" << i;
				return -1;
			}
		}

		pRowData = AddRowData();
		if (pRowData == nullptr)
		{
			QLOG_ERROR_REC << "new NFDataList failed";
			return -1;
		}
		*pRowData = std::forward<T>(var);
	}

	int nAddRow = nRow;
	if (nRow <= 0 || nRow >= m_vRecData.size())
	{
		nAddRow = m_vRecData.size();
		m_vRecData.push_back(pRowData);
	}
	else
	{
		m_vRecData.insert(m_vRecData.begin() + nRow, pRowData);
	}

	RECORD_EVENT_DATA xEventData;
	xEventData.nOpType = RECORD_EVENT_DATA::Add;
	xEventData.nRow = nAddRow;
	xEventData.nCol = 0;
	xEventData.strRecordName = m_strRecName;

	NFData tData;
	OnEventHandler(mSelf, xEventData, tData, tData);

	return nAddRow;
}

bool NFRecord::SetRowValue(const int nRow, const NFDataList & var)
{
	return InnerSetRowValue(nRow, var);
}

bool NFRecord::SetRowValue(const int nRow, NFDataList&& var)
{
	return InnerSetRowValue(nRow, std::move(var));
}

template<class T>
bool NFRecord::InnerSetRowValue(const int nRow, T&& var)
{
	if (nRow < 0 || nRow >= m_vRecData.size())
	{
		QLOG_ERROR_REC << "row:" << nRow << " not find. Rows:" << m_vRecData.size();
		return false;
	}

	if (var.GetCount() != GetCols())
	{
		QLOG_ERROR_REC << "col:" << GetCols() << " " << var.GetCount() << " not match. row:" << nRow;
		return false;
	}

	for (int i = 0; i < GetCols(); ++i)
	{
		if (var.Type(i) != GetColType(i))
		{
			QLOG_ERROR_REC << "col:" << i << " type:" << GetColType(i) << " " << var.Type(i) << " not match. row:" << nRow;
			return false;
		}
	}

	*m_vRecData[nRow] = std::forward<T>(var);

	// 认为是整行更新
	RECORD_EVENT_DATA xEventData;
	xEventData.nOpType = RECORD_EVENT_DATA::Add;
	xEventData.nRow = nRow;
	xEventData.nCol = GetCols();
	xEventData.strRecordName = m_strRecName;

	NFData tData;
	OnEventHandler(mSelf, xEventData, tData, tData);

	return true;
}

const NFDataList& NFRecord::GetRowValue(const int nRow) const
{
	if (!ValidRow(nRow))
	{
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " Rows:" << GetRows();
		return NULL_DATA_LIST;
	}

	return *m_vRecData[nRow];
}

bool NFRecord::SetInt(const int nRow, const int nCol, const NFINT64 value)
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return false;
    }

    if (TDATA_INT != GetColType(nCol))
    {
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
        return false;
    }

	if (mtRecordCallback.empty())
	{
		m_vRecData[nRow]->SetInt(nCol, value);
	}
	else
	{
		auto& var = m_vRecData[nRow]->GetData(nCol);

		NFData oldValue;
		oldValue.SetInt(var.GetInt());

		m_vRecData[nRow]->SetInt(nCol, value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = m_strRecName;

		OnEventHandler(mSelf, xEventData, oldValue, var);
	}

    return true;
}

bool NFRecord::SetFloat(const int nRow, const int nCol, const double value)
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return false;
    }

    if (TDATA_FLOAT != GetColType(nCol))
    {
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
        return false;
    }

	if (mtRecordCallback.empty())
	{
		m_vRecData[nRow]->SetFloat(nCol, value);
	}
	else
	{
		auto& var = m_vRecData[nRow]->GetData(nCol);

		NFData oldValue;
		oldValue.SetInt(var.GetFloat());

		m_vRecData[nRow]->SetFloat(nCol, value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = m_strRecName;

		OnEventHandler(mSelf, xEventData, oldValue, var);
	}


    return true;
}

bool NFRecord::SetString(const int nRow, const int nCol, const std::string& value)
{
	return InnerSetString(nRow, nCol, value);
}

bool NFRecord::SetString(const int nRow, const int nCol, std::string&& value)
{
	return InnerSetString(nRow, nCol, std::move(value));
}

template<class T>
bool NFRecord::InnerSetString(const int nRow, const int nCol, T&& value)
{
	if (!ValidPos(nRow, nCol))
	{
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
		return false;
	}

	if (TDATA_STRING != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
		return false;
	}

	if (mtRecordCallback.empty())
	{
		m_vRecData[nRow]->SetString(nCol, std::forward<T>(value));
	}
	else
	{
		auto& var = m_vRecData[nRow]->GetData(nCol);

		NFData oldValue;
		oldValue.SetString(var.GetString());

		m_vRecData[nRow]->SetString(nCol, std::forward<T>(value));

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = m_strRecName;

		OnEventHandler(mSelf, xEventData, oldValue, var);
	}

	return true;
}

bool NFRecord::SetObject(const int nRow, const int nCol, const NFGUID& value)
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return false;
    }

    if (TDATA_OBJECT != GetColType(nCol))
    {
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
        return false;
    }

	if (mtRecordCallback.empty())
	{
		m_vRecData[nRow]->SetObject(nCol, value);
	}
	else
	{
		auto& var = m_vRecData[nRow]->GetData(nCol);

		NFData oldValue;
		oldValue.SetObject(var.GetObject());

		m_vRecData[nRow]->SetObject(nCol, value);

		RECORD_EVENT_DATA xEventData;
		xEventData.nOpType = RECORD_EVENT_DATA::Update;
		xEventData.nRow = nRow;
		xEventData.nCol = nCol;
		xEventData.strRecordName = m_strRecName;

		OnEventHandler(mSelf, xEventData, oldValue, var);
	}

    return true;
}

NFINT64 NFRecord::GetInt(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return 0;
    }

	if (TDATA_INT != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
		return 0;
	}

    return m_vRecData[nRow]->Int(nCol);
}

double NFRecord::GetFloat(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return 0.0f;
    }

	if (TDATA_FLOAT != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
		return 0.0f;
	}

	return m_vRecData[nRow]->Float(nCol);
}

const std::string& NFRecord::GetString(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return NULL_STR;
    }

	if (TDATA_STRING != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
		return NULL_STR;
	}

	return m_vRecData[nRow]->String(nCol);
}

const NFGUID& NFRecord::GetObject(const int nRow, const int nCol) const
{
    if (!ValidPos(nRow, nCol))
    {
		QLOG_ERROR_REC << "not valid pos nRow:" << nRow << " nCol:" << nCol << " Rows:" << GetRows() << " Cols:" << GetCols();
        return NULL_OBJECT;
    }

	if (TDATA_OBJECT != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col type:" << GetColType(nCol) << " not match";
		return NULL_OBJECT;
	}

	return m_vRecData[nRow]->Object(nCol);
}

int NFRecord::FindRow(const int nCol, const NFData& var, int nStartRow/* = 0*/) const
{
	NFDATA_TYPE eType = var.GetType();
    switch (eType)
    {
        case TDATA_INT:
            return FindInt(nCol, var.GetInt(), nStartRow);
            break;

        case TDATA_FLOAT:
            return FindFloat(nCol, var.GetFloat(), nStartRow);
            break;

        case TDATA_STRING:
            return FindString(nCol, var.GetString(), nStartRow);
            break;

        case TDATA_OBJECT:
            return FindObject(nCol, var.GetObject(), nStartRow);
            break;

        default:
            break;
    }

    return -1;
}

int NFRecord::FindRow(const NFDataList& cols, const NFDataList& vars, int nStartRow/* = 0*/) const
{
	for (int i = 0; i < cols.GetCount(); ++i)
	{
		if (!ValidCol(cols.Int(i)))
		{
			QLOG_ERROR_REC << "not valid col:" << i << " Cols:" << GetCols();
			return -1;
		}

		if (vars.Type(i) != GetColType(i))
		{
			QLOG_ERROR_REC << "col:" << i << " type:" << GetColType(i) << " " << vars.Type(i) << " not match";
			return -1;
		}
	}

	for (int i = nStartRow; i < m_vRecData.size(); ++i)
	{
		for (int j = 0; j < cols.GetCount(); ++j)
		{
			int nCol = cols.Int(j);
			if (m_vRecData[i]->GetData(nCol) == vars.GetData(nCol))
			{
				if (j == cols.GetCount() - 1)
				{
					return i;
				}
			}
		}
	}

	return -1;
}

int NFRecord::FindInt(const int nCol, const NFINT64 value, int nStartRow/* = 0*/) const
{
    if (!ValidCol(nCol))
    {
		QLOG_ERROR_REC << "not valid col:" << nCol << " Cols:" << GetCols();
        return -1;
    }

    if (TDATA_INT != GetColType(nCol))
    {
		QLOG_ERROR_REC << "col:" << nCol <<" type:" << GetColType(nCol) << " not match";
        return -1;
    }

	for (int i = nStartRow; i < m_vRecData.size(); ++i)
	{
		if (m_vRecData[i]->Int(nCol) == value)
		{
			return i;
		}
	}

    return -1;
}

int NFRecord::FindFloat(const int nCol, const double value, int nStartRow/* = 0*/) const
{
    if (!ValidCol(nCol))
    {
		QLOG_ERROR_REC << "not valid col:" << nCol << " Cols:" << GetCols();
        return -1;
    }

	if (TDATA_FLOAT != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col:" << nCol << " type:" << GetColType(nCol) << " not match";
		return -1;
	}

	for (int i = nStartRow; i < m_vRecData.size(); ++i)
	{
		if (m_vRecData[i]->Float(nCol) == value)
		{
			return i;
		}
	}

    return -1;
}

int NFRecord::FindString(const int nCol, const std::string& value, int nStartRow/* = 0*/) const
{
    if (!ValidCol(nCol))
    {
		QLOG_ERROR_REC << "not valid col:" << nCol << " Cols:" << GetCols();
        return -1;
    }

	if (TDATA_STRING != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col:" << nCol << " type:" << GetColType(nCol) << " not match";
		return -1;
	}

	for (int i = nStartRow; i < m_vRecData.size(); ++i)
	{
		if (m_vRecData[i]->String(nCol) == value)
		{
			return i;
		}
	}

    return -1;
}

int NFRecord::FindObject(const int nCol, const NFGUID& value, int nStartRow/* = 0*/) const
{
    if (!ValidCol(nCol))
    {
		QLOG_ERROR_REC << "not valid col:" << nCol << " Cols:" << GetCols();
        return -1;
    }

	if (TDATA_OBJECT != GetColType(nCol))
	{
		QLOG_ERROR_REC << "col:" << nCol << " type:" << GetColType(nCol) << " not match";
		return -1;
	}

	for (int i = nStartRow; i < m_vRecData.size(); ++i)
	{
		if (m_vRecData[i]->Object(nCol) == value)
		{
			return i;
		}
	}

    return -1;
}

bool NFRecord::Remove(const int nRow)
{
	if (!ValidRow(nRow))
	{
		return false;
	}

	RECORD_EVENT_DATA xEventData;
	xEventData.nOpType = RECORD_EVENT_DATA::Del;
	xEventData.nRow = nRow;
	xEventData.nCol = 0;
	xEventData.strRecordName = m_strRecName;

	OnEventHandler(mSelf, xEventData, NFData(), NFData());

	FreeRowData(m_vRecData[nRow]);
	m_vRecData.erase(m_vRecData.begin() + nRow);

	return true;
}

bool NFRecord::Clear()
{
	RECORD_EVENT_DATA xEventData;
	xEventData.nOpType = RECORD_EVENT_DATA::Cleared;
	xEventData.nRow = 0;
	xEventData.nCol = 0;
	xEventData.strRecordName = m_strRecName;

	OnEventHandler(mSelf, xEventData, NULL_TDATA, NULL_TDATA);

	for (auto& it : m_vRecData)
	{
		delete it;
	}
	m_vRecData.clear();

	for (auto& it : m_listPool)
	{
		delete it;
	}
	m_listPool.clear();

    return true;
}

void NFRecord::AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb)
{
    mtRecordCallback.push_back(cb);
}

const bool NFRecord::GetSave() const
{
    return mbSave;
}

const bool NFRecord::GetPublic() const
{
    return mbPublic;
}

const bool NFRecord::GetPrivate() const
{
    return mbPrivate;
}

const std::string& NFRecord::GetName() const
{
    return m_strRecName;
}

const int NFRecord::GetTCallBackCount() const
{
	return static_cast<int>(mtRecordCallback.size());
}

void NFRecord::SetSave(const bool bSave)
{
    mbSave = bSave;
}

void NFRecord::SetPublic(const bool bPublic)
{
    mbPublic = bPublic;
}

void NFRecord::SetPrivate(const bool bPrivate)
{
    mbPrivate = bPrivate;
}


const NFGUID& NFRecord::Self() const
{
	return mSelf;
}

void NFRecord::SetName(const std::string& strName)
{
    m_strRecName = strName;
}

void NFRecord::OnEventHandler(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar)
{
    TRECORDCALLBACKEX::iterator itr = mtRecordCallback.begin();
    TRECORDCALLBACKEX::iterator end = mtRecordCallback.end();
    for (; itr != end; ++itr)
    {
        
        RECORD_EVENT_FUNCTOR_PTR functorPtr = *itr;
        functorPtr.get()->operator()(self, xEventData, oldVar, newVar);
    }
}

NFDataList* NFRecord::AddRowData()
{
	NFDataList* data = nullptr;
	if (m_listPool.empty())
	{
		data = NF_NEW NFDataList();
	}
	else
	{
		data = m_listPool.back();
		m_listPool.pop_back();
	}

	return data;
}

bool NFRecord::FreeRowData(NFDataList* data)
{
	m_listPool.push_back(data);
	return true;
}

bool NFRecord::IsEmpty() const
{
	return m_vRecData.empty();
}

bool NFRecord::ValidPos(int nRow, int nCol) const
{
    if (ValidCol(nCol)
        && ValidRow(nRow))
    {
        return true;
    }

    return false;
}

bool NFRecord::ValidRow(int nRow) const
{
    if (nRow >= m_vRecData.size() || nRow < 0)
    {
        return false;
    }

    return true;
}

bool NFRecord::ValidCol(int nCol) const
{
    if (nCol >= GetCols() || nCol < 0)
    {
        return false;
    }

    return true;
}

bool NFRecord::IsBytes(int nCol) const
{
	return m_pColData->TypeEx(nCol) == TDATA_BYTES;
}

bool NFRecord::PrefillRow(int nRow)
{
	if (nRow < 0 || nRow >= m_nMaxRow)
	{
		return false;
	}

	if (nRow < m_vRecData.size())
	{
		return true;
	}

	int nAddRows = nRow - m_vRecData.size() + 1;
	for (int i = 0; i < nAddRows; ++i)
	{
		AddRow(-1, NULL_DATA_LIST);
	}

	return true;
}
