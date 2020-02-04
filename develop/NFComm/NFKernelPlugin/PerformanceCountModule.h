///--------------------------------------------------------------------
/// �ļ���:		PerformanceCountModule.h
/// ��  ��      ����ͳ��
/// ˵  ��:		
/// ��������:	2019��12��13��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_PerformanceCountModule_H__
#define __H_PerformanceCountModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"

class NetServerMaintainModule;

struct PerformanceItem
{
	//����
	std::string name;
	//�ܻص�����
	__int64 total_num = 0;
	//�ܺ�ʱMS
	time_t total_tiem = 0;
	//���һ�κ�ʱMS
	time_t max_once_tiem = 0;
	//���һ�κ�ʱMS
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