///--------------------------------------------------------------------
/// �ļ���:		ConnectData.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��11��4��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef CONNECT_DATA_H
#define CONNECT_DATA_H
#include "KConstDefine.h"

//����״̬
enum class ConnectState
{
	DISCONNECT,		  //�Ͽ�
	CONNECTING,		  //������
	NORMAL,			  //����
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
