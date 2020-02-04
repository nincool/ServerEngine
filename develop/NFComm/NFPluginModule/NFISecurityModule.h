///--------------------------------------------------------------------
/// �ļ���:		NFISecurityModule.h
/// ��  ��:		���ܽӿ�
/// ˵  ��:		
/// ��������:	2019��8��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_SECURITY_MODULE_H
#define NFI_SECURITY_MODULE_H

#include "NFIModule.h"

#define NO_ENCODE "return_false_false"

#define XXTEA_MX (z >> 5 ^ y << 2) + (y >> 3 ^ z << 4) ^ (sum ^ y) + (k[p & 3 ^ e] ^ z) 
#define XXTEA_DELTA 0x9E3779B9 
#define UTIL_RAND_MAX RAND_MAX
#define LOGINCRYPTO "kOHUSpTLX"

typedef unsigned __int32 xxtea_uint;

class NFISecurityModule : public NFIModule
{

public:
	//when a user login NF system, as a security module, you need to grant user an key.
	virtual const std::string GetSecurityKey(const std::string& strAcount) = 0;

	//���� 
	virtual std::string EncodeMsg(const char* data, std::string key) = 0;

	//����
	virtual const char* DecodeMsg(const char* data, std::string key, int iDataLenth, int& iLength) = 0;

};

#endif