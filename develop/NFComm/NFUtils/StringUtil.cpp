///---------------------------------------------------------
/// �ļ���: StringUtil.h
/// �� ��:  �ַ���������
/// ˵ ��:       
/// ��������: 2019/08/10
/// ������: �ｨ
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------

#include "StringUtil.h"
#include <windows.h>

/// \breif  �и��ַ���-������(������ʽ������Ч����10000������4170ms��û�й��˿��ַ�)
//std::vector<std::string> StringUtil::SplitString(const std::string& str, const std::string& delim)
//{
//	std::regex re{ delim };
//	return std::vector<std::string>{
//		std::sregex_token_iterator(str.begin(), str.end(), re, -1),
//			std::sregex_token_iterator()
//	};
//}

/// \brief ����10000������118ms
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
			// ���˿�
			if (beforeIndex != lastIndex)
			{
				strVec.push_back(Trim(str.substr(beforeIndex, lastIndex - beforeIndex)));
			}
			beforeIndex = lastIndex + delim.size();
		}
	} while (lastIndex != std::string::npos);

	return std::move(strVec);
}

/// \brief  ȥ��str�ַ�����ǰ���ָ���ַ�
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
		//���ʾ����ַ�������ַ�ȫ����ch��ɣ���������Ϊ�մ�
		return "";
	}

	return std::move(str.substr(pos_begin, pos_end - pos_begin + 1));
}

/// \breif    Trim ȥ��str�ַ����е�ָ���ַ�
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

/// \brief  �ַ���ת��Ϊ64����
int64_t StringUtil::StringAsInt64(const char* value, int64_t def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return _atoi64(value);
}

/// \brief  �ַ���ת��Ϊ����
int StringUtil::StringAsInt(const char* value, int def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return std::stoi(value);
}

/// \brief  �ַ���ת��Ϊ������
float StringUtil::StringAsFloat(const char* value, float def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return std::stof(value);
}

/// \brief  �ַ���ת��Ϊ˫����
double StringUtil::StringAsDouble(const char* value, double def_value)
{
	if (!value || !*value)
	{
		return def_value;
	}

	return std::stod(value);
}

/// \brief  ��str�ַ������pat������replace
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
	wchar_t* pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β�� 	
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

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��
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