///--------------------------------------------------------------------
/// 文件名:		ExtraServerModule.h
/// 内  容:		附加服务器逻辑
/// 说  明:		
/// 创建日期:	2019年9月19日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_EXTRA_SERVER_MODULE_H__
#define __H_EXTRA_SERVER_MODULE_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CExtraServerModule : public NFIModule
{
public:
	CExtraServerModule(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();

	virtual bool AfterInit();

private:
	//接收到世界服转发来消息
	void OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInside = nullptr;
};

#endif //__H_EXTRA_SERVER_H__