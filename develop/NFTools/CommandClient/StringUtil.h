///---------------------------------------------------------
/// 文件名: StringUtil.h
/// 内 容:  字符串操作类
/// 说 明:       
/// 创建日期: 2019/08/10
/// 创建人: 孙建
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------

#ifndef NF_STRING_UTIL_H
#define NF_STRING_UTIL_H

#include <vector>
#include <string>
#include <regex>

// 字符串处理
class StringUtil
{
public:

	/// \breif    SplitString 字符串切割-C++ 11新特性
	static std::vector<std::string> SplitString(const std::string& str, const std::string& delim);

	/// \breif    Trim 去除str字符串中前后的指定字符
	static std::string Trim(const std::string& str, const char ch = ' ');

	/// \breif    Trim 去除str字符串中的指定字符
	static void TrimAll(std::string& str, const char ch = ' ');

	/// \brief	  字符串转换为64整数
	static int64_t StringAsInt64(const char* value, int64_t def_value = 0);

	/// \brief    字符串转换为整数
	static int StringAsInt(const char* value, int def_value = 0);

	/// \breif    字符串转换为浮点数
	static float StringAsFloat(const char* value, float def_value = 0.0f);

	/// \breif    字符串转换为双精度浮点数
	static double StringAsDouble(const char* value, double def_value = 0.0);

	/// \breif    其他类型转换为字符串
	template<typename T>
	static std::string TypeAsString(T&& value);

	/// \breif    StrReplace 把str字符串里的pat都换成replace
	static void StrReplace(std::string& str, const std::string& pat, const std::string& replace);
};

#endif// _STRINGUTIL_H
