///--------------------------------------------------------------------
/// �ļ���:		dllmain.cpp
/// ��  ��:		dllmain
/// ˵  ��:		
/// ��������:	2019��8��19��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
