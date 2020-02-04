///--------------------------------------------------------------------
/// 文件名:		ConnectData.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年11月4日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef CONNECT_DATA_H
#define CONNECT_DATA_H
#include "KConstDefine.h"

//连接状态
enum class ConnectState
{
	DISCONNECT,		  //断开
	CONNECTING,		  //连接中
	NORMAL,			  //正常
};

class ConnectCfg
{
public:
	ConnectCfg()
	{
		eServerType = NF_ST_NONE;
		nPort = 0;
		strIP = "";
		uNetBufferMax = 1024 * 1024;
		isWeakLink = false;
	}

	NF_SERVER_TYPES GetServerType() const { return eServerType; }
	void SetServerType(NF_SERVER_TYPES value) { eServerType = value; }

	const std::string& GetIP() const { return strIP; }
	void SetIP(std::string& value) { strIP = value; }

	int GetPort() const { return nPort; }
	void SetPort(int value) { nPort = value; }

	int GetBufferSize() const { return uNetBufferMax;}
	void SetBufferSize(int value) { uNetBufferMax = value; }

	bool GetIsWeakLink() const { return isWeakLink; }
	void SetIsWeakLink(bool v) { isWeakLink = v; }
private:
	NF_SERVER_TYPES eServerType;
	std::string strIP;
	int nPort;
	size_t uNetBufferMax;
	bool isWeakLink;
};

#endif
