///--------------------------------------------------------------------
/// 文件名:		SkipList.h
/// 内  容:		跳表
/// 说  明:		
/// 创建日期:	2019年10月22日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __NF_SKIP_LIST_H__
#define __NF_SKIP_LIST_H__

#include "NFComm/NFPluginModule/KConstDefine.h"

// 最大层数
const int MAX_LEVEL = 32;
// 每次查询范围最大数量
const int MAX_EVERY_GET_RANGE = 100;

// 分数
struct SkipScore
{
	std::vector<int64_t> _vscore; // 分数
	int64_t _insert_time = 0; // 插入时间
	const std::vector<RankOrderType>& _vorder; // 排序
	const NFGUID& _key;

	SkipScore(const std::vector<RankOrderType>& vorder, const NFGUID& key) 
		: _vorder(vorder), _key(key)
	{
		_vscore.resize(vorder.size(), 0);
	}

	// 重载小于比较
	bool operator < (const SkipScore& other) const
	{
		return Less(other._vscore, other._insert_time, other._key);
	}

	// 小于比较
	bool Less(const std::vector<int64_t>& vscore, int64_t insert_time, const NFGUID& other_key) const
	{
		for (int i = 0; i < _vorder.size(); ++i)
		{
			if (_vscore[i] != vscore[i])
			{
				return _vorder[i] == RANK_ORDER_DESC
					? _vscore[i] > vscore[i]
					: _vscore[i] < vscore[i];
			}
		}

		if (_insert_time != insert_time)
		{
			return _insert_time < insert_time;
		}

		if (_key != other_key)
		{
			return _key < other_key;
		}

		return false;
	}
};

// 节点
struct SkipNode
{
	NFGUID _key; // 键值
	SkipScore _score; // 分数

	SkipNode* _prior = nullptr; // 向前节点

	struct SkipLevel
	{
		SkipNode* _next = nullptr; // 向后节点
		int _span = 0; // 跨度
	};
	std::vector<SkipLevel> _vlevel;

	SkipNode(const NFGUID& key,
		const std::vector<RankOrderType>& vorder,
		int level)
		: _key(key)
		, _score(vorder, _key)
		, _vlevel(level)
	{
	}
};

// 跳表
class SkipList
{
public:

	SkipList(const std::string& name, int nRankLimit, const std::vector<RankOrderType>& vorder, const std::vector<std::string>& vdata);

	// 插入
	bool Insert(const NFGUID& key, std::vector<int64_t>& vscore, int64_t insert_time, NFGUID& add, NFGUID& del);
	// 删除
	bool Remove(const NFGUID& key);
	// 获取排名
	int GetRank(const NFGUID& key) const;
	// 获取范围
	bool GetRange(int start, int count, std::vector<NFGUID>& vrange) const;

	// 获取排行榜名称
	const std::string& GetName() const { return m_strName; };
	// 获取排行榜绑定数据名称
	const std::vector<std::string>& GetDataNames() const { return m_vDataName; };
	// 设置分数属性
	void SetScoreNames(const std::vector<std::string>& vec) { m_vScoreName = vec; };
	// 获取分数属性名
	const std::vector<std::string>& GetScoreNames() const { return m_vScoreName; };
	// 获取排序类型
	const std::vector<RankOrderType>& GetOrders() const { return m_vOrderType; }
	
	// 获取当前在排名限制内的最大名次
	int GetMaxRank() const { return m_nLength > m_nRankLimit ? m_nRankLimit : m_nLength; };
	// 数据是否被记录在榜单(名次不一定上榜)
	bool Exist(const NFGUID& key) const { return Find(key) != nullptr; };
	// 获取总节点数
	int GetLength() const { return m_nLength; };
	// 获取限制名次
	int GetLimitRank() const { return m_nRankLimit; }
	// 获取分数
	bool GetScores(const NFGUID& key, std::vector<int64_t>& vec_score, int64_t& insert_time) const;
	// 获取末尾积分
	bool GetTailScores(std::vector<int64_t>& vec_score, int64_t& insert_time) const;
private:
	// 查找
	SkipNode* Find(const NFGUID& key) const;
	// 放入
	bool Push(SkipNode* node);
	// 弹出
	bool Pop(SkipNode* node);
	// 生成层数 符合幂次法则分布
	int RandomLevel(void);

	// 测试打印
	void TestPrint(std::string flag);
private:
	SkipNode* m_pHeader = nullptr;	// 头指针
	SkipNode* m_pTail = nullptr;	// 尾指针
	int m_nLength = 0;	// 当前总节点数
	int m_nLevel = 0;	// 当前最大层数

	// 哈希表存储 NFGUID与分数数据的映射
	std::unordered_map<NFGUID, NF_SHARE_PTR<SkipNode>> m_mapSkipNode;

	// 排行榜名称
	std::string m_strName = "";

	// 排行榜保存名次限制
	int m_nRankLimit = 0;
	// 排行榜限制的两倍(用来决定排行榜上存在的条目数,作用是排行榜上榜分数变化后掉出榜单,有足够的备用数据顶上)
	int64_t m_nRankLimitDouble = 0;

	// 参数排序的分数排序类型
	std::vector<RankOrderType> m_vOrderType;
	// 排行榜数据名称
	std::vector<std::string> m_vDataName;
	// 分数属性名
	std::vector<std::string> m_vScoreName;
};
#endif // __NF_SKIP_LIST_H__
