///--------------------------------------------------------------------
/// �ļ���:		NFIRecord.h
/// ��  ��:		��ӿ�
/// ˵  ��:		
/// ��������:	2019.8.10
/// �޸���:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_RECORD_H
#define NFI_RECORD_H

#include "NFDataList.hpp"
#include "NFComm/NFPluginModule/NFPlatform.h"

struct RECORD_EVENT_DATA
{
	enum RecordOptype
	{
		Add = 0,
		Del,
		Update,
		Cleared,

		UNKNOW,
	};
	RECORD_EVENT_DATA()
	{
		nOpType = UNKNOW;
		nRow = 0;
		nCol = 0;
	}

	RecordOptype nOpType;
	int nRow;
	int nCol;
	std::string strRecordName;
};

typedef std::function<int(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&)> RECORD_EVENT_FUNCTOR;
typedef NF_SHARE_PTR<RECORD_EVENT_FUNCTOR> RECORD_EVENT_FUNCTOR_PTR;

class _NFExport NFIRecord :public NFMemoryCounter<NFIRecord>
{
public:
    NFIRecord() : NFMemoryCounter(GET_CLASS_NAME(NFIRecord))
	{
	}

    typedef std::vector< NF_SHARE_PTR<NFData> > TRECORDVEC;
    typedef TRECORDVEC::const_iterator TRECORDVECCONSTITER;

    virtual ~NFIRecord() {}

	virtual std::string ToString() = 0;

	// �Ƿ�Ϊ��
	virtual bool IsEmpty() const = 0;
	// �������
    virtual int GetCols() const  = 0;
	// ��ʹ�õ��������
	virtual int GetRows() const = 0;
	// ����������
	virtual int GetMaxRows() const = 0;
	// ��ȡ������
    virtual NFDATA_TYPE GetColType(const int nCol) const = 0;
	// ��ȡĬ������
	virtual const NF_SHARE_PTR<const NFDataList> GetColData() const = 0;
   
	/// \breif   AddRow ����������
	/// \param   const int nRow ������(-1ĩβ���)
	/// \param   const NFDataList & var ������(���� NULL_DATA_LIST)
	/// \return  int ������к�
	virtual int AddRow(const int nRow, const NFDataList& var) = 0;
	virtual int AddRow(const int nRow, NFDataList&& var) = 0;

	// �޸�һ������
	virtual bool SetRowValue(const int nRow, const NFDataList& var) = 0;
	virtual bool SetRowValue(const int nRow, NFDataList&& var) = 0;
	// ��ȡһ������
	virtual const NFDataList& GetRowValue(const int nRow) const = 0;

    virtual bool SetInt(const int nRow, const int nCol, const NFINT64 value) = 0;
    virtual bool SetFloat(const int nRow, const int nCol, const double value) = 0;
    virtual bool SetString(const int nRow, const int nCol, const std::string& value) = 0;
	virtual bool SetString(const int nRow, const int nCol, std::string&& value) = 0;
    virtual bool SetObject(const int nRow, const int nCol, const NFGUID& value) = 0;

    virtual NFINT64 GetInt(const int nRow, const int nCol) const = 0;
	virtual double GetFloat(const int nRow, const int nCol) const = 0;
    virtual const std::string& GetString(const int nRow, const int nCol) const = 0;
    virtual const NFGUID& GetObject(const int nRow, const int nCol) const = 0;

	/// \breif   FindRow ���Ҷ���ֵ
	/// \param   const NFDataList & cols ��
	/// \param   const NFDataList & vars ֵ
	virtual int FindRow(const NFDataList& cols, const NFDataList& vars, int nStartRow = 0) const = 0;
    virtual int FindRow(const int nCol, const NFData& var, int nStartRow = 0) const = 0;
    virtual int FindInt(const int nCol, const NFINT64 value, int nStartRow = 0) const = 0;
    virtual int FindFloat(const int nCol, const double value, int nStartRow = 0) const = 0;
	virtual int FindString(const int nCol, const std::string& value, int nStartRow = 0) const = 0;
    virtual int FindObject(const int nCol, const NFGUID& value, int nStartRow = 0) const = 0;

    virtual bool Remove(const int nRow) = 0;

    virtual bool Clear() = 0;

    virtual void AddRecordHook(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;

    virtual const bool GetSave() const = 0;
    virtual const bool GetPublic() const = 0;
    virtual const bool GetPrivate() const = 0;
    virtual const std::string& GetName() const = 0;
	virtual const int GetTCallBackCount() const = 0;
};

#endif
