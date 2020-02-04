///--------------------------------------------------------------------
/// �ļ���:		WhiteIP.h
/// ��  ��:		������
/// ˵  ��:		
/// ��������:	2019��11��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _WHITE_IP_H
#define _WHITE_IP_H
#include <vector>

// ������IP��ַ��
class CWhiteIP
{
public:
	CWhiteIP();
	~CWhiteIP();

	// ��ӵ�ַ��Χ
	bool AddWhiteIP(const char* beg, const char* end);
	// ������е�ַ
	void ClearWhiteIP();
	// �Ƿ��ڵ�ַ��Χ��
	bool IsWhiteIP(unsigned int ip_addr);

private:
	// ��������ַ��Χ
	struct white_addr_t
	{
		unsigned int nBeg;
		unsigned int nEnd;
	};

	std::vector<white_addr_t> m_WhiteIPs;
};

#endif // _WHITE_IP_H
