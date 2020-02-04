///--------------------------------------------------------------------
/// 文件名:		SkipList.cpp
/// 内  容:		跳表
/// 说  明:		
/// 创建日期:	2019年10月22日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "SkipList.h"
#include "NFComm/NFCore/NFDateTime.hpp"

SkipList::SkipList(const std::string& name, 
	int nRankLimit, 
	const std::vector<RankOrderType>& vorder, 
	const std::vector<std::string>& vdata)
	: m_strName(name)
	, m_nRankLimit(nRankLimit)
	, m_vOrderType(vorder)
	, m_vDataName(vdata)
{
	m_mapSkipNode[NULL_OBJECT] = NF_MAKE_SPTR<SkipNode>(NULL_OBJECT, m_vOrderType, MAX_LEVEL);
	m_pHeader = m_mapSkipNode[NULL_OBJECT].get();
	m_pTail = m_pHeader;

	if (m_nRankLimit <= 0)
	{
		m_nRankLimit = INT_MAX;
	}

	m_nRankLimitDouble = static_cast<int64_t>(m_nRankLimit) * 2;
}

bool SkipList::Insert(const NFGUID& key, std::vector<int64_t>& vscore, int64_t insert_time, NFGUID& add, NFGUID& del)
{
	if (key.IsNull())
	{
		return false;
	}

	if (vscore.size() != m_vOrderType.size())
	{
		return false;
	}

	if (insert_time <= 0)
	{
		insert_time = NFDateTime::NowTime();
	}

	SkipNode* node = Find(key);
	if (node != nullptr)
	{
		// 已存在节点，弹出此节点
		if (!Pop(node))
		{
			return false;
		}
	}
	else
	{
		// 超出排名限制，弹出最后一个节点
		if (m_nLength > m_nRankLimitDouble)
		{
			node = m_pTail;
			if (node->_score.Less(vscore, insert_time, key))
			{
				// 不上榜
				return false;
			}

			NF_SHARE_PTR<SkipNode> pNode = m_mapSkipNode[node->_key];
			if (!Remove(node->_key))
			{
				return false;
			}

			// 记录被删除的节点
			del = pNode->_key;

			// 更新节点key
			m_mapSkipNode[key] = pNode;
			node->_key = key;

			add = key;
		}
		else 
		{
			// 创建新的节点
			NF_SHARE_PTR<SkipNode> pNode = NF_MAKE_SPTR<SkipNode>(key, m_vOrderType, 0);
			m_mapSkipNode[key] = pNode;
			node = pNode.get();

			add = key;
		}
	}

	// 更新节点分数
	node->_score._vscore = std::move(vscore);
	node->_score._insert_time = insert_time;
	node->_vlevel.resize(RandomLevel());
	
	return Push(node);
}

bool SkipList::Remove(const NFGUID& key)
{
	auto it = m_mapSkipNode.find(key);
	if (it == m_mapSkipNode.end())
	{
		return false;
	}

	SkipNode* node = it->second.get();
	if (node == nullptr)
	{
		return false;
	}

	if (!Pop(node))
	{
		return false;
	}

	m_mapSkipNode.erase(it);

	return true;
}

int SkipList::GetRank(const NFGUID& key) const
{
	SkipNode* node = Find(key);
	if (node == nullptr)
	{
		// 不在榜上
		return 0;
	}

	int rank = 0;
	SkipNode* x = m_pHeader;
	for (int i = m_nLevel - 1; i >= 1; --i)
	{
		while (x->_vlevel[i]._next != nullptr
			&& (x->_vlevel[i]._next->_score < node->_score))
		{
			rank += x->_vlevel[i]._span;
			x = x->_vlevel[i]._next;
		}
	}

	while (x->_vlevel[0]._next != nullptr)
	{
		rank += x->_vlevel[0]._span;
		x = x->_vlevel[0]._next;
		if (x == node)
		{
			return rank > m_nRankLimit ? 0 : rank;
		}
	}

	QLOG_ERROR << __FUNCTION__ << " error rank key:" << key;
	return 0;
}

bool SkipList::GetRange(int start, int count, std::vector<NFGUID>& vrange) const
{
	if (start > m_nLength || start <= 0)
	{
		return false;
	}

	// 获取数量限制
	if (count > MAX_EVERY_GET_RANGE)
	{
		count = MAX_EVERY_GET_RANGE;
	}

	vrange.clear();
	int end = start + count - 1;
	if (end > m_nLength)
	{
		count = m_nLength - start + 1;
	}
	vrange.resize(count);

	int rank = 0;
	SkipNode* x = m_pHeader;
	for (int i = m_nLevel - 1; i >= 0; --i)
	{
		while (x->_vlevel[i]._next != nullptr)
		{
			int temp_rank = rank + x->_vlevel[i]._span;
			if (temp_rank > start)
			{
				break;
			}
			rank = temp_rank;

			x = x->_vlevel[i]._next;
		}

		if (rank == start)
		{
			break;
		}
	}

	for (int i = 0; i < count; ++i)
	{
		if (x == nullptr)
		{
			break;
		}

		vrange[i] = x->_key;
		x = x->_vlevel[0]._next;
	}

	return !vrange.empty();
}

bool SkipList::GetScores(const NFGUID& key, std::vector<int64_t>& vec_score, int64_t& insert_time) const
{
	if (key.IsNull())
	{
		return false;
	}

	auto* pNode = Find(key);
	if (pNode == nullptr)
	{
		return false;
	}

	vec_score = pNode->_score._vscore;
	insert_time = pNode->_score._insert_time;
	return true;
}

bool SkipList::GetTailScores(std::vector<int64_t>& vec_score, int64_t& insert_time) const
{
	if (m_pTail == nullptr)
	{
		return false;
	}

	vec_score = m_pTail->_score._vscore;
	insert_time = m_pTail->_score._insert_time;
	return true;
}

SkipNode* SkipList::Find(const NFGUID& key) const
{
	auto it = m_mapSkipNode.find(key);
	if (it == m_mapSkipNode.end())
	{
		return nullptr;
	}

	return it->second.get();
}

bool SkipList::Push(SkipNode* node)
{
	//TestPrint("push begin");

	SkipNode* update[MAX_LEVEL] = { nullptr };
	SkipNode* x = nullptr;
	int rank[MAX_LEVEL+1] = { 0 };

	// 从上到下 搜集所有前驱及排名
	x = m_pHeader;
	for (int i = m_nLevel - 1; i >= 0; --i)
	{
		rank[i] = rank[i+1];
		while (x->_vlevel[i]._next != nullptr
			&& x->_vlevel[i]._next->_score < node->_score)
		{
			rank[i] += x->_vlevel[i]._span;
			x = x->_vlevel[i]._next;
		}

		update[i] = x;
	}

	// 更新最高层级
	int nLevel = node->_vlevel.size();
	if (nLevel > m_nLevel)
	{
		for (int i = m_nLevel; i < nLevel; ++i)
		{
			rank[i] = 0;
			update[i] = m_pHeader;
			update[i]->_vlevel[i]._span = m_nLength;
		}

		m_nLevel = nLevel;
	}

	// 插入节点
	x = node;
	for (int i = 0; i < nLevel; i++) 
	{
		x->_vlevel[i]._next = update[i]->_vlevel[i]._next;
		update[i]->_vlevel[i]._next = x;

		x->_vlevel[i]._span = update[i]->_vlevel[i]._span - (rank[0] - rank[i]);
		update[i]->_vlevel[i]._span = (rank[0] - rank[i]) + 1;
	}

	// 更新跨度
	for (int i = nLevel; i < m_nLevel; i++) 
	{
		update[i]->_vlevel[i]._span++;
	}

	// 更新前驱指针
	x->_prior = (update[0] == m_pHeader) ? nullptr : update[0];
	if (x->_vlevel[0]._next != nullptr)
		x->_vlevel[0]._next->_prior = x;
	else
		m_pTail = x;
	
	// 更新总长度
	m_nLength++;

	//TestPrint("push end");

	return true;
}

bool SkipList::Pop(SkipNode* node)
{
	//TestPrint("pop begin");

	SkipNode* update[MAX_LEVEL] = { nullptr };
	SkipNode* x = nullptr;

	// 从上到下 搜集所有前驱
	x = m_pHeader;
	for (int i = m_nLevel - 1; i >= 0; --i)
	{
		while (x->_vlevel[i]._next != nullptr
			&& x->_vlevel[i]._next->_score < node->_score)
		{
			x = x->_vlevel[i]._next;
		}

		update[i] = x;
	}

	// 判断是否找到删除节点
	x = x->_vlevel[0]._next;
	if (x != node)
	{
		return false;
	}

	// 调整尾节点
	if (m_pTail == x)
	{
		m_pTail = x->_prior;
	}
	else
	{
		// 调整前驱
		x->_vlevel[0]._next->_prior = x->_prior;
	}
	x->_prior = nullptr;

	// 调整后继及跨度
	for (int i = 0; i < m_nLevel; ++i)
	{
		if (update[i]->_vlevel[i]._next == x)
		{
			update[i]->_vlevel[i]._next = x->_vlevel[i]._next;
			update[i]->_vlevel[i]._span += x->_vlevel[i]._span - 1;
			x->_vlevel[i]._next = nullptr;
		}
		else
		{
			update[i]->_vlevel[i]._span--;
		}
	}

	// 更新最高层级
	for (int i = m_nLevel - 1; i >= 0; --i)
	{
		if (m_pHeader->_vlevel.back()._next != nullptr)
		{
			m_nLevel = i + 1;
			break;
		}
	}

	// 更新总量
	--m_nLength;

	//TestPrint("pop end");

	return true;;
}

int SkipList::RandomLevel(void)
{
	int nLevel = 1;
	while ((rand() & 0xFFFF) < (0.25 * 0xFFFF))
		nLevel += 1;
	return (nLevel < MAX_LEVEL) ? nLevel : MAX_LEVEL;
}

void SkipList::TestPrint(std::string flag)
{
	std::cout.setf(std::ios::left);
	std::cout << flag << std::endl;

	SkipNode* x = m_pHeader;
	int rank = 0;
	while (x != nullptr)
	{
		std::cout.width(3);
		std::cout << " rank:" << (rank++);
		std::cout.width(3);
		std::cout << " id:" << x->_key.GetData();
		int64_t prior = (x->_prior != nullptr) ? x->_prior->_key.GetData() : 0;
		std::cout.width(3);
		std::cout << " prior:" << prior;
		std::cout.width(3);
		std::cout << " level:";
		for (int i = 0; i < x->_vlevel.size(); ++i)
		{
			std::cout << " ";
			std::cout.width(3);
			std::cout << (((x->_vlevel[i]._next) != nullptr) ? (x->_vlevel[i]._next->_key.GetData()) : 0);
		}
		std::cout << std::endl;

		x = x->_vlevel[0]._next;
	}
	std::cout << " maxlevel:" << m_nLevel << " length:" << m_nLength << std::endl;
}