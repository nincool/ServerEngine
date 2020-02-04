
#include <string.h>

#include "CryptoServer.h"
#include "../AutoMem.h"
#include "../Base64.h"
#include "../rijndael.h"

bool AES_Decrypt(std::string& cipherText)
{
	int cipherTextLen = strlen(cipherText.c_str());        // Plain text length

	//×ª³Ébase64
	TAutoMem<unsigned char, 256> auto_buf(cipherTextLen * 2 + 1);
	memset(auto_buf.GetBuffer(), 0, cipherTextLen * 2 + 1);
	unsigned char* output = auto_buf.GetBuffer();
	int size = base64_decode(output, cipherText.c_str(), cipherTextLen);

	TAutoMem<char, 256> temp(size + 16);
	memset(temp.GetBuffer(), 0, size + 16);

	Rijndael rin;
	rin.init(Rijndael::CBC, Rijndael::Decrypt, AES_KEY, Rijndael::Key32Bytes);

	int len = rin.padDecrypt((const UINT8*)output, size, (UINT8*)temp.GetBuffer());
	if (len >= 0)
	{
		cipherText = (char*)temp.GetBuffer();
	}
	else
	{
		return false;
	}

	return true;
}

std::string Decrypt(const char* cipherText, const char* key)
{
	if (key)
	{
		int len = strlen(key);
		int key_len = sizeof(AES_KEY) / sizeof(UINT8);
		if (len > key_len - 1)
		{
			len = key_len - 1;
		}
		memcpy(AES_KEY, key, len);
	}

	std::string plainText = cipherText;

	try
	{

		if (!AES_Decrypt(plainText))
			return "";
	}
	catch (...)
	{
		return "";
	}

	return plainText;
}
