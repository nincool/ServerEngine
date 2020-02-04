///--------------------------------------------------------------------
/// 文件名:		MetricsDefine.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年12月26日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef METRICS_DEFINE_H
#define METRICS_DEFINE_H
#include <string>
#include <map>
#include <vector>

class Metrics
{
public:
	void SetType(int value) { nType = value; }
	int GetType() { return nType; }

	void SetName(std::string& value) { strName = value; }
	std::string& GetName() { return strName; }

	void SetHelp(std::string& value) { strHelp = value; }
	std::string& GetHelp() { return strHelp; }

	void SetLables(std::map<std::string, std::string>& value) { mapLables = value; }
	std::map<std::string, std::string>& GetLables() { return mapLables; }

	void SetQuantiles(std::map<double, double>& value) { vecQuantiles = value; }
	std::map<double, double>& GetQuantiles() { return vecQuantiles; }

	void SetBuckets(std::vector<double>& value) { vecBuckets = value; }
	std::vector<double>& GetBuckets() { return vecBuckets; }
private:
	int nType = -1;
	std::string strName = "";
	std::string strHelp = "";
	std::map<std::string, std::string> mapLables;
	std::map<double, double> vecQuantiles;
	std::vector<double> vecBuckets;
};

#endif
