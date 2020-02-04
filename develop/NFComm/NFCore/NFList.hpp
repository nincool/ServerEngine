///--------------------------------------------------------------------
/// 文件名:		NFList.cpp
/// 内  容:		NF列表
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_LIST_H
#define NF_LIST_H

#include <iostream>
#include <map>
#include <list>
#include <algorithm>
#include "NFComm/NFPluginModule/NFPlatform.h"

template <typename T>
class NFList
{
public:
	virtual ~NFList() {}
	bool Add(const T& id);
	bool Find(const T& id);
	bool Remove(const T& id);
	bool ClearAll();

	bool First(T& id);
	bool Next(T& id);
	bool Get(const int32_t index, T& id);
	int Count();

protected:
	typedef std::list<T> TLISTOBJCONFIGLIST;
	TLISTOBJCONFIGLIST mtObjConfigList;
	typename std::list<T>::iterator mCurIter;
};

template <typename T>
bool NFList<T>::Add(const T& id)
{
	//if (!Find(id))
	{
		mtObjConfigList.push_back(id);
		return true;
	}

	return false;
}

template <typename T>
bool NFList<T>::Remove(const T& id)
{
	if (Find(id))
	{
		mtObjConfigList.remove(id);
		return true;
	}

	return false;
}

template <typename T>
bool NFList<T>::ClearAll()
{
	mtObjConfigList.clear();
	return true;
}

template <typename T>
bool NFList<T>::First(T& id)
{
	if (mtObjConfigList.size() <= 0)
	{
		return false;
	}

	mCurIter = mtObjConfigList.begin();
	if (mCurIter != mtObjConfigList.end())
	{
		id = *mCurIter;
		return true;
	}

	return false;
}

template <typename T>
bool NFList<T>::Next(T& id)
{
	if (mCurIter == mtObjConfigList.end())
	{
		return false;
	}

	++mCurIter;
	if (mCurIter != mtObjConfigList.end())
	{
		id = *mCurIter;
		return true;
	}

	return false;
}

template <typename T>
bool NFList<T>::Find(const T& id)
{
	typename TLISTOBJCONFIGLIST::iterator it = std::find(mtObjConfigList.begin(), mtObjConfigList.end(), id);
	if (it != mtObjConfigList.end())
	{
		return true;
	}

	return false;
}

template <typename T>
bool NFList<T>::Get(const int32_t index, T& id)
{
	if (index >= mtObjConfigList.size())
	{
		return false;
	}

	typename std::list<T>::iterator it = this->mtObjConfigList.begin();
	std::advance(it, index);

	id = *it;

	return true;
}

template <typename T>
int NFList<T>::Count()
{
	return (int)(mtObjConfigList.size());
}

#endif
