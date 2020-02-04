///---------------------------------------------------------
/// 文件名:   FuncUtil.h
/// 内 容:    函数工具
/// 说 明:       
/// 创建日期: 2019/08/10
/// 创建人:   孙建
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------

#ifndef _GAME_UTIL_FUNC_H
#define _GAME_UTIL_FUNC_H
#include <vector>

/// \breif    util_random_int 返回[0, (scale-1)]的随机整数
/// \Access   public  
/// \param    int scale 
/// \return   int
int util_random_int(int scale);

/// \breif    util_random_int_in_range 返回[min, max]范围随机整数
/// \Access   public  
/// \param    int min 
/// \param    int max 
/// \return   int
int util_random_int_in_range(int min, int max);

/// \breif    util_random_float 返回(0-scale)的随机浮点数
/// \Access   public  
/// \param    float scale 
/// \return   float
float util_random_float(float scale);

/// \breif    util_random_double 获得一个高精度的随机数(0.0-1.0)
/// \Access   public  
/// \return   double
double util_random_double();

/// \breif	  util_rand_shuffle 随机打乱顺序
/// \Access   public 
/// \param    std::vector<int>& vecInt
/// \return   void
void util_rand_shuffle(std::vector<int>& vecInt);

/// \breif    util_sort_int 调整两个数的大小后返回
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

/// \breif    util_float_equal 比较两个浮点数是否可认为相等
/// \Access   public  
/// \param    float f1 
/// \param    float f2 
/// \return   bool
bool util_float_equal(float f1, float f2);

/// \breif   util_double_equal 双精度浮点数相等
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

/// \breif    convert_int 字符串类型的整数类型转换为整型
/// \Access   public  
/// \param    const char * szint 
/// \param    int default_int 
/// \return   int
int convert_int(const char* szint, int default_int = 0);

/// \breif    convert_float 字符串类型的浮点型转换为浮点型
/// \Access   public  
/// \param    const char * szfloat 
/// \param    float default_float 
/// \return   float
float convert_float(const char* szfloat, float default_float = 0.0f);

// 自定义随机种子类
class UtilRandSeed
{
public:
	UtilRandSeed(unsigned int n) : seed(n) {}

	/// \breif    util_rand_seed 的随机函数(非线程安全)
	/// \Access   public 
	/// \param    int scale 
	/// \return   int
	int util_rand_seed(int scale);

	/// \breif	  util_rand_seed_in_range 自定义随机种子的随机范围函数[min, max]
	/// \Access   public 
	/// \param    int64_t min 
	/// \param    int64_t max 
	/// \return   int64_t
	int64_t util_rand_seed_in_range(int64_t min, int64_t max);

	/// \breif	  util_rand_seed_shuffle 自定义随机种子的随机打乱顺序
	/// \Access   public 
	/// \param    std::vector<int>& vecInt
	/// \param    unsigned int& seed
	/// \return   void
	void util_rand_seed_shuffle(std::vector<int>& vecInt);

	/// \breif	  util_rand_seed_get 自定义随机种子的随机scale中获得get个
	/// \Access   public 
	/// \param    int scale
	/// \param    int get
	/// \param    std::vector<int>& vecGet
	/// \return   int
	int util_rand_seed_get(int scale, int get, std::vector<int>& vecGet);
private:
	unsigned int seed = 0;
};

/// \breif    Invfast_sqrt 快速计算根号分之1
/// \Access   public  
/// \param    float x 
/// \return   float
inline float Invfast_sqrt(float x)
{
	float xhalf = 0.5f * x;

	int i = *(int *)&x;             // 浮点数按BIT强行赋给长整形

	i = 0x5f375a86 - (i >> 1);      // Quake-III Arena (雷神之锤3)的底层代码

	x = *(float *)&i;               // convert bits back to float

	x = x * (1.5f - xhalf * x * x); // 牛顿叠代法Newton step, repeating increases accuracy

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

