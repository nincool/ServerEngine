///--------------------------------------------------------------------
/// 文件名:		NFDynLib.cpp
/// 内  容:		动态库加载
/// 说  明:		
/// 创建日期:	2019年10月31日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "NFDynLib.h"

bool NFDynLib::Load()
{
	std::string strLibPath = "./";
	strLibPath += mstrName;
	mInst = (DYNLIB_HANDLE)DYNLIB_LOAD(strLibPath.c_str());

	return mInst != NULL;
}

bool NFDynLib::UnLoad()
{
	DYNLIB_UNLOAD(mInst);
	return true;
}

void* NFDynLib::GetSymbol(const char* szProcName)
{
	return (DYNLIB_HANDLE)DYNLIB_GETSYM(mInst, szProcName);
}