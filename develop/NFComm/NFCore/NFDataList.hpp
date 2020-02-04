///--------------------------------------------------------------------
/// 文件名:		NFDataList.cpp
/// 内  容:		NF数据列表
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_DATALIST_H
#define NF_DATALIST_H

#include <cstdarg>
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <assert.h>
#include <typeinfo>
#include <iomanip>
#include <functional>
#include <iostream>
#include <fstream>
#include <memory>
#include "NFMemoryCounter.hpp"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFUtils/NFTool.hpp"

#ifdef _MSC_VER
#if _MSC_VER <= 1800
#include "common/variant.h"
#else
#include "common/variant.hpp"
#endif
#else
#include "common/variant.hpp"
#endif

enum NFDATA_TYPE
{
	TDATA_UNKNOWN,
	TDATA_INT,
	TDATA_FLOAT,
	TDATA_STRING,
	TDATA_OBJECT,
	TDATA_MAX,

	// 附加的数据类型
	TDATA_BYTES = TDATA_STRING | (TDATA_STRING << sizeof(short) * CHAR_BIT),
};

const static std::string NULL_STR = "";
const static NFGUID NULL_OBJECT;
const static double NULL_FLOAT = 0.0;
const static NFINT64 NULL_INT = 0;

struct NFData
{
public:
	NFData()
	{
		nType = TDATA_UNKNOWN;
	}

	NFData(NFDATA_TYPE eType)
	{
		nType = eType;
	}

	NFData(const NFData& value)
	{
		*this = value;
	}
	NFData(NFData&& value) noexcept
	{
		*this = std::move(value);
	}

	~NFData()
	{
		nType = TDATA_UNKNOWN;
	}

	NFData& operator=(const NFData& src)
	{
		nType = src.nType;
		variantData = src.variantData;

		return *this;
	}
	NFData& operator=(NFData&& src) noexcept
	{
		nType = src.nType;
		variantData = std::move(src.variantData);
		src.Reset();

		return *this;
	}

	inline bool operator==(const NFData& src) const
	{
		//&& src.variantData == variantData
		if (src.GetType() == GetType())
		{
			switch (GetType())
			{
			case TDATA_INT:
			{
				if (src.GetInt() == GetInt())
				{
					return true;
				}
			}
			break;
			case TDATA_FLOAT:
			{
				if (DoubleEqual(GetFloat(), src.GetFloat()))
				{
					return true;
				}
			}
			break;
			case TDATA_STRING:
			{
				if (src.GetString() == GetString())
				{
					return true;
				}
			}
			break;
			case TDATA_OBJECT:
			{
				if (src.GetObject() == GetObject())
				{
					return true;
				}
			}
			break;
			default:
				break;
			}
		}

		return false;
	}

	inline bool operator!=(const NFData& src) const
	{
		return !(*this == src);
	}

	void Reset()
	{
		variantData = mapbox::util::variant<NFINT64, double, std::string, NFGUID>();
		nType = TDATA_UNKNOWN;
	}

	bool IsNullValue() const
	{
		bool bChanged = false;

		switch (GetType())
		{
		case TDATA_INT:
		{
			if (0 != GetInt())
			{
				bChanged = true;
			}
		}
		break;
		case TDATA_FLOAT:
		{
			double fValue = GetFloat();
			if (fValue > 0.001 || fValue < -0.001)
			{
				bChanged = true;
			}
		}
		break;
		case TDATA_STRING:
		{
			const std::string& strData = GetString();
			if (!strData.empty())
			{
				bChanged = true;
			}
		}
		break;
		case TDATA_OBJECT:
		{
			if (!GetObject().IsNull())
			{
				bChanged = true;
			}
		}
		break;
		default:
			break;
		}

		return !bChanged;
	}

	// 获取原始类型
	NFDATA_TYPE GetType() const
	{
		return (NFDATA_TYPE)short(nType);
	}

	// 获取附加类型
	NFDATA_TYPE GetTypeEx() const
	{
		return nType;
	}

	void SetInt(const NFINT64 var)
	{
		if (GetType() == TDATA_INT)
		{
			variantData = (NFINT64)var;
		}
		else if(TDATA_UNKNOWN == GetType())
		{
			nType = TDATA_INT;
			variantData = (NFINT64)var;
		}
	}

	void SetFloat(const double var)
	{
		if (GetType() == TDATA_FLOAT)
		{
			variantData = (double)var;
		}
		else if (TDATA_UNKNOWN == GetType())
		{
			nType = TDATA_FLOAT;
			variantData = (double)var;
		}
	}

	void SetString(const std::string& var)
	{
		if (GetType() == TDATA_STRING)
		{
			variantData = (std::string)var;
		}
		else if (TDATA_UNKNOWN == GetType())
		{
			nType = TDATA_STRING;
			variantData = (std::string)var;
		}
	}

	void SetString(std::string&& var)
	{
		if (GetType() == TDATA_STRING)
		{
			variantData = std::move(var);
		}
		else if (TDATA_UNKNOWN == GetType())
		{
			nType = TDATA_STRING;
			variantData = std::move(var);
		}
	}

	void SetObject(const NFGUID var)
	{
		if (GetType() == TDATA_OBJECT)
		{
			variantData = (NFGUID)var;
		}
		else if (TDATA_UNKNOWN == GetType())
		{
			nType = TDATA_OBJECT;
			variantData = (NFGUID)var;
		}
	}

	NFINT64 GetInt() const
	{
		if (TDATA_INT == GetType())
		{
			return variantData.get<NFINT64>();
		}

		return NULL_INT;
	}

	double GetFloat() const
	{
		if (TDATA_FLOAT == GetType())
		{
			return variantData.get<double>();
		}

		return NULL_FLOAT;
	}

	const std::string& GetString() const
	{
		if (TDATA_STRING == GetType())
		{
			return variantData.get<std::string>();
		}

		return NULL_STR;
	}

	const NFGUID& GetObject() const
	{
		if (TDATA_OBJECT == GetType())
		{
			return variantData.get<NFGUID>();
		}

		return NULL_OBJECT;
	}

	std::string ToString() const
	{
		std::string strData;

		switch (GetType())
		{
		case TDATA_INT:
			strData = lexical_cast<std::string> (GetInt());
			break;

		case TDATA_FLOAT:
			strData = lexical_cast<std::string> (GetFloat());
			break;

		case TDATA_STRING:
			strData = GetString();
			break;

		case TDATA_OBJECT:
			strData = GetObject().ToString();
			break;

		default:
			strData = NULL_STR;
			break;
		}
		return std::move(strData);
	}

	bool FromString(const std::string& strData)
	{
		try
		{
			switch (GetType())
			{
			case TDATA_INT:
				SetInt(lexical_cast<int64_t> (strData));
				break;

			case TDATA_FLOAT:
				SetFloat(lexical_cast<float> (strData));
				break;

			case TDATA_STRING:
				SetString(strData);
				break;

			case TDATA_OBJECT:
			{
				NFGUID xID;
				xID.FromString(strData);
				SetObject(xID);
			}
			break;

			default:
				break;
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
		
		return false;
	}

private:

	NFDATA_TYPE nType;
	//std::variant
	mapbox::util::variant<NFINT64, double, std::string, NFGUID> variantData;
};
const static NFData NULL_TDATA;

class NFDataList :public NFMemoryCounter<NFDataList>
{
public:
    NFDataList() : NFMemoryCounter(GET_CLASS_NAME(NFDataList))
    {
    }

	virtual ~NFDataList()
	{
		for (auto& it : m_vData)
		{
			delete it;
		}
		m_vData.clear();
		m_nDataUsed = 0;
	}

	NFDataList(const NFDataList& src)
		: NFMemoryCounter(GET_CLASS_NAME(NFDataList))
	{
		*this = src;
	}
	NFDataList(NFDataList&& src) noexcept
		: NFMemoryCounter(GET_CLASS_NAME(NFDataList))
	{
		*this = std::move(src);
	}
	NFDataList& operator=(const NFDataList& src)
	{
		for (auto& it: m_vData)
		{
			it->Reset();
		}
		m_nDataUsed = 0;

		Append(src);

		return *this;
	}
	NFDataList& operator=(NFDataList&& src) noexcept
	{
		m_nDataUsed = src.m_nDataUsed;
		m_vData.swap(src.m_vData);
		src.m_nDataUsed = 0;
		src.m_vData.clear();

		return *this;
	}

	static const NFDataList& Empty()
	{
		static NFDataList data;
		return data;
	}

	virtual std::string ToString() const
	{
		std::stringstream os;
		for (int i = 0; i < GetCount(); ++i)
		{
			os << ToString(i);
			if (i < GetCount() - 1)
			{
				os << "|";
			}
		}

		return std::move(os.str());
	}

	virtual std::string ToString(const int index) const
	{
		std::string strData;

		if (ValidIndex(index))
		{
			const NFDATA_TYPE eType = Type(index);
			switch (eType)
			{
			case TDATA_INT:
				strData = lexical_cast<std::string> (Int(index));
				break;

			case TDATA_FLOAT:
				strData = lexical_cast<std::string> (Float(index));
				break;

			case TDATA_STRING:
				strData = String(index);
				break;

			case TDATA_OBJECT:
				strData = Object(index).ToString();
				break;

			default:
				strData = NULL_STR;
				break;
			}
		}

		return std::move(strData);
	}

	virtual bool ToString(std::string& str, const std::string& strSplit) const
	{
		for (int i = 0; i < GetCount(); ++i)
		{
			std::string strVal = ToString(i);
			str += strVal;
			str += strSplit;
		}

		std::string strTempSplit(strSplit);
		std::string::size_type nPos = str.rfind(strSplit);
		if (nPos == str.length() - strTempSplit.length())
		{
			str = str.substr(0, nPos);
		}

		return true;
	}

public:

	virtual bool Concat(const NFDataList& src)
	{
		Append(src, 0, src.GetCount());
		return true;
	}
    
	virtual bool Append(const NFDataList& src)
	{
		return Append(src, 0, src.GetCount());
	}

	virtual bool Append(const NFDataList& src, const int start, const int count)
	{
		if (start >= src.GetCount())
		{
			return false;
		}

		int end = start + count;

		if (end > src.GetCount())
		{
			return false;
		}

		m_vData.reserve(m_vData.size() + count);
		for (int i = start; i < end; ++i)
		{
			Add(src.GetData(i));
		}

		return true;
	}
    
	virtual void Clear()
	{
		m_nDataUsed = 0;

		for (int i = int(m_vData.size()) - 1; i >= STACK_SIZE; --i)
		{
			delete m_vData[i];
			m_vData.pop_back();
		}

		for (auto& it : m_vData)
		{
			it->Reset();
		}
	}
    
	virtual bool IsEmpty() const
	{
		return (0 == m_nDataUsed);
	}
    
	virtual int GetCount() const
	{
		return m_nDataUsed;
	}
    
	virtual NFDATA_TYPE Type(const int index) const
	{
		if (!ValidIndex(index))
		{
			return TDATA_UNKNOWN;
		}

		return m_vData[index]->GetType();
	}

	virtual NFDATA_TYPE TypeEx(const int index) const
	{
		if (!ValidIndex(index))
		{
			return TDATA_UNKNOWN;
		}

		return m_vData[index]->GetTypeEx();
	}
    
	virtual bool Split(const std::string& str, const std::string& strSplit)
	{
		Clear();

		std::string strData(str);
		if (strData.empty())
		{
			return true;
		}

		std::string temstrSplit(strSplit);
		std::string::size_type pos;
		strData += temstrSplit;
		std::string::size_type size = strData.length();

		for (std::string::size_type i = 0; i < size; i++)
		{
			pos = int(strData.find(temstrSplit, i));
			if (pos < size)
			{
				std::string strSub = strData.substr(i, pos - i);
				Add(strSub.c_str());

				i = pos + temstrSplit.size() - 1;
			}
		}

		return true;
	}
    
	virtual bool Add(const NFData& xData)
	{
		NFData* pData = AddData();
		*pData = xData;

		return true;
	}
	virtual bool Add(NFData&& xData)
	{
		NFData* pData = AddData();
		*pData = std::move(xData);

		return true;
	}

	virtual bool Add(const NFINT64 value)
	{
		NFData* pData = AddData();
		pData->SetInt(value);
		return true;
	}

	virtual bool Add(const double value)
	{
		NFData* pData = AddData();
		pData->SetFloat(value);
		return true;
	}

	virtual bool Add(const std::string& value)
	{
		NFData* pData = AddData();
		pData->SetString(value);
		return true;
	}

	virtual bool Add(std::string&& value)
	{
		NFData* pData = AddData();
		pData->SetString(std::move(value));
		return true;
	}

	virtual bool Add(const NFGUID& value)
	{
		NFData* pData = AddData();
		pData->SetObject(value);
		return true;
	}

	virtual bool Set(const int index, const NFINT64 value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == TDATA_INT)
			{
				data.SetInt(value);
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const double value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == TDATA_FLOAT)
			{
				data.SetFloat(value);
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const std::string& value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == TDATA_STRING)
			{
				data.SetString(value);
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, std::string&& value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == TDATA_STRING)
			{
				data.SetString(std::move(value));
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const NFGUID& value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == TDATA_OBJECT)
			{
				data.SetObject(value);
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const NFData& value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == value.GetType())
			{
				data = value;
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, NFData&& value)
	{
		if (ValidIndex(index))
		{
			auto& data = *m_vData[index];
			if (data.GetType() == value.GetType())
			{
				data = std::move(value);
				return true;
			}
		}

		return false;
	}
    
	virtual const NFData& GetData(const int index) const
	{
		if (ValidIndex(index))
		{
			return *m_vData[index];
		}

		return NULL_TDATA;
	}

	virtual NFINT64 Int(const int index) const
	{
		if (ValidIndex(index))
		{
			return m_vData[index]->GetInt();
		}

		return 0;
	}

	virtual double Float(const int index) const
	{
		if (ValidIndex(index))
		{
			return m_vData[index]->GetFloat();
		}

		return 0.0f;
	}

	virtual const std::string& String(const int index) const
	{
		if (ValidIndex(index))
		{
			return m_vData[index]->GetString();
		}

		return NULL_STR;
	}

	virtual const NFGUID& Object(const int index) const
	{
		if (ValidIndex(index))
		{
			return m_vData[index]->GetObject();
		}

		return NULL_OBJECT;
	}

    bool AddInt(const NFINT64 value)
    {
        return Add(value);
    }
    bool AddFloat(const double value)
    {
        return Add(value);
    }
    bool AddString(const std::string& value)
    {
        return Add(value);
    }
	bool AddString(std::string&& value)
	{
		return Add(std::move(value));
	}
    bool AddStringFromChar(const char* value)
    {
        return Add(value);
    }
    bool AddObject(const NFGUID& value)
    {
        return Add(value);
    }

    bool SetInt(const int index, const NFINT64 value)
    {
        return Set(index, value);
    }
    bool SetFloat(const int index, const double value)
    {
        return Set(index, value);
    }
    bool SetString(const int index, const std::string& value)
    {
        return Set(index, value);
    }
	bool SetString(const int index, std::string&& value)
	{
		return Set(index, std::move(value));
	}
    bool SetObject(const int index, const NFGUID& value)
    {
        return Set(index, value);
    }

    inline bool Compare(const int nPos, const NFDataList& src) const
    {
        if (src.GetCount() > nPos
            && GetCount() > nPos
            && src.Type(nPos) == Type(nPos))
        {
            switch (src.Type(nPos))
            {
                case TDATA_INT:
                    return Int(nPos) == src.Int(nPos);
                    break;

                case TDATA_FLOAT:
                    return DoubleEqual(Float(nPos), src.Float(nPos));
                    break;

                case TDATA_STRING:
                    return String(nPos) == src.String(nPos);
                    break;

                case TDATA_OBJECT:
                    return Object(nPos) == src.Object(nPos);
                    break;

                default:
                    return false;
                    break;
            }
        }

        return false;
    }

    inline bool operator==(const NFDataList& src) const
    {
        if (src.GetCount() == GetCount())
        {
            for (int i = 0; i < GetCount(); i++)
            {
                if (!Compare(i, src))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    inline bool operator!=(const NFDataList& src)
    {
        return !(*this == src);
    }
    inline NFDataList& operator<<(const double value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const char* value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const std::string& value)
    {
        Add(value);
        return *this;
    }
	inline NFDataList& operator<<(std::string&& value)
	{
		Add(std::move(value));
		return *this;
	}
    inline NFDataList& operator<<(const NFINT64& value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const int value)
    {
        Add((NFINT64)value);
        return *this;
    }
    inline NFDataList& operator<<(const NFGUID& value)
    {
        Add(value);
        return *this;
    }
	inline NFDataList& operator<<(const NFData& value)
	{
		Add(value);
		return *this;
	}
	inline NFDataList& operator<<(NFData&& value)
	{
		Add(std::move(value));
		return *this;
	}
    inline NFDataList& operator<<(const NFDataList& value)
    {
        Concat(value);
        return *this;
    }
    enum { STACK_SIZE = 8 };

protected:

	bool ValidIndex(int index) const
	{
		return (index < m_nDataUsed) && (index >= 0);
	}

	NFData* AddData()
	{
		if (m_nDataUsed >= m_vData.size())
		{
			m_vData.push_back(NF_NEW NFData());
		}

		return m_vData[m_nDataUsed++];
	}

protected:
    int m_nDataUsed = 0;
    std::vector< NFData* > m_vData;
};
#define NULL_DATA_LIST NFDataList::Empty()

template<class Type>
const static std::vector<Type> NULL_VECTOR;
template<class FirstType, class SecondType>
const static std::map<FirstType, SecondType> NULL_MAP;
template<class Type>
const static std::set<Type> NULL_SET;
template<class Type>
const static std::list<Type> NULL_LIST;


#endif