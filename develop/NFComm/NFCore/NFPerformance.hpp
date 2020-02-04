///--------------------------------------------------------------------
/// �ļ���:		NFPerformance.cpp
/// ��  ��:		���ܸ���
/// ˵  ��:		
/// ��������:	2019.8.10
/// �޸���:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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