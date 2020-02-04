#include <string>
#include "Base64.h"

// BASE64块解码
void base64_decode_block(unsigned char* dst, const char* src)
{
	unsigned int x = 0;

	for (int i = 0; i < 4; ++i)
	{
		if (src[i] >= 'A' && src[i] <= 'Z')
		{
			x = (x << 6) + (unsigned int)(src[i] - 'A' + 0);
		}
		else if (src[i] >= 'a' && src[i] <= 'z')
		{
			x = (x << 6) + (unsigned int)(src[i] - 'a' + 26);
		}
		else if (src[i] >= '0' && src[i] <= '9')
		{
			x = (x << 6) + (unsigned int)(src[i] - '0' + 52);
		}
		else if (src[i] == '+')
		{
			x = (x << 6) + 62;
		}
		else if (src[i] == '/')
		{
			x = (x << 6) + 63;
		}
		else if (src[i] == '=')
		{
			x = (x << 6);
		}
	}

	dst[2] = (unsigned char)(x & 255);
	x >>= 8;
	dst[1] = (unsigned char)(x & 255);
	x >>= 8;
	dst[0] = (unsigned char)(x & 255);
}

// BASE64解码
int base64_decode(unsigned char* dst, const char* src, int inlen)
{
	int length = 0;

	while ((length < inlen) && (src[length] != '='))
	{
		length++;
	}

	int equal_num = 0;

	while (((length + equal_num) < inlen) && (src[length + equal_num] == '='))
	{
		equal_num++;
	}

	int block_num = (length + equal_num) / 4;

	for (int i = 0; i < (block_num - 1); ++i)
	{
		base64_decode_block(dst, src);

		dst += 3;
		src += 4;
	}

	unsigned char last_block[3];

	base64_decode_block(last_block, src);

	for (int k = 0; k < (3 - equal_num); ++k)
	{
		dst[k] = last_block[k];
	}

	return (block_num * 3) - equal_num;
}

// BASE64编码
int base64_encode(const void* inbuf, int inlen, char* outbuf)
{
	static char table64[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	char* indata = (char*)inbuf;

	if (0 == inlen)
	{
		inlen = (int)strlen(indata);
	}

	char* output = outbuf;

	while (inlen > 0)
	{
		unsigned char ibuf[3];
		unsigned char obuf[4];
		int input_num = 0;

		for (int i = 0; i < 3; ++i)
		{
			if (inlen > 0)
			{
				input_num++;
				ibuf[i] = *indata;
				indata++;
				inlen--;
			}
			else
			{
				ibuf[i] = 0;
			}
		}

		obuf[0] = (ibuf[0] & 0xFC) >> 2;
		obuf[1] = ((ibuf[0] & 0x03) << 4) | ((ibuf[1] & 0xF0) >> 4);
		obuf[2] = ((ibuf[1] & 0x0F) << 2) | ((ibuf[2] & 0xC0) >> 6);
		obuf[3] = ibuf[2] & 0x3F;

		switch (input_num)
		{
		case 1:
			output[0] = table64[obuf[0]];
			output[1] = table64[obuf[1]];
			output[2] = '=';
			output[3] = '=';
			break;
		case 2:
			output[0] = table64[obuf[0]];
			output[1] = table64[obuf[1]];
			output[2] = table64[obuf[2]];
			output[3] = '=';
			break;
		default:
			output[0] = table64[obuf[0]];
			output[1] = table64[obuf[1]];
			output[2] = table64[obuf[2]];
			output[3] = table64[obuf[3]];
			break;
		}

		output += 4;
	}

	*output = 0;

	return (int)(output - outbuf);
}

// 十六进制转成字符
unsigned int hex_to_char(const char* s, size_t start, size_t count)
{
	unsigned int v = 0;

	for (size_t i = 0; i < count; ++i)
	{
		v <<= 4;

		char c = s[start + i];

		if ((c >= '0') && (c <= '9'))
		{
			v += (unsigned int)(c - '0');
		}
		else if ((c >= 'A') && (c <= 'F'))
		{
			v += (unsigned int)(c - 'A' + 10);
		}
		else if ((c >= 'a') && (c <= 'f'))
		{
			v += (unsigned int)(c - 'a' + 10);
		}
		else
		{
			return 0;
		}
	}

	return v;
}

// 字符转成十六进制表示
void char_to_hex(unsigned int ch, int bytes, char* buf)
{
	static char to_hex[] = "0123456789ABCDEF";

	for (int i = (bytes - 1); i >= 0; --i)
	{
		buf[bytes - 1 - i] = to_hex[(ch >> (i * 4)) & 0xF];
	}
}