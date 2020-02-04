///--------------------------------------------------------------------
/// 文件名:		NFGUID.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_IDENTID_H
#define NF_IDENTID_H
#include "NFPlatform.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#pragma warning(disable:4996)

class NFGUID
{
public:
	NFINT64 nHead64;
    NFINT64 nData64;

    NFGUID()
    {
		nHead64 = 0;
        nData64 = 0;
    }

    NFGUID(NFINT64 nHeadData, NFINT64 nData)
    {
        nHead64 = nHeadData;
        nData64 = nData;
    }

    NFGUID(const NFGUID& xData)
    {
        nHead64 = xData.nHead64;
        nData64 = xData.nData64;
    }
  
    NFGUID(const std::string& strID)
    {
        if (!FromString(strID))
        {
			NFGUID();
        }
    }

    NFGUID& operator=(const NFGUID& xData)
    {
        nHead64 = xData.nHead64;
        nData64 = xData.nData64;

        return *this;
    }
  
    NFGUID& operator=(const std::string& strID)
    {
        FromString(strID);

        return *this;
    }

    const NFINT64 GetData() const
    {
        return nData64;
    }

    const NFINT64 GetHead() const
    {
        return nHead64;
    }

    void SetData(const NFINT64 nData)
    {
        nData64 = nData;
    }

    void SetHead(const NFINT64 nData)
    {
        nHead64 = nData;
    }

    bool IsNull() const
    {
        return 0 == nData64 && 0 == nHead64;
    }

    bool operator == (const NFGUID& id) const
    {
        return this->nData64 == id.nData64 && this->nHead64 == id.nHead64;
    }

    bool operator != (const NFGUID& id) const
    {
        return this->nData64 != id.nData64 || this->nHead64 != id.nHead64;
    }

    bool operator < (const NFGUID& id) const
    {
        if (this->nHead64 == id.nHead64)
        {
            return this->nData64 < id.nData64;
        }

        return this->nHead64 < id.nHead64;
    }

    const std::string ToString() const
    {
		char szBuffer[33] = { 0 };
		_snprintf(szBuffer, sizeof(szBuffer), "%016I64X%016I64X", nHead64, nData64);
		szBuffer[32] = 0;

		return szBuffer;
    }

    bool FromString(const std::string& strID)
    {
        if (strID.length() != 32)
        {
			return false;
        }

		return sscanf(strID.c_str(), "%016I64X%016I64X", &nHead64, &nData64) == 2;
    }
};

// std::hash 的自定义特化能注入 namespace std
namespace std
{
	template<> struct hash<NFGUID>
	{
		typedef NFGUID argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& id) const
		{
			result_type const h1(std::hash<int64_t>{}(id.nHead64));
			result_type const h2(std::hash<int64_t>{}(id.nData64));
			return h1 ^ (h2 << 1); // 或使用 boost::hash_combine
		}
	};
}

#endif
