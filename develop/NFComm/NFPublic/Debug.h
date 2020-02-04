///--------------------------------------------------------------------
/// 文件名:		Debug.h 
/// 内  容:		调试工具
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PUBLIC_DEBUG_H
#define _PUBLIC_DEBUG_H
#include <assert.h>

#if NF_PLATFORM == NF_PLATFORM_WIN
#include <crtdbg.h>
#define NFASSERT(exp_, msg_, file_, func_)  \
    std::string strInfo("Error:");        \
    strInfo += msg_  + std::string("\n\nFile:") + std::string(file_) + std::string("\n Function:") + func_; \
    MessageBoxA(0, strInfo.c_str(), ("Error_"#exp_), MB_RETRYCANCEL | MB_ICONERROR); \
    assert(0);
#else
#define NFASSERT(exp_, msg_, file_, func_)
#endif

#ifdef _DEBUG
#define NEW ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW ::new
#endif // _DEBUG

#ifdef _DEBUG
#define Assert(p)	\
	if (!(p))\
	{\
		std::string strInfo("Error:");        \
		strInfo += std::string("\n\nFile:") + std::string(__FILE__) + std::string("\n Function:") + __FUNCTION__; \
		MessageBoxA(0, strInfo.c_str(), ("Error"), MB_RETRYCANCEL | MB_ICONERROR); \
		assert(0);\
	}
#else
	#define Assert(p) (void(0))
#endif // _DEBUG


#endif // _PUBLIC_DEBUG_H
