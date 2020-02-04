///--------------------------------------------------------------------
/// 文件名:		WhiteIP.cpp
/// 内  容:		白名单
/// 说  明:		
/// 创建日期:	2019年11月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "WhiteIP.h"
#include "NFComm/NFUtils/NFTool.hpp"

// CWhiteIP

CWhiteIP::CWhiteIP()
{
}

CWhiteIP::~CWhiteIP()
{
}

bool CWhiteIP::AddWhiteIP(const char* beg, const char* end)
{
	Assert(beg != NULL);
	Assert(end != NULL);

	unsigned int beg_addr = reverse_ip(convert_ip(beg));
	if (0 == beg_addr)
	{
		return false;
	}
	
	unsigned int end_addr = reverse_ip(convert_ip(end));
	if (0 == end_addr)
	{
		return false;
	}

	if (beg_addr > end_addr)
	{
		return false;
	}
	
	white_addr_t data;
	data.nBeg = beg_addr;
	data.nEnd = end_addr;
	
	m_WhiteIPs.push_back(data);

	return true;
}

void CWhiteIP::ClearWhiteIP()
{
	m_WhiteIPs.clear();
}

bool CWhiteIP::IsWhiteIP(unsigned int ip_addr)
{
	size_t size = m_WhiteIPs.size();
	if (0 == size)
	{
		return false;
	}

	unsigned int rip = reverse_ip(ip_addr);
	for (size_t i = 0; i < size; ++i)
	{
		if ((rip >= m_WhiteIPs[i].nBeg) && (rip <= m_WhiteIPs[i].nEnd))
		{
			return true;
		}
	}

	return false;
}

