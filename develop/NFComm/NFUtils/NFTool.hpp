///--------------------------------------------------------------------
/// 文件名:		NFTool.hpp
/// 内  容:		NF工具函数
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_TOOL_H
#define NF_TOOL_H
#include <stdarg.h>
#include <stdio.h>
#include <objbase.h>
#include "NFComm/NFPublic/Debug.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFPluginModule/NFGUID.h"

#define SafeSprintf Port_SafeSprintf

// 取哈希值，忽略大小写
inline unsigned int GetHashValue(const char* name)
{
	static unsigned char convert_to_lower[256] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
		0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
		0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
		0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
		0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
		0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
	};

	Assert(name != NULL);

	unsigned int h = 0;

	for (; *name; name++)
	{
		h = h * 131 + convert_to_lower[(unsigned char)(*name)];
	}

	return h;
}

// 取哈希值
inline unsigned int GetHashValueCase(const char* name)
{
	Assert(name != NULL);

	unsigned int h = 0;

	for (; *name; name++)
	{
		h = h * 131 + *name;
	}

	return h;
}

// 取哈希值，忽略大小写
inline unsigned int GetHashValueW(const wchar_t* name)
{
	static unsigned short convert_to_lower[256] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
		0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
		0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
		0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
		0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
		0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
		0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
		0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
		0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
		0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
		0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
		0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
		0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7,
		0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
		0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
		0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
		0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
		0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
		0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
		0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
		0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
		0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
	};

	Assert(name != NULL);

	unsigned int h = 0;

	for (; *name; name++)
	{
		unsigned int val = (unsigned int)(*name);

		if (val < 256)
		{
			h = h * 131 + convert_to_lower[val];
		}
		else
		{
			h = h * 131 + val;
		}
	}

	return h;
}

// 取哈希值
inline unsigned int GetHashValueCaseW(const wchar_t* name)
{
	Assert(name != NULL);

	unsigned int h = 0;

	for (; *name; name++)
	{
		h = h * 131 + *name;
	}

	return h;
}

// 组织字符串
inline int Port_SafeSprintf(char* buf, size_t byte_size, const char* info, ...)
{
	if (byte_size <= 1)
	{
		return 0;
	}

	va_list args;

	va_start(args, info);

	size_t buf_size = byte_size - 1;

	int res = _vsnprintf(buf, buf_size, info, args);

	va_end(args);

	if ((size_t)res >= buf_size)
	{
		buf[buf_size] = 0;
		return (int)byte_size;
	}

	return res;
}

// 创建唯一标识符
inline bool Port_CreateGuid(NFGUID& id)
{
	return CoCreateGuid((_GUID*)& id) == S_OK;
}

// 创建唯一标识符
inline const char* Port_CreateGuid(char* buffer, size_t size)
{
	_GUID id;

	if (CoCreateGuid(&id) != S_OK)
	{
		return NULL;
	}

	_snprintf(buffer, size - 1,
		"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
		id.Data1, id.Data2, id.Data3,
		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
	buffer[size - 1] = 0;

	return buffer;
}

// 字符串是否为空
inline bool StringEmpty(const char* s)
{
	return (s[0] == 0);
}

// 宽字符串是否为空
inline bool WideStrEmpty(const wchar_t* s)
{
	return (s[0] == 0);
}

// 拷贝字符串
inline void CopyString(char* buf, size_t byte_size, const char* str)
{
	const size_t SIZE1 = strlen(str) + 1;

	if (SIZE1 <= byte_size)
	{
		memcpy(buf, str, SIZE1);
	}
	else
	{
		memcpy(buf, str, byte_size - 1);
		buf[byte_size - 1] = 0;
	}
}

// 拷贝宽字符串
inline void CopyWideStr(wchar_t* buf, size_t byte_size, const wchar_t* str)
{
	const size_t SIZE1 = (wcslen(str) + 1) * sizeof(wchar_t);

	if (SIZE1 <= byte_size)
	{
		memcpy(buf, str, SIZE1);
	}
	else
	{
		const size_t SIZE2 = byte_size / sizeof(wchar_t) - 1;
		memcpy(buf, str, SIZE2 * sizeof(wchar_t));
		buf[SIZE2] = 0;
	}
}

// 消除空格
inline size_t TrimStringLen(const char* str, size_t len, char* buf, size_t maxlen)
{
	const char* beg = str;
	const char* end = str + len - 1;

	for (; beg <= end; beg++)
	{
		if ((*beg != ' ') && (*beg != '\t'))
		{
			break;
		}
	}

	for (; end >= beg; end--)
	{
		if ((*end != ' ') && (*end != '\t'))
		{
			break;
		}
	}

	size_t size = end - beg + 1;
	if (size >= maxlen)
	{
		size = maxlen - 1;
	}

	memcpy(buf, beg, size);

	buf[size] = 0;

	return size;
}

// 消除空格
inline size_t TrimString(const char* str, char* buf, size_t maxlen)
{
	return TrimStringLen(str, strlen(str), buf, maxlen);
}

// 消除空格
inline size_t TrimWideStrLen(const wchar_t* str, size_t len, wchar_t* buf, size_t maxlen)
{
	const wchar_t* beg = str;
	const wchar_t* end = str + len - 1;

	for (; beg <= end; beg++)
	{
		if ((*beg != L' ') && (*beg != L'\t'))
		{
			break;
		}
	}

	for (; end >= beg; end--)
	{
		if ((*end != L' ') && (*end != L'\t'))
		{
			break;
		}
	}

	size_t size = end - beg + 1;
	const size_t MAX_LEN = maxlen / sizeof(wchar_t);
	if (size >= MAX_LEN)
	{
		size = MAX_LEN - 1;
	}

	memcpy(buf, beg, size * sizeof(wchar_t));

	buf[size] = 0;

	return size;
}

// 消除空格
inline size_t TrimWideStr(const wchar_t* str, wchar_t* buf, size_t maxlen)
{
	return TrimWideStrLen(str, wcslen(str), buf, maxlen);
}

// 单精度浮点数相等
inline bool FloatEqual(float v1, float v2)
{
	return (v1 <= (v2 + FLT_EPSILON)) && (v1 >= (v2 - FLT_EPSILON));
}

// 双精度浮点数相等
inline bool DoubleEqual(double v1, double v2)
{
	return (v1 <= (v2 + DBL_EPSILON)) && (v1 >= (v2 - DBL_EPSILON));
}

// 交换变量
template<typename TYPE>
void SwapValue(TYPE& v1, TYPE& v2)
{
	TYPE t(v1);

	v1 = v2;
	v2 = t;
}

inline NFGUID PBToNF(OuterMsg::GUID xID)
{
	NFGUID  xIdent;
	xIdent.nHead64 = xID.data1();
	xIdent.nData64 = xID.data2();

	return xIdent;
}

inline OuterMsg::GUID NFToPB(NFGUID xID)
{
	OuterMsg::GUID  xIdent;
	xIdent.set_data1(xID.nHead64);
	xIdent.set_data2(xID.nData64);

	return xIdent;
}

static bool AddNetPB(const google::protobuf::Message& xData, const NFGUID& id, std::string& strMsg)
{
	OuterMsg::MsgBase xMsg;
	if (!xData.SerializeToString(xMsg.mutable_msg_data()))
	{
		char szData[MAX_PATH] = { 0 };
		NFSPRINTF(szData, MAX_PATH, "AddNetPB SerializeToString failed \n");
#ifdef DEBUG
		std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

		return false;
	}

	OuterMsg::GUID* pPlayerID = xMsg.mutable_player_id();
	*pPlayerID = NFToPB(id);

	if (!xMsg.SerializeToString(&strMsg))
	{
		char szData[MAX_PATH] = { 0 };
		NFSPRINTF(szData, MAX_PATH, "AddNetPB SerializeToString failed \n");
#ifdef DEBUG
		std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

		return false;
	}
	
	return true;
}

static bool AddNetPB(const std::string& xData, const NFGUID& id, std::string& strMsg)
{
	OuterMsg::MsgBase xMsg;
	xMsg.set_msg_data(xData.data(), xData.length());

	OuterMsg::GUID* pPlayerID = xMsg.mutable_player_id();
	*pPlayerID = NFToPB(id);

	if (!xMsg.SerializeToString(&strMsg))
	{
		char szData[MAX_PATH] = { 0 };
		NFSPRINTF(szData, MAX_PATH, "AddNetPB SerializeToString failed \n");
#ifdef DEBUG
		std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

		return false;
	}

	return true;
}

static bool ReceivePB(const int nMsgID, const char* msg, const uint32_t nLen, google::protobuf::Message& xData, NFGUID& nPlayer)
{
	OuterMsg::MsgBase xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		char szData[MAX_PATH] = { 0 };
		NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);
#ifdef DEBUG
		std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

		return false;
	}

	if (!xData.ParseFromString(xMsg.msg_data()))
	{
		char szData[MAX_PATH] = { 0 };
		NFSPRINTF(szData, MAX_PATH, "Parse Message Failed from MsgData to ProtocolData, MessageID: %d\n", nMsgID);
#ifdef DEBUG
		std::cout << "--------------------" << szData << __FUNCTION__ << " " << __LINE__ << std::endl;
#endif // DEBUG

		return false;
	}

	nPlayer = PBToNF(xMsg.player_id());

	return true;
}

// 转换IP地址为整数
inline unsigned int convert_ip(const char* addr)
{
	char buf[32];
	CopyString(buf, sizeof(buf), addr);

	char* dot1 = strchr(buf, '.');
	if (NULL == dot1)
	{
		return 0;
	}

	char* dot2 = strchr(dot1 + 1, '.');
	if (NULL == dot2)
	{
		return 0;
	}

	char* dot3 = strchr(dot2 + 1, '.');
	if (NULL == dot3)
	{
		return 0;
	}

	*dot1 = 0;
	*dot2 = 0;
	*dot3 = 0;

	unsigned int part0 = atoi(buf);
	unsigned int part1 = atoi(dot1 + 1);
	unsigned int part2 = atoi(dot2 + 1);
	unsigned int part3 = atoi(dot3 + 1);

	return (part3 << 24) + (part2 << 16) + (part1 << 8) + part0;
}

// 反转IP地址
inline unsigned int reverse_ip(unsigned int addr)
{
	return ((addr >> 24) & 0xFF) + ((addr >> 8) & 0xFF00)
		+ ((addr << 24) & 0xFF000000) + ((addr << 8) & 0xFF0000);
}

inline bool isnum(string s)
{
	stringstream sin(s);
	double t;
	char p;
	if (!(sin >> t))
		/*解释：
			sin>>t表示把sin转换成double的变量（其实对于int和float型的都会接收），如果转换成功，则值为非0，如果转换不成功就返回为0
		*/
		return false;
	if (sin >> p)
		/*解释：此部分用于检测错误输入中，数字加字符串的输入形式（例如：34.f），在上面的的部分（sin>>t）已经接收并转换了输入的数字部分，在stringstream中相应也会把那一部分给清除，如果此时传入字符串是数字加字符串的输入形式，则此部分可以识别并接收字符部分，例如上面所说的，接收的是.f这部分，所以条件成立，返回false;如果剩下的部分不是字符，那么则sin>>p就为0,则进行到下一步else里面
		  */
		return false;
	else
		return true;
}

#endif
