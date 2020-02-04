///--------------------------------------------------------------------
/// 文件名:		NotifyData.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年11月14日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NOTIFY_DATA_H
#define NOTIFY_DATA_H
#include <string>

class NotifyData
{
public:
	NotifyData()
	{
		m_strIP = "";
		m_nPort = 0;
		m_nWebPort = 0;
		m_strName = "";
	}

	std::string& GetIP() { return m_strIP; }
	void SetIP(std::string& value) { m_strIP = value; }

	int GetPort() const { return m_nPort; }
	void SetPort(int value) { m_nPort = value; }

	int GetWebPort()const { return m_nWebPort; }
	void SetWebPort(int value) { m_nWebPort = value; }

	std::string& GetName() { return m_strName; }
	void SetName(std::string& value) { m_strName = value; }
private:
	// 地址
	std::string m_strIP = "";
	// 端口
	int m_nPort = 0;
	// 网络端口
	int m_nWebPort = 0;
	// BUFF大小
	std::string m_strName = "";
};

#endif
