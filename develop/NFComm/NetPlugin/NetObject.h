///--------------------------------------------------------------------
/// 文件名:		NetObject.h
/// 内  容:		网络链接对象
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetObject_H__
#define __H_NetObject_H__
#if NF_PLATFORM == NF_PLATFORM_WIN
#include <WinSock2.h>
#include <windows.h>
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#if NF_PLATFORM == NF_PLATFORM_APPLE
#include <libkern/OSByteOrder.h>
#endif
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif
#include <sys/socket.h>
#include <unistd.h>
#endif
#include <string>
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NetServer.h"

class NetObject
{
public:

private:
	sockaddr_in sin;
	void* m_pUserData;
	std::string mstrBuff;
	std::string mstrUserData;
	std::string mstrSecurityKey;

	int32_t mnLogicState;
	int32_t mnGameID;
	NFGUID mnUserID;//player id
	NFGUID mnClientID;//temporary client id
	NFGUID mnHashIdentID;//hash ident, special for distributed
	NetServer* m_pNet;
	//
	NFSOCK nFD;
	bool bNeedRemove;
};

#endif //__H_NetObject_H__