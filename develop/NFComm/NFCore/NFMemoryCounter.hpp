///--------------------------------------------------------------------
/// 文件名:		NFMemoryCounter.cpp
/// 内  容:		内存计数
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_MEMORY_COUNTER_H
#define NF_MEMORY_COUNTER_H

#include <iostream>
#include <string>
#include <map>
#include "NFComm/NFPluginModule/NFPlatform.h"

template <class T>
class NFMemoryCounter
{
private:
	NFMemoryCounter() {}
public:
	NFMemoryCounter(const std::string& strClassName)
	{
		mstrClassName = strClassName;
		mxCounter[mstrClassName]++;
	}

	virtual ~NFMemoryCounter()
	{
		mxCounter[mstrClassName]--;
	}

	std::string mstrClassName;
	static std::map<std::string, int> mxCounter;
};

template <class T>
std::map<std::string, int> NFMemoryCounter<T>::mxCounter;

#endif