///--------------------------------------------------------------------
/// �ļ���:		NFSecurityModule.h
/// ��  ��:		�ӽ��ܽӿ�
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_SECURITY_MODULE_H
#define NF_SECURITY_MODULE_H

#include "NFComm/NFPluginModule/NFISecurityModule.h"

class NFSecurityModule
    : public NFISecurityModule
{
public:
	NFSecurityModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

	//when a user login NF system, as a security module, you need to grant user an key.
	virtual const std::string GetSecurityKey(const std::string& strAcount);

	// ����
	std::string EncodeMsg(const char* data, std::string key);

	// ����
	const char* DecodeMsg(const char* data, std::string key, int iDataLenth, int& iLength);

private:
	std::string base64_encode(unsigned char const*, unsigned int len);
	std::string base64_decode(std::string const& s);

	void xxtea_uint_encrypt(xxtea_uint* v, xxtea_uint len, xxtea_uint* k);
	void xxtea_uint_decrypt(xxtea_uint* v, xxtea_uint len, xxtea_uint* k);

	unsigned char* fix_key_length(unsigned char* key, xxtea_uint key_len); //������Ȳ�������������;
	xxtea_uint* xxtea_to_uint_array(unsigned char* data, xxtea_uint len, int include_length, xxtea_uint* ret_len);//������ת�������������������;
	unsigned char* xxtea_to_byte_array(xxtea_uint* data, xxtea_uint len, int include_length, xxtea_uint* ret_len); //��������ת�����޷����ֽ���;
	unsigned char* do_xxtea_encrypt(unsigned char* data, xxtea_uint len, unsigned char* key, xxtea_uint* ret_len); //�ڶ�����ʼ�������Ѿ���λ�����������ݻ�Ϊ�ַ���;
	unsigned char* do_xxtea_decrypt(unsigned char* data, xxtea_uint len, unsigned char* key, xxtea_uint* ret_len);
	unsigned char* xxtea_encrypt(unsigned char* data, xxtea_uint data_len, unsigned char* key, xxtea_uint key_len, xxtea_uint* ret_length);
	unsigned char* xxtea_decrypt(unsigned char* data, xxtea_uint data_len, unsigned char* key, xxtea_uint key_len, xxtea_uint* ret_length);

	int util_range_random_int(int max);
};
#endif
