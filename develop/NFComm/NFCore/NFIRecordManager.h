///--------------------------------------------------------------------
/// �ļ���:		NFIRecord.h
/// ��  ��:		�����ӿ�
/// ˵  ��:		
/// ��������:	2019.8.10
/// �޸���:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_RECORDMANAGER_H
#define NFI_RECORDMANAGER_H

#include "NFIRecord.h"
#include "NFMap.hpp"
#include "NFComm/NFPluginModule/NFPlatform.h"

class _NFExport NFIRecordManager : public NFMapEx<std::string, NFIRecord>
{
public:
    virtual ~NFIRecordManager() {}

    virtual NF_SHARE_PTR<NFIRecord> AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<const NFDataList>& TData, const int nRows) = 0;
    virtual const NFGUID& Self() = 0;
	virtual std::string ToString() = 0;

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 nValue) = 0;
    virtual bool SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue) = 0;
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue) = 0;
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& obj) = 0;

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual double GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol) = 0;
    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol) = 0;

};

#endif
