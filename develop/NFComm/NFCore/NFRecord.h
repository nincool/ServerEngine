///--------------------------------------------------------------------
/// 文件名:		NFRecord.h
/// 内  容:		表
/// 说  明:		
/// 创建日期:	
/// 修改人:		
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_RECORD_H
#define NF_RECORD_H

#include <vector>
#include "NFIRecord.h"
#include "NFDataList.hpp"
#include "NFMapEx.hpp"
#include "NFComm/NFPluginModule/NFPlatform.h"

class _NFExport NFRecord : public NFIRecord
{
private:
	NFRecord() {};
	NFRecord(const NFRecord&) = delete;
	NFRecord& operator=(const NFRecord&) = delete;

public:
    NFRecord(const NFGUID& self, const std::string& strRecordName, const NF_SHARE_PTR<const NFDataList>& valueList, const int nMaxRow);
    virtual ~NFRecord();

	virtual std::string ToString();

	virtual bool IsEmpty() const;

    virtual int GetCols() const;
    virtual int GetRows() const;
	virtual int GetMaxRows() const;

    virtual NFDATA_TYPE GetColType(const int nCol) const;
	virtual const NF_SHARE_PTR<const NFDataList> GetColData() const;
    
	virtual int AddRow(const int nRow, const NFDataList& var);
	virtual int AddRow(const int nRow, NFDataList&& var);
	virtual bool SetRowValue(const int nRow, const NFDataList& var);
	virtual bool SetRowValue(const int nRow, NFDataList&& var);
	virtual const NFDataList& GetRowValue(const int nRow) const;

    virtual bool SetInt(const int nRow, const int nCol, const NFINT64 value);
    virtual bool SetFloat(const int nRow, const int nCol, const double value);
    virtual bool SetString(const int nRow, const int nCol, const std::string& value);
	virtual bool SetString(const int nRow, const int nCol, std::string&& value);
    virtual bool SetObject(const int nRow, const int nCol, const NFGUID& value);

    virtual NFINT64 GetInt(const int nRow, const int nCol) const;
    virtual double GetFloat(const int nRow, const int nCol) const;
    virtual const std::string& GetString(const int nRow, const int nCol) const;
    virtual const NFGUID& GetObject(const int nRow, const int nCol) const;

    virtual int FindRow(const int nCol, const NFData& var, int nStartRow = 0) const;
	virtual int FindRow(const NFDataList& cols, const NFDataList& vars, int nStartRow = 0) const;
    virtual int FindInt(const int nCol, const NFINT64 value, int nStartRow = 0) const;
    virtual int FindFloat(const int nCol, const double value, int nStartRow = 0) const;
	virtual int FindString(const int nCol, const std::string& value, int nStartRow = 0) const;
    virtual int FindObject(const int nCol, const NFGUID& value, int nStartRow = 0) const;

    virtual bool Remove(const int nRow);
    virtual bool Clear();

    virtual void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb);

    virtual const bool GetSave() const;

    virtual const bool GetPublic() const;

    virtual const bool GetPrivate() const;

    virtual const std::string& GetName() const;

	virtual const int GetTCallBackCount() const;

    virtual void SetSave(const bool bSave);

    virtual void SetPublic(const bool bPublic);

    virtual void SetPrivate(const bool bPrivate);

	bool IsBytes(int nCol) const;
	bool PrefillRow(int nRow);

protected:
	const NFGUID& Self() const;
	void SetName(const std::string& strName);

    bool ValidPos(int nRow, int nCol) const;
    bool ValidRow(int nRow) const;
    bool ValidCol(int nCol) const;

    void OnEventHandler(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar);

	NFDataList* AddRowData();
	bool FreeRowData(NFDataList* data);

	template<class T>
	int InnerAddRow(const int nRow, T&& var);
	template<class T>
	bool InnerSetRowValue(const int nRow, T&& var);
	template<class T>
	bool InnerSetString(const int nRow, const int nCol, T&& value);

protected:

	std::string m_strRecName = "";
	int m_nMaxRow = 0;

	const NF_SHARE_PTR<const NFDataList> m_pColData;
	std::vector<NFDataList*> m_vRecData;
	std::list<NFDataList*> m_listPool;

    NFGUID mSelf;
    bool mbSave = false;
    bool mbPublic = false;
    bool mbPrivate = false;

    typedef std::vector<RECORD_EVENT_FUNCTOR_PTR> TRECORDCALLBACKEX;
    TRECORDCALLBACKEX mtRecordCallback;
};

#endif
