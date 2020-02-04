///---------------------------------------------------------
/// 文件名:   FuncUtil.cpp
/// 内 容:    函数工具
/// 说 明:       
/// 创建日期: 2019/08/10
/// 创建人:   孙建
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------
#include "FuncUtil.h"
#include <math.h>
#include <stdlib.h>
#include <vcruntime_string.h>
#include <float.h>

#define RAND_MAX1 (RAND_MAX + 1)
#define FLOAT_EPSILON 1.192092896e-07F

/// \breif   返回0-(scale-1)的随机整数
int util_random_int(int scale)
{
	if ((scale <= 1) || (scale > RAND_MAX))
	{
		return 0;
	}

	int r = ::rand();
	int ret = r % scale;
	return ret;
}

/// \breif    util_random_int_in_range
int util_random_int_in_range(int min, int max)
{
	if (min == max)
	{
		return min;
	}

	if (min > max)
	{
		int temp = min;
		min = max;
		max = temp;
	}

	int offset = max - min + 1;
	if (offset > RAND_MAX)
	{
		offset = RAND_MAX;
	}

	int randomInt = ::rand() % offset;
	return min + randomInt;
}

/// \breif   返回(0-scale)的随机浮点数
float util_random_float(float scale)
{
	static const float TMP = 1.0f / float(RAND_MAX1);

	if (scale <= 0.0f)
	{
		return 0.0f;
	}

	return (float(::rand()) * TMP * scale);
}

/// \breif  获得一个高精度的随机数(0.0-1.0)
double util_random_double()
{
	return (double(::rand()) / double(RAND_MAX1) + double(::rand())) / double(RAND_MAX1);
}

/// \brief 打乱
void util_rand_shuffle(std::vector<int>& vecInt)
{
	for (int i = 0; i < vecInt.size(); i++)
	{
		int j = util_random_int((vecInt.size() - i));
		std::swap(vecInt.at(i), vecInt.at(j));
	}
}

/// \breif  util_rand_seed 自定义随机种子的随机函数(非线程安全)
int UtilRandSeed::util_rand_seed(int scale)
{
	if ((scale <= 1) || (scale > RAND_MAX))
	{
		return 0;
	}

	return(((seed = seed * 214013L
		+ 2531011L) >> 16) & 0x7fff) % scale;
}

/// \breif  自定义随机种子的随机范围函数
int64_t UtilRandSeed::util_rand_seed_in_range(int64_t min, int64_t max)
{
	if (min == max)
	{
		return min;
	}

	if (min > max)
	{
		int64_t temp = min;
		min = max;
		max = temp;
	}

	int64_t offset = max - min + 1;
	if (offset > RAND_MAX)
	{
		offset = RAND_MAX;
	}

	int64_t randomInt = util_rand_seed(offset);
	return min + randomInt;
}

/// \breif util_rand_seed_shuffle 自定义随机种子的随机打乱顺序
void UtilRandSeed::util_rand_seed_shuffle(std::vector<int>& vecInt)
{
	for (int i = 0; i < vecInt.size(); i++)
	{
		int j = util_rand_seed((vecInt.size() - i));
		std::swap(vecInt.at(i), vecInt.at(j));
	}
}

/// \breif util_rand_seed_get 自定义随机种子的随机scale中获得get个
int UtilRandSeed::util_rand_seed_get(int scale, int get, std::vector<int>& vecGet)
{
	if (get < 1 || scale <= 1)
	{
		return 0;
	}

	// 只有一个时
	if (1 == get)
	{
		vecGet.push_back(util_rand_seed(scale));
		return 1;
	}
	
	std::vector<int> vecInt;
	for (int i = 0; i < scale; ++i)
	{
		vecInt.push_back(i);
	}

	if (get >= scale)
	{
		vecGet = vecInt;
	}
	else
	{
		util_rand_seed_shuffle(vecInt);
		vecGet.assign(vecInt.begin(), vecInt.begin() + get);
	}

	return vecGet.size();
}

/// \breif   调整两个数的大小后返回
void util_sort_int(int& minvalue, int& maxvalue)
{
	if (minvalue > maxvalue)
	{
		int tmpvalue = minvalue;
		minvalue = maxvalue;
		maxvalue = tmpvalue;
	}
}

/// \breif    util_sort_float 
void util_sort_float(float& minvalue, float& maxvalue)
{
	if (minvalue > maxvalue)
	{
		float tmpvalue = minvalue;
		minvalue = maxvalue;
		maxvalue = tmpvalue;
	}
}

/// \brief  判断两个浮点数是否可认为相等
bool util_float_equal(float f1, float f2)
{
	if (memcmp(&f1, &f2, sizeof(f1)) == 0)
	{
		return true;
	}

	return (f1 < (f2 + FLOAT_EPSILON)) && (f1 > (f2 - FLOAT_EPSILON));
}

/// \brief  双精度浮点数相等
bool util_double_equal(double v1, double v2)
{
	return (v1 <= (v2 + DBL_EPSILON)) && (v1 >= (v2 - DBL_EPSILON));
}

/// \brief util_float_equal_zero
bool util_float_equal_zero(float f)
{
	return (f >= -FLT_EPSILON && f <= FLT_EPSILON);
}

/// \brief 字符串类型的整数类型转换为整型
int convert_int(const char* szint, int default_int)
{
	if (szint == NULL || *szint == '\0')
	{
		return default_int;
	}

	return ::atoi(szint);
}

/// \brief 字符串类型的浮点型转换为浮点型
float convert_float(const char* szfloat, float default_float)
{
	if (szfloat == NULL || *szfloat == '\0')
	{
		return default_float;
	}

	return (float)::atof(szfloat);
}
