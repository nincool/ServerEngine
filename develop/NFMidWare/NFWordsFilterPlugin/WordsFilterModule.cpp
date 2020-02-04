///--------------------------------------------------------------------
/// 文件名:		WordsFilterModule.cpp
/// 内  容:		敏感词屏蔽
/// 说  明:		
/// 创建日期:	2019年9月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include <map>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include "WordsFilterModule.h"
#include "NFComm/NFUtils/QLOG.h"

WordsFilterModule::WordsFilterModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

WordsFilterModule::~WordsFilterModule()
{

}

//管理器调用初始化
bool WordsFilterModule::Init()
{
	LoadSensitiveWordFile();
	return true;
}

//管理器调用初始化后
bool WordsFilterModule::AfterInit()
{
	return true;
}

//管理器调用关闭
bool WordsFilterModule::Shut()
{
	return true;
}

void WordsFilterModule::LoadSensitiveWordFile()
{
	std::string strFile = pPluginManager->GetConfigPath() + "/NFDataCfg/SensitiveWord.txt";
	std::ifstream fp(strFile.c_str());
	if (!fp.is_open())
	{
		QLOG_WARING << __FUNCTION__ << " file:" << strFile << " not find";
		return;
	}

	std::string line;
	while (!fp.eof())
	{
		getline(fp, line);
		Insert(&mTreeRoot, line);
	}
	fp.close();
}

bool WordsFilterModule::AddSensitiveWord(const std::string& strSensitiveWord)
{
	return Insert(&mTreeRoot, strSensitiveWord);
}

void WordsFilterModule::FindAllSensitiveWords(const std::string& text, std::set<std::string>& out)
{
	int begin_index = 0;
	const int n = (int)text.size();
	while (begin_index < n)
	{
		int match_len = CheckSensitiveWord(&mTreeRoot, text, begin_index);
		if (match_len == 0)
		{
			begin_index++;
		}
		else
		{
			out.insert(text.substr(begin_index, match_len));
			begin_index += match_len;
		}
	}
}


void WordsFilterModule::ReplaceAllSensitiveWords(const std::string& text, std::string& out,
	const unsigned char replacedChar) 
{
	int begin_index = 0;
	int shift_len = 0;
	const int n = (int)text.size();
	while (begin_index < n)
	{
		int match_len = CheckSensitiveWord(&mTreeRoot, text, begin_index);
		if (match_len == 0)
		{
			begin_index++;
		}
		else
		{
			const std::string& matched_word = text.substr(begin_index, match_len);
			int utf8Len = Utf8StringLen(matched_word);

			out = out.substr(0, (size_t)begin_index - shift_len)
				+ std::string(utf8Len, replacedChar)
				+ out.substr((size_t)begin_index + match_len - shift_len);

			begin_index += match_len;
			shift_len += match_len - utf8Len;
		}
	}

	return;
}

bool WordsFilterModule::Insert(TreeNode* parent, const std::string& sensitive_word)
{
	const int n = (int)sensitive_word.size();
	for (int i = 0; i < n; i++)
	{
		if (!parent)
		{
			return false;
		}

		const unsigned char c = sensitive_word[i];
		TreeNode* child = parent->findChild(c);
		if (!child)
		{
			parent = parent->insertChild(c);
		}
		else
		{
			parent = child;
		}

		if (i == n - 1)
		{
			parent->is_end_ = true;
		}
	}

	return true;
}

int WordsFilterModule::CheckSensitiveWord(const TreeNode* node,
	const std::string& text,
	const int begin_index) const
{
	bool flag = false;
	int match_len = 0;
	const int n = (int)text.size();
	for (int i = begin_index; i < n; i++)
	{
		const unsigned char c = text[i];
		node = node->findChild(c);
		if (!node)
		{
			break;
		}
		else
		{
			match_len++;
			if (node->is_end_)
			{
				flag = true;
			}
		}
	}

	if (match_len < 1 || !flag)
	{
		match_len = 0;
	}

	return match_len;
}

int WordsFilterModule::Utf8StringLen(const std::string& word) const
{
	const char* s = word.c_str();
	int len = 0;
	while (*s) len += (*s++ & 0xc0) != 0x80;

	return len;
}

bool WordsFilterModule::ExistSensitiveWord(const std::string& text) const
{
	int begin_index = 0;
	const int n = (int)text.size();
	while (begin_index < n)
	{
		int match_len = CheckSensitiveWord(&mTreeRoot, text, begin_index);
		if (match_len == 0)
		{
			begin_index++;
		}
		else
		{
			return true;
		}
	}

	return false;
}