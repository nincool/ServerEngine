///--------------------------------------------------------------------
/// 文件名:		WhiteIP.h
/// 内  容:		白名单
/// 说  明:		
/// 创建日期:	2019年11月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _WHITE_IP_H
#define _WHITE_IP_H
#include <vector>

// 白名单IP地址表
class CWhiteIP
{
public:
	CWhiteIP();
	~CWhiteIP();

	// 添加地址范围
	bool AddWhiteIP(const char* beg, const char* end);
	// 清除所有地址
	void ClearWhiteIP();
	// 是否在地址范围内
	bool IsWhiteIP(unsigned int ip_addr);

private:
	// 白名单地址范围
	struct white_addr_t
	{
		unsigned int nBeg;
		unsigned int nEnd;
	};

	std::vector<white_addr_t> m_WhiteIPs;
};

#endif // _WHITE_IP_H
