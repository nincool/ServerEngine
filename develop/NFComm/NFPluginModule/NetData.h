///--------------------------------------------------------------------
/// 文件名:		NetData.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NET_DATA_H
#define NET_DATA_H
#include <string>

enum ENUM_NET_TYPE
{
	NET_TYPE_UNKNOW = 0,
	NET_TYPE_INNER = 1,		//内网服务
	NET_TYPE_OUTER = 2,		// 外网服务
	NET_TYPE_MAINTAIN = 3,	// 维护服务
	NET_TYPE_HTTP = 4,		// http服务
	NET_TYPE_WLAN = 5,		// Wlan服务
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
	// 地址
	std::string m_strIP = "";
	// 端口
	int m_nPort = 0;
	// BUFF大小
	int m_nBufferSize = 0;
	int m_nReadBufferSize = 0;
	// 连接上线
	int m_nMaxConnect = 0;
	// 网络类型
	ENUM_NET_TYPE m_nNetType = NET_TYPE_UNKNOW;
};

#endif
