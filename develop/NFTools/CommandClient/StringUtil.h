///---------------------------------------------------------
/// �ļ���: StringUtil.h
/// �� ��:  �ַ���������
/// ˵ ��:       
/// ��������: 2019/08/10
/// ������: �ｨ
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------

#ifndef NF_STRING_UTIL_H
#define NF_STRING_UTIL_H

#include <vector>
#include <string>
#include <regex>

// �ַ�������
class StringUtil
{
public:

	/// \breif    SplitString �ַ����и�-C++ 11������
	static std::vector<std::string> SplitString(const std::string& str, const std::string& delim);

	/// \breif    Trim ȥ��str�ַ�����ǰ���ָ���ַ�
	static std::string Trim(const std::string& str, const char ch = ' ');

	/// \breif    Trim ȥ��str�ַ����е�ָ���ַ�
	static void TrimAll(std::string& str, const char ch = ' ');

	/// \brief	  �ַ���ת��Ϊ64����
	static int64_t StringAsInt64(const char* value, int64_t def_value = 0);

	/// \brief    �ַ���ת��Ϊ����
	static int StringAsInt(const char* value, int def_value = 0);

	/// \breif    �ַ���ת��Ϊ������
	static float StringAsFloat(const char* value, float def_value = 0.0f);

	/// \breif    �ַ���ת��Ϊ˫���ȸ�����
	static double StringAsDouble(const char* value, double def_value = 0.0);

	/// \breif    ��������ת��Ϊ�ַ���
	template<typename T>
	static std::string TypeAsString(T&& value);

	/// \breif    StrReplace ��str�ַ������pat������replace
	static void StrReplace(std::string& str, const std::string& pat, const std::string& replace);
};

#endif// _STRINGUTIL_H
