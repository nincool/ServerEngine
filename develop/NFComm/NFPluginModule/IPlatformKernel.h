///--------------------------------------------------------------------
/// 文件名:		IPlatformKernel.h
/// 内  容:		平台服核心模块接口
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef I_PLATFORM_KERNEL_H
#define I_PLATFORM_KERNEL_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFIKernelModule.h"

#define IPLATFORM_LOGIC_VERSION 0x0001

class IPlatformKernel : public NFIModule
{
public:
	virtual ~IPlatformKernel() {};

	// 获取共用核心
	virtual NFIKernelModule* CKernel() = 0;

	// 获得主辅助对象
	virtual NF_SHARE_PTR<NFIObject> GetPlatformHelper() = 0;

	// 保存平台服数据
	virtual bool SavePlatformData(const NFGUID& object_id) = 0;

	// 删除平台服对象
	virtual bool DeletePlatformData(const NFGUID& object_id) = 0;

	// 开启战场
	virtual NF_SHARE_PTR<NFIObject> OnStartBattle(const std::string& sBattleName, const NFDataList& args) = 0;

	// 发送消息给游戏服GUID
	virtual bool SendCommandToGameGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args) = 0;

	// 发送消息给Game
	virtual bool SendCommandToGame(int nCommandId, int world_id, const NFGUID& sendobj, const NFDataList& args) = 0;

	// 发送消息给所有Game
	virtual bool SendCommandToAllGame(int nCommandId, const NFGUID& sendobj, const NFDataList& args) = 0;

	// 发送消息给战场服
	virtual bool SendCommandToBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& battle_guid, const NFDataList& args) = 0;
};

#endif
