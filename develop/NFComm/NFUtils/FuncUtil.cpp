///---------------------------------------------------------
/// �ļ���:   FuncUtil.cpp
/// �� ��:    ��������
/// ˵ ��:       
/// ��������: 2019/08/10
/// ������:   �ｨ
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------
#include "FuncUtil.h"
#include <math.h>
#include <stdlib.h>
#include <vcruntime_string.h>
#include <float.h>

#define RAND_MAX1 (RAND_MAX + 1)
#define FLOAT_EPSILON 1.192092896e-07F

/// \breif   ����0-(scale-1)���������
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

/// \breif   ����(0-scale)�����������
float util_random_float(float scale)
{
	static const float TMP = 1.0f / float(RAND_MAX1);

	if (scale <= 0.0f)
	{
		return 0.0f;
	}

	return (float(::rand()) * TMP * scale);
}

/// \breif  ���һ���߾��ȵ������(0.0-1.0)
double util_random_double()
{
	return (double(::rand()) / double(RAND_MAX1) + double(::rand())) / double(RAND_MAX1);
}

/// \brief ����
void util_rand_shuffle(std::vector<int>& vecInt)
{
	for (int i = 0; i < vecInt.size(); i++)
	{
		int j = util_random_int((vecInt.size() - i));
		std::swap(vecInt.at(i), vecInt.at(j));
	}
}

/// \breif  util_rand_seed �Զ���������ӵ��������(���̰߳�ȫ)
int UtilRandSeed::util_rand_seed(int scale)
{
	if ((scale <= 1) || (scale > RAND_MAX))
	{
		return 0;
	}

	return(((seed = seed * 214013L
		+ 2531011L) >> 16) & 0x7fff) % scale;
}

/// \breif  �Զ���������ӵ������Χ����
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

/// \breif util_rand_seed_shuffle �Զ���������ӵ��������˳��
void UtilRandSeed::util_rand_seed_shuffle(std::vector<int>& vecInt)
{
	for (int i = 0; i < vecInt.size(); i++)
	{
		int j = util_rand_seed((vecInt.size() - i));
		std::swap(vecInt.at(i), vecInt.at(j));
	}
}

/// \breif util_rand_seed_get �Զ���������ӵ����scale�л��get��
int UtilRandSeed::util_rand_seed_get(int scale, int get, std::vector<int>& vecGet)
{
	if (get < 1 || scale <= 1)
	{
		return 0;
	}

	// ֻ��һ��ʱ
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

/// \breif   �����������Ĵ�С�󷵻�
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

/// \brief  �ж������������Ƿ����Ϊ���
bool util_float_equal(float f1, float f2)
{
	if (memcmp(&f1, &f2, sizeof(f1)) == 0)
	{
		return true;
	}

	return (f1 < (f2 + FLOAT_EPSILON)) && (f1 > (f2 - FLOAT_EPSILON));
}

/// \brief  ˫���ȸ��������
bool util_double_equal(double v1, double v2)
{
	return (v1 <= (v2 + DBL_EPSILON)) && (v1 >= (v2 - DBL_EPSILON));
}

/// \brief util_float_equal_zero
bool util_float_equal_zero(float f)
{
	return (f >= -FLT_EPSILON && f <= FLT_EPSILON);
}

/// \brief �ַ������͵���������ת��Ϊ����
int convert_int(const char* szint, int default_int)
{
	if (szint == NULL || *szint == '\0')
	{
		return default_int;
	}

	return ::atoi(szint);
}

/// \brief �ַ������͵ĸ�����ת��Ϊ������
float convert_float(const char* szfloat, float default_float)
{
	if (szfloat == NULL || *szfloat == '\0')
	{
		return default_float;
	}

	return (float)::atof(szfloat);
}
