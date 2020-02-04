///--------------------------------------------------------------------
/// �ļ���:		LocalNetServer.cpp
/// ��  ��:		�����ڴ����������
/// ˵  ��:		
/// ��������:	2019��12��28��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "LocalNetServer.h"

std::set<std::string> LocalNetServer::mServerSet;

void LocalNetServer::AddLocalServer(const std::string& ip, const int port)
{
	std::string key = ip + ":" + std::to_string(port);
	mServerSet.insert(key);
}

bool LocalNetServer::IsLocalServer(const std::string& ip, const int port)
{
	std::string key = ip + ":" + std::to_string(port);

	return mServerSet.find(key) != mServerSet.end();
}