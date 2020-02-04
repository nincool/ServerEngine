///---------------------------------------------------------
/// 文件名: StringUtil.h
/// 内 容:  字符串操作类
/// 说 明:       
/// 创建日期: 2019/08/10
/// 创建人: 孙建
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------

#include "StringUtil.h"
#include <windows.h>

/// \breif  切割字符串-新特性(正则表达式不够高效测试10000次消耗4170ms，没有过滤空字符)
//std::vector<std::string> StringUtil::SplitString(const std::string& str, const std::string& delim)
//{
//	std::regex re{ delim };
//	return std::vector<std::string>{
//		std::sregex_token_iterator(str.begin(), str.end(), re, -1),
//			std::sregex_token_iterator()
//	};
//}

/// \brief 测试10000次消耗118ms
std::vector<std::string> StringUtil::SplitString(const std::string& str, const std::string& delim)
{
	std::vector<std::string> strVec;
	if (str.empty() || delim.empty())
	{
		return std::move(strVec);
	}

	size_t beforeIndex = 0;
	size_t lastIndex = 0;
	do
	{
		lastIndex = str.find(delim.c_str(), beforeIndex);
		if (lastIndex == std::string::npos)
		{
			if (beforeIndex != str.size())
			{
				strVec.push_back(Trim(str.substr(beforeIndex)));
			}
		}
		else
		{
			// 过滤空
			if (beforeIndex != lastIndex)
			{
				strVec.push_back(Trim(str.substr(beforeIndex, lastIndex - beforeIndex)));
			}
			beforeIndex = lastIndex + delim.size();
		}
	} while (lastIndex != std::string::npos);

	return std::move(strVec);
}

/// \brief  去除str字符串中前后的指定字符
std::string StringUtil::Trim(const std::string& str, const char ch)
{
	if (str.empty())
	{
		return "";
	}

	if (str[0] != ch && str[str.size() - 1] != ch)
	{
		return str;
	}

	size_t pos_begin = str.find_first_not_of(ch, 0);
	size_t pos_end = str.find_last_not_of(ch, str.size());
	if (pos_begin == std::string::npos || pos_end == std::string::npos)
	{
		//这表示这个字符串里的字符全都是ch组成，把它设置为空串
		return "";
	}

	return std::move(str.substr(pos_begin, pos_end - pos_begin + 1));
}

/// \breif    Trim 去除str字符串中的指定字符
void StringUtil::TrimAll(std::string& str, const char ch/* = ' '*/)
{
	size_t index = 0;
	if (!str.empty())
	{
		while ((index = str.find(ch, index)) != std::string::npos)
		{
			str.erase(index, 1);
		}
	}
}

/// \brief  字符串转换为64整数
int64_t StringUtil::StringAsInt64(const char* value, int64_t def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return _atoi64(value);
}

/// \brief  字符串转换为整数
int StringUtil::StringAsInt(const char* value, int def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return std::stoi(value);
}

/// \brief  字符串转换为浮点数
float StringUtil::StringAsFloat(const char* value, float def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return std::stof(value);
}

/// \brief  字符串转换为双精度
double StringUtil::StringAsDouble(const char* value, double def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return std::stod(value);
}

/// \brief  把str字符串里的pat都换成replace
void StringUtil::StrReplace(std::string& str, const std::string& pat, const std::string& replace)
{
	size_t pos = 0;

	while (pos < str.size())
	{
		pos = str.find(pat, pos);
		if (pos == std::string::npos)
		{
			break;
		}

		str.replace(pos, pat.size(), replace);
		pos += replace.size();
	}
}

std::string StringUtil::StringToUTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴 	
	memset(pwBuf, 0, nwLen * 2 + 2);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr(pBuf); 	
	delete[]pwBuf;	
	delete[]pBuf; 	
	pwBuf = NULL;
	pBuf = NULL;
	return std::move(retStr);
}

std::string StringUtil::UTF8ToString(const std::string& utf8Data)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, utf8Data.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, utf8Data.c_str(), utf8Data.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return std::move(retStr);
}