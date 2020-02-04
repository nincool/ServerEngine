///--------------------------------------------------------------------
/// 文件名:		IPubKernel.h
/// 内  容:		公共域核心模块接口
/// 说  明:		
/// 创建日期:	2019年8月26日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef I_PUB_KERNEL_H
#define I_PUB_KERNEL_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFIKernelModule.h"

#define IPUB_LOGIC_VERSION 0x0001

class IPubKernel : public NFIModule
{
public:
	virtual ~IPubKernel() {};

	// 获取共用核心
	virtual NFIKernelModule* CKernel() = 0;

	// 创建域名
	virtual NF_SHARE_PTR<NFIObject> CreatePub(const std::string& pub_name, bool bSave = false) = 0;
	// 创建公会
	virtual NF_SHARE_PTR<NFIObject> CreateGuild(const std::string& guild_name) = 0;
	// 删除公共域
	virtual bool DeletePub(const std::string& pub_name) = 0;
	// 删除公会
	virtual bool DeleteGuild(const std::string& guild_name) = 0;
	// 获得公共GUID
	virtual const NFGUID& GetPubGuid(const std::string& name) = 0;
	// 获得公会GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name) = 0;
	// 手动保存域数据
	virtual bool SavePubData(const std::string& name, const NFGUID& object_id) = 0;
	// 手动保存公会数据
	virtual bool SaveGuildData(const NFGUID& object_id) = 0;
	// Command事件给玩家
	virtual bool CommandToPlayer(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg) = 0;
	// Command事件给Game辅助对象
	virtual bool CommandToGame(int nCommandId, const NFGUID& sendobj, const NFDataList& msg) = 0;
	// Command事件给内部对象
	virtual bool Command(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg) = 0;
	// Command事件给辅助对象
	virtual bool CommandToHelper(int nCommandId, const NFGUID& sendobj, const NFDataList& msg) = 0;
};

#endif
