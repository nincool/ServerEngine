///--------------------------------------------------------------------
/// 文件名:		NFPerformance.cpp
/// 内  容:		性能辅助
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_PERFORMANCE_H
#define NF_PERFORMANCE_H

#include <iostream>
#include <time.h>
#include "../NFPluginModule/NFPlatform.h"

class NFPerformance
{
private:
	int64_t time = 0;
	int64_t checkTime = 0;

public:
	NFPerformance()
	{
		time = NFGetTimeMS();
	}

	bool CheckTimePoint(const int milliSecond = 1)
	{
		checkTime = NFGetTimeMS();
		if (checkTime > milliSecond + time)
		{
			return true;
		}

		return false;
	}

	int64_t TimeScope()
	{
		return checkTime - time;
	}
};

#endif