///--------------------------------------------------------------------
/// 文件名:		RoomKernel.h
/// 内  容:		房间服核心模块
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __ROOM_KERNEL_H
#define __ROOM_KERNEL_H
#include "NFComm/NFPluginModule/IRoomKernel.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CRoomKernel : public IRoomKernel
{
public:
	CRoomKernel(NFIPluginManager* p);
    virtual ~CRoomKernel();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();
	
	// 获取共用核心
	virtual NFIKernelModule* CKernel();
	
	// 发送消息给平台服
	virtual bool SendCommandToPlatform(int nCommandId, const NFGUID& sendobj, const NFDataList& args);

	// 发送消息给平台服辅助对象
	virtual bool SendCommandToPlatformBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args);

private:
	//通用内核模块
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
};

#endif
