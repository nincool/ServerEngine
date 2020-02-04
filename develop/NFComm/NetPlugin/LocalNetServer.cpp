///--------------------------------------------------------------------
/// 文件名:		LocalNetServer.cpp
/// 内  容:		本地内存服务器对象
/// 说  明:		
/// 创建日期:	2019年12月28日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
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