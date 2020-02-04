///--------------------------------------------------------------------
/// 文件名:		NetDefine.h
/// 内  容      网络相关定义
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetDefine_H__
#define __H_NetDefine_H__

//网络连接事件
enum class NET_EVENT
{
	NET_EVENT_READING = 0x01,	/**< error encountered while reading */
	NET_EVENT_WRITING = 0x02,	/**< error encountered while writing */
	NET_EVENT_EOF = 0x10,		//连接关闭
	NET_EVENT_ERROR = 0x20,		//连接出错
	NET_EVENT_TIMEOUT = 0x40,	//读取超时
	NET_EVENT_CONNECTED = 0x80,	//连接成功
};

#endif //__H_NetDefine_H__