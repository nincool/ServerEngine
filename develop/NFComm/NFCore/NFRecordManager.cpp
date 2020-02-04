///--------------------------------------------------------------------
/// 文件名:		NFRecordManager.cpp
/// 内  容:		表管理
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFRecordManager.h"

NFRecordManager::~NFRecordManager()
{
    ClearAll();
}

NF_SHARE_PTR<NFIRecord> NFRecordManager::AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<const NFDataList>& ValueList, const int nRows)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (!pRecord)
    {
        pRecord = NF_SHARE_PTR<NFIRecord>(NF_NEW NFRecord(self, strRecordName, ValueList, nRows));
        this->AddElement(strRecordName, pRecord);
    }

    return pRecord;
}

const NFGUID& NFRecordManager::Self()
{
    return mSelf;
}

std::string NFRecordManager::ToString()
{
	std::stringstream stream;
	NF_SHARE_PTR<NFIRecord> pRecord = First();
	while (pRecord)
	{
		stream << pRecord->ToString() << std::endl;
		pRecord = Next();
	}

	return stream.str();
}

bool NFRecordManager::SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 nValue)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->SetInt(nRow, nCol, nValue);
    }

    return false;
}

bool NFRecordManager::SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->SetFloat(nRow, nCol, dwValue);
    }

    return false;
}
bool NFRecordManager::SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->SetString(nRow, nCol, strValue);
    }

    return false;
}

bool NFRecordManager::SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& obj)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->SetObject(nRow, nCol, obj);
    }

    return false;
}

NFINT64 NFRecordManager::GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->GetInt(nRow, nCol);
    }

    return 0;
}

double NFRecordManager::GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->GetFloat(nRow, nCol);
    }

    return 0.0;
}

const std::string& NFRecordManager::GetRecordString(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->GetString(nRow, nCol);
    }

    return NULL_STR;
}

const NFGUID& NFRecordManager::GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol)
{
    NF_SHARE_PTR<NFIRecord> pRecord = GetElement(strRecordName);
    if (pRecord)
    {
        return pRecord->GetObject(nRow, nCol);
    }

    return NULL_OBJECT;
}