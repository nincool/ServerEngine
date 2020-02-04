///--------------------------------------------------------------------
/// 文件名:		IRoomKernel.h
/// 内  容:		房间服核心模块接口
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef I_ROOM_KERNEL_H
#define I_ROOM_KERNEL_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFIKernelModule.h"

#define IROOM_LOGIC_VERSION 0x0001

class IRoomKernel : public NFIModule
{
public:
	virtual ~IRoomKernel() {};

	// 获取共用核心
	virtual NFIKernelModule* CKernel() = 0;

	// 发送消息给平台服
	virtual bool SendCommandToPlatform(int nCommandId, const NFGUID& sendobj, const NFDataList& args) = 0;

	// 发送消息给平台服辅助对象
	virtual bool SendCommandToPlatformBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args) = 0;
};

#endif
