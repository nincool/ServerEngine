///--------------------------------------------------------------------
/// 文件名:		dllmain.cpp
/// 内  容:		dllmain
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifdef _DEBUG

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "libevent.lib" )
#pragma comment( lib, "hiredis.lib") 
#pragma comment( lib, "NFCore_d.lib" )
#else

#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32" )
#pragma comment( lib, "libevent.lib" )
#pragma comment( lib, "hiredis.lib") 
#pragma comment( lib, "NFCore.lib" )

#else

#endif

#endif
