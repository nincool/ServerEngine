///---------------------------------------------------------
/// �ļ���:   FuncUtil.h
/// �� ��:    ��������
/// ˵ ��:       
/// ��������: 2019/08/10
/// ������:   �ｨ
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------

#ifndef _GAME_UTIL_FUNC_H
#define _GAME_UTIL_FUNC_H
#include <vector>

/// \breif    util_random_int ����[0, (scale-1)]���������
/// \Access   public  
/// \param    int scale 
/// \return   int
int util_random_int(int scale);

/// \breif    util_random_int_in_range ����[min, max]��Χ�������
/// \Access   public  
/// \param    int min 
/// \param    int max 
/// \return   int
int util_random_int_in_range(int min, int max);

/// \breif    util_random_float ����(0-scale)�����������
/// \Access   public  
/// \param    float scale 
/// \return   float
float util_random_float(float scale);

/// \breif    util_random_double ���һ���߾��ȵ������(0.0-1.0)
/// \Access   public  
/// \return   double
double util_random_double();

/// \breif	  util_rand_shuffle �������˳��
/// \Access   public 
/// \param    std::vector<int>& vecInt
/// \return   void
void util_rand_shuffle(std::vector<int>& vecInt);

/// \breif    util_sort_int �����������Ĵ�С�󷵻�
/// \Access   public  
/// \param    int & minvalue 
/// \param    int & maxvalue 
/// \return   void
void util_sort_int(int& minvalue, int& maxvalue);

/// \breif    util_sort_float 
/// \Access   public  
/// \param    float & minvalue 
/// \param    float & maxvalue 
/// \return   void
void util_sort_float(float& minvalue, float& maxvalue);

/// \breif    util_float_equal �Ƚ������������Ƿ����Ϊ���
/// \Access   public  
/// \param    float f1 
/// \param    float f2 
/// \return   bool
bool util_float_equal(float f1, float f2);

/// \breif   util_double_equal ˫���ȸ��������
/// \Access  public 
/// \param   double v1
/// \param   double v2
/// \return  bool
bool util_double_equal(double v1, double v2);

/// \breif    util_float_equal_zero 
/// \Access   public  
/// \param    float f 
/// \return   bool
bool util_float_equal_zero(float f);

/// \breif    convert_int �ַ������͵���������ת��Ϊ����
/// \Access   public  
/// \param    const char * szint 
/// \param    int default_int 
/// \return   int
int convert_int(const char* szint, int default_int = 0);

/// \breif    convert_float �ַ������͵ĸ�����ת��Ϊ������
/// \Access   public  
/// \param    const char * szfloat 
/// \param    float default_float 
/// \return   float
float convert_float(const char* szfloat, float default_float = 0.0f);

// �Զ������������
class UtilRandSeed
{
public:
	UtilRandSeed(unsigned int n) : seed(n) {}

	/// \breif    util_rand_seed ���������(���̰߳�ȫ)
	/// \Access   public 
	/// \param    int scale 
	/// \return   int
	int util_rand_seed(int scale);

	/// \breif	  util_rand_seed_in_range �Զ���������ӵ������Χ����[min, max]
	/// \Access   public 
	/// \param    int64_t min 
	/// \param    int64_t max 
	/// \return   int64_t
	int64_t util_rand_seed_in_range(int64_t min, int64_t max);

	/// \breif	  util_rand_seed_shuffle �Զ���������ӵ��������˳��
	/// \Access   public 
	/// \param    std::vector<int>& vecInt
	/// \param    unsigned int& seed
	/// \return   void
	void util_rand_seed_shuffle(std::vector<int>& vecInt);

	/// \breif	  util_rand_seed_get �Զ���������ӵ����scale�л��get��
	/// \Access   public 
	/// \param    int scale
	/// \param    int get
	/// \param    std::vector<int>& vecGet
	/// \return   int
	int util_rand_seed_get(int scale, int get, std::vector<int>& vecGet);
private:
	unsigned int seed = 0;
};

/// \breif    Invfast_sqrt ���ټ�����ŷ�֮1
/// \Access   public  
/// \param    float x 
/// \return   float
inline float Invfast_sqrt(float x)
{
	float xhalf = 0.5f * x;

	int i = *(int *)&x;             // ��������BITǿ�и���������

	i = 0x5f375a86 - (i >> 1);      // Quake-III Arena (����֮��3)�ĵײ����

	x = *(float *)&i;               // convert bits back to float

	x = x * (1.5f - xhalf * x * x); // ţ�ٵ�����Newton step, repeating increases accuracy

	return x;
}

/// \breif    fast_sqrt 
/// \Access   public  
/// \param    const float x 
/// \return   float
inline float fast_sqrt(const float x)
{
	return x * Invfast_sqrt(x);
}

/// \breif    fast_fabs 
/// \Access   public  
/// \param    const float num 
/// \return   float
inline float fast_fabs(const float num)
{
	unsigned int ret = *((unsigned int*)&num);

	ret = ret << 1;
	ret = ret >> 1;

	return *((float*)&ret);
}

#if 0
/// \breif    fast_abs 
/// \Access   public  
/// \param    const int num 
/// \return   int
inline int fast_abs(const int num)
{
	int ret = num;
	int tmp = num;

	tmp = tmp >> 31;
	ret = ret ^ tmp;
	ret = ret - tmp;

	return ret;
}
#endif

#endif // _GAME_UTIL_FUNC_H

