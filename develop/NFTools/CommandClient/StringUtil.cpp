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
#include <locale>
#include <codecvt>

/// \breif  �и��ַ���-������
std::vector<std::string> StringUtil::SplitString(const std::string& str, const std::string& delim)
{
	std::regex re{ delim };
	return std::vector<std::string>{
		std::sregex_token_iterator(str.begin(), str.end(), re, -1),
			std::sregex_token_iterator()
	};
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

	return str.substr(pos_begin, pos_end - pos_begin + 1);
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

/// \breif    ��������ת��Ϊ�ַ���
template<typename T>
std::string StringUtil::TypeAsString(T&& value)
{
	return std::to_string(value);
}

/// \brief  ��str�ַ������pat������replace
void StringUtil::StrReplace(std::string& str, const std::string& pat, const std::string& replace)
{
	size_t pos = 0;

	while (pos < str.size())
	{
		pos = str.find(pat, pos);
		if (pos == std::wstring::npos)
		{
			break;
		}

		str.replace(pos, pat.size(), replace);
		pos += replace.size();
	}
}
