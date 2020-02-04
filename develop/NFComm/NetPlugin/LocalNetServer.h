///--------------------------------------------------------------------
/// �ļ���:		LocalNetServer.h
/// ��  ��:		�����ڴ����������
/// ˵  ��:		
/// ��������:	2019��12��28��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_LocalNetServer_H__
#define __H_LocalNetServer_H__
#include <string>
#include <set>

class LocalNetServer
{
public:
	static void AddLocalServer(const std::string& ip, const int port);
	static bool IsLocalServer(const std::string& ip, const int port);

private:
	static std::set<std::string> mServerSet;
};

#endif //__H_LocalNetServer_H__