///--------------------------------------------------------------------
/// 文件名:		NFConsistentHash.cpp
/// 内  容:		一致性哈希
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_CONSISTENT_HASH_H
#define NF_CONSISTENT_HASH_H

#include <map>
#include <string>
#include <list>
#include <functional> 
#include <algorithm>
#include <chrono>
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "Dependencies/common/crc32.hpp"

//template <typename T>
class NFIVirtualNode 
{
public:

    NFIVirtualNode(const int nVirID)
        :nVirtualIndex(nVirID)
    {

    }

	NFIVirtualNode()
	{
		nVirtualIndex = 0;
	}

	virtual ~NFIVirtualNode()
	{
		nVirtualIndex = 0;
	}

	virtual std::string GetDataStr() const
	{
		return "";
	}

    std::string ToStr() const 
    {
        std::ostringstream strInfo;
        strInfo << GetDataStr() << "-" << nVirtualIndex;
        return strInfo.str();
    }

private:
    int nVirtualIndex;
};

template <typename T>
class NFVirtualNode : public NFIVirtualNode
{
public:
	NFVirtualNode(const T tData, const int nVirID ) : NFIVirtualNode(nVirID)
	{
		mxData = tData;
	}
	NFVirtualNode()
	{
	}

	virtual std::string GetDataStr() const
	{
		return lexical_cast<std::string>(mxData);
	}

	T mxData;
};

class NFIHasher
{
public:
    virtual uint32_t GetHashValue(const NFIVirtualNode& vNode) = 0;
};

class NFHasher : public NFIHasher
{
public:
    virtual uint32_t GetHashValue(const NFIVirtualNode& vNode)
    {
        std::string vnode = vNode.ToStr();
        return NFrame::CRC32(vnode);
    }
};

template <typename T>
class NFIConsistentHash
{
public:
	virtual std::size_t Size() const = 0;
	virtual bool Empty() const = 0;

	virtual void ClearAll() = 0;
	virtual void Insert(const T& name) = 0;
	virtual void Insert(const NFVirtualNode<T>& xNode) = 0;

	virtual bool Exist(const NFVirtualNode<T>& xInNode) = 0;
	virtual void Erase(const T& name) = 0;
	virtual std::size_t Erase(const NFVirtualNode<T>& xNode)  = 0;

	virtual bool GetSuitNodeConsistent(NFVirtualNode<T>& node) = 0;
	virtual bool GetSuitNode(const T& name, NFVirtualNode<T>& node) = 0;
	virtual bool GetSuitNode(uint32_t hashValue, NFVirtualNode<T>& node) = 0;
};

template <typename T>
class NFConsistentHash : public NFIConsistentHash<T>
{
public:
    NFConsistentHash()
    {
        m_pHasher = new NFHasher();
    }

    virtual ~NFConsistentHash()
    {
        delete m_pHasher;
        m_pHasher = NULL;
    }

public:
	virtual std::size_t Size() const
    {
        return mxNodes.size();
    }

	virtual bool Empty() const
    {
        return mxNodes.empty();
    }

	virtual void ClearAll()
	{
		mxNodes.clear();
	}

	virtual void Insert(const T& name)
	{
		for (int i = 0; i < mnNodeCount; ++i)
		{
			NFVirtualNode<T> vNode(name, i);
			Insert(vNode);
		}
	}

	virtual void Insert(const NFVirtualNode<T>& xNode)
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        auto it = mxNodes.find(hash);
        if (it == mxNodes.end())
        {
            mxNodes.insert(typename std::map<uint32_t, NFVirtualNode<T>>::value_type(hash, xNode));
        }
    }

	virtual bool Exist(const NFVirtualNode<T>& xInNode)
	{
		uint32_t hash = m_pHasher->GetHashValue(xInNode);
		typename std::map<uint32_t, NFVirtualNode<T>>::iterator it = mxNodes.find(hash);
		if (it != mxNodes.end())
		{
			return true;
		}

		return false;
	}

	virtual void Erase(const T& name)
	{
		for (int i = 0; i < mnNodeCount; ++i)
		{
			NFVirtualNode<T> vNode(name, i);
			Erase(vNode);
		}
	}

	virtual std::size_t Erase(const NFVirtualNode<T>& xNode)
    {
        uint32_t hash = m_pHasher->GetHashValue(xNode);
        return mxNodes.erase(hash);
    }

	virtual bool GetSuitNodeConsistent(NFVirtualNode<T>& node)
	{
		return GetSuitNode(0, node);
	}

	virtual bool GetSuitNode(const T& name, NFVirtualNode<T>& node)
	{
		std::string str = lexical_cast<std::string>(name);
		uint32_t nCRC32 = NFrame::CRC32(str);
		return GetSuitNode(nCRC32, node);
	}

	virtual bool GetSuitNode(uint32_t hashValue, NFVirtualNode<T>& node)
	{
		if(mxNodes.empty())
		{
			return false;
		}

		typename std::map<uint32_t, NFVirtualNode<T>>::iterator it = mxNodes.lower_bound(hashValue);

		if (it == mxNodes.end())
		{
			it = mxNodes.begin();
		}

		node = it->second;

		return true;
	}

private:
	int mnNodeCount = 500;
	typename std::map<uint32_t, NFVirtualNode<T>> mxNodes;
    NFIHasher* m_pHasher;
};


#endif
