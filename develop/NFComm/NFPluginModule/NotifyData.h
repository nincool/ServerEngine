///--------------------------------------------------------------------
/// �ļ���:		NotifyData.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��11��14��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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
	// ��ַ
	std::string m_strIP = "";
	// �˿�
	int m_nPort = 0;
	// ����˿�
	int m_nWebPort = 0;
	// BUFF��С
	std::string m_strName = "";
};

#endif
