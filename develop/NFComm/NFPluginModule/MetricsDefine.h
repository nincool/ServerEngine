///--------------------------------------------------------------------
/// �ļ���:		MetricsDefine.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��12��26��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
