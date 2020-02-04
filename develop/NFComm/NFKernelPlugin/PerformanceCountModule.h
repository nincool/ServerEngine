///--------------------------------------------------------------------
/// 文件名:		PerformanceCountModule.h
/// 内  容      性能统计
/// 说  明:		
/// 创建日期:	2019年12月13日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_PerformanceCountModule_H__
#define __H_PerformanceCountModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"

class NetServerMaintainModule;

struct PerformanceItem
{
	//名字
	std::string name;
	//总回调次数
	__int64 total_num = 0;
	//总耗时MS
	time_t total_tiem = 0;
	//最大一次耗时MS
	time_t max_once_tiem = 0;
	//最近一次耗时MS
	time_t recent_once_tiem = 0;

	void ToString(std::string& out)
	{
		out.append("name:" + name + ", ");
		out.append("total_num:" + to_string(total_num) + ", ");
		out.append("total_tiem:" + to_string(total_tiem) + ", ");
		out.append("max_once_tiem:" + to_string(max_once_tiem) + ", ");
		out.append("recent_once_tiem:" + to_string(recent_once_tiem));
	}
};

class PerformanceCountModule : public NFIModule
{
public:
	PerformanceCountModule(NFIPluginManager* p);
	~PerformanceCountModule();

	virtual bool Init();

	virtual PerformanceItem* GetPerformanceItem(const std::string& name, const int id);
	virtual void Recording(const __int64 id, const std::string& name, const time_t consume);

	virtual void ToString(std::string& out);

	std::string ExportPerformanceCount(std::vector<std::string>& arg);

private:
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;

	std::map<std::string, std::map<__int64, PerformanceItem*>> mPerformanceItemMap;
};

#endif //__H_PerformanceCountModule_H__