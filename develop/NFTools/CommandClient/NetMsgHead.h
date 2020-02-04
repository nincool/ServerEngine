///--------------------------------------------------------------------
/// 文件名:		NetMsgHead.h
/// 内  容:		Socket通信消息头
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetMsgHead_H__
#define __H_NetMsgHead_H__

#include <WinSock2.h>
#include <windows.h>
#include <assert.h>

class NetMsgHead
{
public:
	NetMsgHead()
	{

	}
	~NetMsgHead()
	{

	}

	int64_t NF_HTONLL(int64_t nData)
	{
#if NF_PLATFORM == NF_PLATFORM_WIN
		return htonll(nData);
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
		return OSSwapHostToBigInt64(nData);
#else
		return htobe64(nData);
#endif
	}

	int64_t NF_NTOHLL(int64_t nData)
	{
#if NF_PLATFORM == NF_PLATFORM_WIN
		return ntohll(nData);
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
		return OSSwapBigToHostInt64(nData);
#elif NF_PLATFORM == NF_PLATFORM_ANDROID
		return betoh64(nData);
#else
		return be64toh(nData);
#endif
	}

	int32_t NF_HTONL(int32_t nData)
	{
#if NF_PLATFORM == NF_PLATFORM_WIN
		return htonl(nData);
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
		return OSSwapHostToBigInt32(nData);
#else
		return htobe32(nData);
#endif
	}

	int32_t NF_NTOHL(int32_t nData)
	{
#if NF_PLATFORM == NF_PLATFORM_WIN
		return ntohl(nData);
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
		return OSSwapBigToHostInt32(nData);
#elif NF_PLATFORM == NF_PLATFORM_ANDROID
		return betoh32(nData);
#else
		return be32toh(nData);
#endif
	}

	int16_t NF_HTONS(int16_t nData)
	{
#if NF_PLATFORM == NF_PLATFORM_WIN
		return htons(nData);
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
		return OSSwapHostToBigInt16(nData);
#else
		return htobe16(nData);
#endif
	}

	int16_t NF_NTOHS(int16_t nData)
	{
#if NF_PLATFORM == NF_PLATFORM_WIN
		return ntohs(nData);
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
		return OSSwapBigToHostInt16(nData);
#elif NF_PLATFORM == NF_PLATFORM_ANDROID
		return betoh16(nData);
#else
		return be16toh(nData);
#endif
	}

	// Message Head[ MsgID(2) | MsgSize(4) ]
	virtual int EnCode(char* strData)
	{
		uint32_t nOffset = 0;

		uint16_t nMsgID = NF_HTONS(munMsgID);
		memcpy(strData + nOffset, (void*)(&nMsgID), sizeof(munMsgID));
		nOffset += sizeof(munMsgID);

		uint32_t nPackSize = munSize + HEAD_LENGTH;
		uint32_t nSize = NF_HTONL(nPackSize);
		memcpy(strData + nOffset, (void*)(&nSize), sizeof(munSize));
		nOffset += sizeof(munSize);

		if (nOffset != HEAD_LENGTH)
		{
			assert(0);
		}

		return nOffset;
	}

	// Message Head[ MsgID(2) | MsgSize(4) ]
	virtual int DeCode(const char* strData)
	{
		uint32_t nOffset = 0;

		uint16_t nMsgID = 0;
		memcpy(&nMsgID, strData + nOffset, sizeof(munMsgID));
		munMsgID = NF_NTOHS(nMsgID);
		nOffset += sizeof(munMsgID);
		uint32_t nPackSize = 0;
		memcpy(&nPackSize, strData + nOffset, sizeof(munSize));
		munSize = NF_NTOHL(nPackSize) - HEAD_LENGTH;
		nOffset += sizeof(munSize);

		if (nOffset != HEAD_LENGTH)
		{
			assert(0);
		}

		return nOffset;
	}

	virtual uint16_t GetMsgID() const
	{
		return munMsgID;
	}

	virtual void SetMsgID(uint16_t nMsgID)
	{
		munMsgID = nMsgID;
	}

	virtual uint32_t GetBodyLength() const
	{
		return munSize;
	}

	virtual void SetBodyLength(uint32_t nLength)
	{
		munSize = nLength;
	}
protected:
	//消息内容大小
	unsigned int munSize = 0;
	//消息ID
	unsigned short munMsgID = 0;

public:
	static const int HEAD_LENGTH = 6;
};


#endif //__H_NetMsgHead_H__