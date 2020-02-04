///--------------------------------------------------------------------
/// 文件名:		NFSecurityModule.h
/// 内  容:		加解密接口
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
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

	// 加密
	std::string EncodeMsg(const char* data, std::string key);

	// 解密
	const char* DecodeMsg(const char* data, std::string key, int iDataLenth, int& iLength);

private:
	std::string base64_encode(unsigned char const*, unsigned int len);
	std::string base64_decode(std::string const& s);

	void xxtea_uint_encrypt(xxtea_uint* v, xxtea_uint len, xxtea_uint* k);
	void xxtea_uint_decrypt(xxtea_uint* v, xxtea_uint len, xxtea_uint* k);

	unsigned char* fix_key_length(unsigned char* key, xxtea_uint key_len); //如果长度不够，则进行填充;
	xxtea_uint* xxtea_to_uint_array(unsigned char* data, xxtea_uint len, int include_length, xxtea_uint* ret_len);//将数据转换成整型流，方便加密;
	unsigned char* xxtea_to_byte_array(xxtea_uint* data, xxtea_uint len, int include_length, xxtea_uint* ret_len); //将整型流转换成无符号字节流;
	unsigned char* do_xxtea_encrypt(unsigned char* data, xxtea_uint len, unsigned char* key, xxtea_uint* ret_len); //第二级初始操作（已经补位），这里数据还为字符串;
	unsigned char* do_xxtea_decrypt(unsigned char* data, xxtea_uint len, unsigned char* key, xxtea_uint* ret_len);
	unsigned char* xxtea_encrypt(unsigned char* data, xxtea_uint data_len, unsigned char* key, xxtea_uint key_len, xxtea_uint* ret_length);
	unsigned char* xxtea_decrypt(unsigned char* data, xxtea_uint data_len, unsigned char* key, xxtea_uint key_len, xxtea_uint* ret_length);

	int util_range_random_int(int max);
};
#endif
