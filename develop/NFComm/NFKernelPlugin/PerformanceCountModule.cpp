///--------------------------------------------------------------------
/// 文件名:		PerformanceCountModule.cpp
/// 内  容      性能统计
/// 说  明:		
/// 创建日期:	2019年12月13日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "PerformanceCountModule.h"
#include "NFComm/NFUtils/StringUtil.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

PerformanceCountModule::PerformanceCountModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

PerformanceCountModule::~PerformanceCountModule()
{
	auto it = mPerformanceItemMap.begin();
	auto it_end = mPerformanceItemMap.end();
	for (; it != it_end; ++it)
	{
		auto it_sub = it->second.begin();
		auto it_end_sub = it->second.end();
		for (; it_sub != it_end_sub; ++it_sub)
		{
			delete it_sub->second;
		}
	}
	mPerformanceItemMap.clear();
}

bool PerformanceCountModule::Init()
{
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	m_pNetServerMaintainModule->AddCommandCallBack("export_performance", this, (MaintainCBFun)&PerformanceCountModule::ExportPerformanceCount);

	return true;
}

PerformanceItem* PerformanceCountModule::GetPerformanceItem(const std::string& name, const int id)
{
	auto it = mPerformanceItemMap.find(name);
	if (mPerformanceItemMap.end() == it)
	{
		return nullptr;
	}

	auto it_sub = it->second.find(id);
	if (it->second.end() == it_sub)
	{
		return nullptr;
	}

	return it_sub->second;
}

void PerformanceCountModule::Recording(const __int64 id, const std::string& name, const time_t consume)
{
	PerformanceItem* pPerformanceItem = GetPerformanceItem(name, id);
	if (nullptr == pPerformanceItem)
	{
		pPerformanceItem = new PerformanceItem();
		pPerformanceItem->name = name;
		mPerformanceItemMap[name].insert(std::make_pair(id, pPerformanceItem));
	}

	pPerformanceItem->total_num += 1;
	pPerformanceItem->recent_once_tiem = consume;
	pPerformanceItem->total_tiem += consume;
	if (pPerformanceItem->max_once_tiem < consume)
	{
		pPerformanceItem->max_once_tiem = consume;
	}
}

void PerformanceCountModule::ToString(std::string& out)
{
	auto it = mPerformanceItemMap.begin();
	auto it_end = mPerformanceItemMap.end();
	for (; it != it_end; ++it)
	{
		auto it_sub = it->second.begin();
		auto it_end_sub = it->second.end();
		for (; it_sub != it_end_sub; ++it_sub)
		{
			out.append("id:" + to_string(it_sub->first) + ",");
			it_sub->second->ToString(out);
			out.append("\n");
		}
	}

}

std::string PerformanceCountModule::ExportPerformanceCount(std::vector<std::string>& arg)
{
	std::string out;
	ToString(out);

	std::ofstream outFile;
	if (arg.size() == 2)
	{
		outFile.open(arg[1], std::ios::trunc);
	}
	else
	{
		outFile.open("ExportPerformanceCount.txt", std::ios::trunc);
	}

	outFile << out;
	outFile.close();

	return "out file ok.";
}