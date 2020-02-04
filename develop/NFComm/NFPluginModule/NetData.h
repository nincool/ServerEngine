///--------------------------------------------------------------------
/// �ļ���:		NetData.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NET_DATA_H
#define NET_DATA_H
#include <string>

enum ENUM_NET_TYPE
{
	NET_TYPE_UNKNOW = 0,
	NET_TYPE_INNER = 1,		//��������
	NET_TYPE_OUTER = 2,		// ��������
	NET_TYPE_MAINTAIN = 3,	// ά������
	NET_TYPE_HTTP = 4,		// http����
	NET_TYPE_WLAN = 5,		// Wlan����
};

class NetData
{
public:
	NetData()
	{
		m_strIP = "";
		m_nPort = 0;
		m_nBufferSize = 0;
		m_nMaxConnect = 0;
	}

	std::string& GetIP() { return m_strIP; }
	void SetIP(std::string& value) { m_strIP = value; }

	int GetPort() const { return m_nPort; }
	void SetPort(int value) { m_nPort = value; }

	int GetBufferSize() const { return m_nBufferSize; }
	void SetBufferSize(int value) { m_nBufferSize = value; }

	int GetReadBufferSize() const { return m_nReadBufferSize; }
	void SetReadBufferSize(int value) { m_nReadBufferSize = value; }

	int GetMaxConnect() const { return m_nMaxConnect; }
	void SetMaxConnect(int value) { m_nMaxConnect = value; }

	ENUM_NET_TYPE GetNetType() const { return m_nNetType; }
	void SetNetType(ENUM_NET_TYPE value) { m_nNetType = value; }
private:
	// ��ַ
	std::string m_strIP = "";
	// �˿�
	int m_nPort = 0;
	// BUFF��С
	int m_nBufferSize = 0;
	int m_nReadBufferSize = 0;
	// ��������
	int m_nMaxConnect = 0;
	// ��������
	ENUM_NET_TYPE m_nNetType = NET_TYPE_UNKNOW;
};

#endif
