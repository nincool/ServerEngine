///--------------------------------------------------------------------
/// �ļ���:		SkipList.h
/// ��  ��:		����
/// ˵  ��:		
/// ��������:	2019��10��22��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __NF_SKIP_LIST_H__
#define __NF_SKIP_LIST_H__

#include "NFComm/NFPluginModule/KConstDefine.h"

// ������
const int MAX_LEVEL = 32;
// ÿ�β�ѯ��Χ�������
const int MAX_EVERY_GET_RANGE = 100;

// ����
struct SkipScore
{
	std::vector<int64_t> _vscore; // ����
	int64_t _insert_time = 0; // ����ʱ��
	const std::vector<RankOrderType>& _vorder; // ����
	const NFGUID& _key;

	SkipScore(const std::vector<RankOrderType>& vorder, const NFGUID& key) 
		: _vorder(vorder), _key(key)
	{
		_vscore.resize(vorder.size(), 0);
	}

	// ����С�ڱȽ�
	bool operator < (const SkipScore& other) const
	{
		return Less(other._vscore, other._insert_time, other._key);
	}

	// С�ڱȽ�
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

// �ڵ�
struct SkipNode
{
	NFGUID _key; // ��ֵ
	SkipScore _score; // ����

	SkipNode* _prior = nullptr; // ��ǰ�ڵ�

	struct SkipLevel
	{
		SkipNode* _next = nullptr; // ���ڵ�
		int _span = 0; // ���
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

// ����
class SkipList
{
public:

	SkipList(const std::string& name, int nRankLimit, const std::vector<RankOrderType>& vorder, const std::vector<std::string>& vdata);

	// ����
	bool Insert(const NFGUID& key, std::vector<int64_t>& vscore, int64_t insert_time, NFGUID& add, NFGUID& del);
	// ɾ��
	bool Remove(const NFGUID& key);
	// ��ȡ����
	int GetRank(const NFGUID& key) const;
	// ��ȡ��Χ
	bool GetRange(int start, int count, std::vector<NFGUID>& vrange) const;

	// ��ȡ���а�����
	const std::string& GetName() const { return m_strName; };
	// ��ȡ���а����������
	const std::vector<std::string>& GetDataNames() const { return m_vDataName; };
	// ���÷�������
	void SetScoreNames(const std::vector<std::string>& vec) { m_vScoreName = vec; };
	// ��ȡ����������
	const std::vector<std::string>& GetScoreNames() const { return m_vScoreName; };
	// ��ȡ��������
	const std::vector<RankOrderType>& GetOrders() const { return m_vOrderType; }
	
	// ��ȡ��ǰ�����������ڵ��������
	int GetMaxRank() const { return m_nLength > m_nRankLimit ? m_nRankLimit : m_nLength; };
	// �����Ƿ񱻼�¼�ڰ�(���β�һ���ϰ�)
	bool Exist(const NFGUID& key) const { return Find(key) != nullptr; };
	// ��ȡ�ܽڵ���
	int GetLength() const { return m_nLength; };
	// ��ȡ��������
	int GetLimitRank() const { return m_nRankLimit; }
	// ��ȡ����
	bool GetScores(const NFGUID& key, std::vector<int64_t>& vec_score, int64_t& insert_time) const;
	// ��ȡĩβ����
	bool GetTailScores(std::vector<int64_t>& vec_score, int64_t& insert_time) const;
private:
	// ����
	SkipNode* Find(const NFGUID& key) const;
	// ����
	bool Push(SkipNode* node);
	// ����
	bool Pop(SkipNode* node);
	// ���ɲ��� �����ݴη���ֲ�
	int RandomLevel(void);

	// ���Դ�ӡ
	void TestPrint(std::string flag);
private:
	SkipNode* m_pHeader = nullptr;	// ͷָ��
	SkipNode* m_pTail = nullptr;	// βָ��
	int m_nLength = 0;	// ��ǰ�ܽڵ���
	int m_nLevel = 0;	// ��ǰ������

	// ��ϣ��洢 NFGUID��������ݵ�ӳ��
	std::unordered_map<NFGUID, NF_SHARE_PTR<SkipNode>> m_mapSkipNode;

	// ���а�����
	std::string m_strName = "";

	// ���а񱣴���������
	int m_nRankLimit = 0;
	// ���а����Ƶ�����(�����������а��ϴ��ڵ���Ŀ��,���������а��ϰ�����仯�������,���㹻�ı������ݶ���)
	int64_t m_nRankLimitDouble = 0;

	// ��������ķ�����������
	std::vector<RankOrderType> m_vOrderType;
	// ���а���������
	std::vector<std::string> m_vDataName;
	// ����������
	std::vector<std::string> m_vScoreName;
};
#endif // __NF_SKIP_LIST_H__
