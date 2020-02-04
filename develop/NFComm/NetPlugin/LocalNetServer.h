///--------------------------------------------------------------------
/// 文件名:		LocalNetServer.h
/// 内  容:		本地内存服务器对象
/// 说  明:		
/// 创建日期:	2019年12月28日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
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