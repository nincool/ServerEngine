#include "Crypto.h"
#include <time.h>
#include <stdio.h>
#include <vcruntime_string.h>
#include "ProxyServer.h"
#include "NFComm/NFUtils/QLOG.h"
#include "NFComm/NFUtils/md5.h"
#include "NFComm/NFUtils/d3des.h"

// 解密授权文件
void __cdecl lookup_thread(void* lpParameter)
{
	CProxyServer* pthis = (CProxyServer*)lpParameter;

	std::string strPath = pthis->GetPluginManager()->GetConfigPath() + 
		"Lua/" + pthis->GetLicense() + ".lic";
	FILE* fp = fopen(strPath.c_str(), "rb");
	if (NULL == fp)
	{
		QLOG_ERROR << __FUNCTION__ << " file not exist";
		return;
	}

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// 是否内部使用
	bool is_internal = (file_size == 256);
	CRYPTO_INFO	data;

	memset(&data, 0, sizeof(data));
	if (is_internal)
	{
		if (fread(&data, sizeof(char), 256, fp) != 256)
		{
			QLOG_ERROR_S << "internal license file error" << pthis->GetLicense();
			fclose(fp);
			return;
		}
	}
	else
	{
		if (fread(&data, sizeof(data), 1, fp) != 1)
		{
			QLOG_ERROR_S << "license file error" << pthis->GetLicense();
			fclose(fp);
			return;
		}
	}

	fclose(fp);

	const std::string& strOutIP = pthis->GetTypeIP(NET_TYPE_OUTER);
	if (strOutIP != pthis->GetLicense())
	{
		QLOG_ERROR_S << "network address error" << pthis->GetLicense()
			<< " addr:" << strOutIP;
		return;
	}

	char key[32];

	memset(key, 0xCC, sizeof(key));
	key[0] = STATIC_KEY0_VALUE;
	key[1] = STATIC_KEY1_VALUE;
	key[2] = STATIC_KEY2_VALUE;
	key[3] = STATIC_KEY3_VALUE;
	key[4] = STATIC_KEY4_VALUE;
	key[5] = STATIC_KEY5_VALUE;
	key[6] = STATIC_KEY6_VALUE;
	key[7] = STATIC_KEY7_VALUE;
	key[8] = STATIC_KEY8_VALUE;
	key[9] = STATIC_KEY9_VALUE;
	key[10] = STATIC_KEY10_VALUE;
	key[11] = STATIC_KEY11_VALUE;
	key[12] = STATIC_KEY12_VALUE;
	key[13] = STATIC_KEY13_VALUE;
	key[14] = STATIC_KEY14_VALUE;
	key[15] = STATIC_KEY15_VALUE;

	CopyString(key + 16, 16, strOutIP.c_str());

	md5 alg;
	alg.Update((unsigned char*)key, sizeof(key));
	alg.Finalize();

	unsigned char* res_key = alg.Digest();

	// 有效期加密的key
	des2key(res_key, DE1);
	unsigned char* source = (unsigned char*)& data.time;
	D2des(source, source);

	unsigned int* pTemp = (unsigned int*)& key[8];
	*pTemp = (unsigned int)data.nStartTime;
	pTemp = (unsigned int*)& key[12];
	*pTemp = (unsigned int)data.nEndTime;

	alg.Update((unsigned char*)key, sizeof(key));
	alg.Finalize();

	res_key = alg.Digest();

	// 数据加密的key
	des2key(res_key, DE1);

	source = (unsigned char*)& data;
	for (size_t i = 0; i < 16; ++i)
	{
		D2des(source, source);
		source += 16;
	}

	unsigned int check_sum = 0;
	for (size_t k = 0; k < sizeof(data.data); ++k)
	{
		check_sum += data.data[k];
	}

	if (check_sum != data.nCheckSum)
	{
		return;
	}

	//有效期验证
	pthis->SetLicStartTime(data.nStartTime);
	pthis->SetLicEndTime(data.nEndTime);

	if (time(NULL) < data.nStartTime)
	{
		time_t time_value = (time_t)data.nStartTime;
		tm* pTm = localtime(&time_value);
		QLOG_ERROR_S << "license too young! date start:" << pTm->tm_year + 1900 << "-" <<
			pTm->tm_mon + 1 << "-" << pTm->tm_mday << " " <<
			pTm->tm_hour << ":" << pTm->tm_min << ":" << pTm->tm_sec;
		return;
	}

	if (time(NULL) > data.nEndTime)
	{
		time_t time_value = (time_t)data.nEndTime;
		tm* pTm = localtime(&time_value);
		QLOG_ERROR_S << "license too old! date end:" << pTm->tm_year + 1900 << "-" <<
			pTm->tm_mon + 1 << "-" << pTm->tm_mday << " " <<
			pTm->tm_hour << ":" << pTm->tm_min << ":" << pTm->tm_sec;
		return;
	}

	pthis->SetLicVaild(true);
}