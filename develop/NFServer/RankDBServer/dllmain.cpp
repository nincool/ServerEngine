///--------------------------------------------------------------------
/// �ļ���:		dllmain.cpp
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��8��30��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFComm/NFPluginModule/NFPlatform.h"

#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "NFCore_d.lib" )
#pragma comment( lib, "NFMessageDefine_d.lib" )
#pragma comment( lib, "libprotobufd.lib" )
#pragma comment( lib, "lua.lib" )
#pragma comment( lib, "mysqlpp_d.lib" )

#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "NFCore_d.a" )
#pragma comment( lib, "NFMessageDefine_d.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "NFCore.lib" )
#pragma comment( lib, "NFMessageDefine.lib" )
#pragma comment( lib, "libprotobuf.lib" )
#pragma comment( lib, "lua.lib" )
#pragma comment( lib, "mysqlpp.lib" )

#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "NFCore.a" )
#pragma comment( lib, "NFMessageDefine.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#endif

#endif