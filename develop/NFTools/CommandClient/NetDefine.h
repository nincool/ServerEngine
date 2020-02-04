///--------------------------------------------------------------------
/// �ļ���:		NetDefine.h
/// ��  ��      ������ض���
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_NetDefine_H__
#define __H_NetDefine_H__

//���������¼�
enum class NET_EVENT
{
	NET_EVENT_EOF = 0x10,		//���ӹر�
	NET_EVENT_ERROR = 0x20,		//���ӳ���
	NET_EVENT_TIMEOUT = 0x40,	//��ȡ��ʱ
	NET_EVENT_CONNECTED = 0x80,	//���ӳɹ�
};


#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "libevent_core.lib" )
#pragma comment( lib, "libevent.lib" )

#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "libevent_core.a" )
#pragma comment( lib, "libevent.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32.lib" )
#pragma comment( lib, "libevent_core.lib" )
#pragma comment( lib, "libevent.lib" )

#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "libevent.a" )
#pragma comment( lib, "libevent_core.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif


#endif //__H_NetDefine_H__