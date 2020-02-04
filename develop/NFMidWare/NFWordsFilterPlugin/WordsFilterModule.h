///--------------------------------------------------------------------
/// 文件名:		WordsFilterModule.h
/// 内  容:		敏感词屏蔽
/// 说  明:		
/// 创建日期:	2019年9月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_WordsFilterModule_H__
#define __H_WordsFilterModule_H__
#include "NFComm/NFPluginModule/IWordsFilterModule.h"

class WordsFilterModule : public IWordsFilterModule
{
public:
	WordsFilterModule(NFIPluginManager* p);
	~WordsFilterModule();

	//管理器调用初始化
	virtual bool Init();
	//管理器调用初始化后
	virtual bool AfterInit();
	//管理器调用关闭
	virtual bool Shut();
	
	//增加一个敏感词
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord);

	//查找字符串里所有的敏感词
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out);

	//把字符串里所有的敏感词替换 返回新串
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*');

	//检测字符串里是否有敏感词
	virtual bool ExistSensitiveWord(const std::string& text) const;
private:

	struct TreeNode
	{
		TreeNode()
		{
			c_ = '0';
			is_end_ = false;
		}

		TreeNode(unsigned char c, bool is_end) :
			c_(c), is_end_(is_end)
		{
		}

		TreeNode* findChild(const unsigned char next_char) const
		{
			if (subtree_map_.count(next_char))
			{
				return subtree_map_.at(next_char);
			}

			return nullptr;
		}

		// insert and return child node
		TreeNode* insertChild(const unsigned char next_char)
		{
			// already have the child
			if (findChild(next_char))
			{
				return nullptr;
			}

			TreeNode* child = new TreeNode(next_char, false);
			if (child == nullptr)
			{
				return nullptr;
			}

			subtree_map_.insert(std::make_pair(next_char, child));
			return child;
		}

		// keyword
		unsigned char c_;
		// end flag
		bool is_end_;
		// subtree
		std::map<unsigned char, TreeNode*> subtree_map_;
	};

private:
	void LoadSensitiveWordFile();

	bool Insert(TreeNode* parent, const std::string& sensitive_word);

	int CheckSensitiveWord(const TreeNode* node,
		const std::string& text,
		const int begin_index) const;

	int Utf8StringLen(const std::string& word) const;
private:
	//敏感词树根
	TreeNode mTreeRoot;
};



#endif //__H_WordsFilterModule_H__