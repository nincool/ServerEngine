///--------------------------------------------------------------------
/// 文件名:		NFRecordManager.h
/// 内  容:		表管理
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_RECORDMANAGER_H
#define NF_RECORDMANAGER_H

#include <map>
#include <list>
#include <string>
#include <vector>
#include "NFRecord.h"
#include "NFIRecordManager.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

class _NFExport NFRecordManager : public NFIRecordManager
{
public:
    NFRecordManager(const NFGUID& self)
    {
        mSelf = self;
    }

    virtual ~NFRecordManager();

    virtual NF_SHARE_PTR<NFIRecord> AddRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<const NFDataList>& TData, const int nRows);

    virtual const NFGUID& Self();

	virtual std::string ToString();

    virtual bool SetRecordInt(const std::string& strRecordName, const int nRow, const int nCol, const NFINT64 nValue);
    virtual bool SetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol, const double dwValue);
    virtual bool SetRecordString(const std::string& strRecordName, const int nRow, const int nCol, const std::string& strValue);
    virtual bool SetRecordObject(const std::string& strRecordName, const int nRow, const int nCol, const NFGUID& obj);

    virtual NFINT64 GetRecordInt(const std::string& strRecordName, const int nRow, const int nCol);
    virtual double GetRecordFloat(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const std::string& GetRecordString(const std::string& strRecordName, const int nRow, const int nCol);
    virtual const NFGUID& GetRecordObject(const std::string& strRecordName, const int nRow, const int nCol);
private:
    NFGUID mSelf;

};

#endif
