///--------------------------------------------------------------------
/// 文件名:		NFIClass.h
/// 内  容:		逻辑类接口
/// 说  明:		
/// 创建日期:	2019年8月26日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------


#ifndef __NF_I_CLASS_H__
#define __NF_I_CLASS_H__

#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFIPropertyManager.h"
#include "NFComm/NFCore/NFIRecordManager.h"
#include "NFComm/NFPluginModule/KConstDefine.h"

// 逻辑类
class NFIClass : public NFList<std::string>
{
public:
	virtual ~NFIClass() {};

	virtual NF_SHARE_PTR<NFIPropertyManager> GetPropertyManager() const = 0;
	virtual NF_SHARE_PTR<NFIRecordManager> GetRecordManager() const = 0;

	virtual void SetParent(NF_SHARE_PTR<NFIClass> pClass) = 0;
	virtual NF_SHARE_PTR<NFIClass> GetParent() const = 0;

	virtual void SetType(OuterMsg::ObjectType eType) = 0;
	virtual OuterMsg::ObjectType GetType() const = 0;

	virtual const std::string& GetClassName() const = 0;

	virtual const bool AddId(std::string& strId) = 0;
	virtual const bool RemoveId(std::string& strId) = 0;
	virtual const bool ClearId() = 0;
	virtual const std::vector<std::string>& GetIDList() const = 0;

	virtual void SetInstancePath(const std::string& strPath) = 0;
	virtual const std::string& GetInstancePath() const = 0;

	// 添加事件
	virtual bool AddEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;
	// 移除事件
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;
	virtual bool RemoveEventCallBack(EventType eType, NFIModule* pOwner) = 0;
	virtual bool RemoveEventCallBack(NFIModule* pOwner) = 0;
	// 查找事件
	virtual bool FindEventCallBack(EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;
	// 执行事件
	virtual int RunEventCallBack(EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;

	// 添加内部消息回调
	virtual bool AddCommandCallBack(int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;
	// 执行内部消息回调
	virtual int RunCommandCallBack(int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;
	// 添加客户端消息回调
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;
	// 执行客户端消息回调
	virtual int RunCustomCallBack(int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;
	// 添加附加服务器消息回调
	virtual bool AddExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;
	// 执行附加服务器消息回调
	virtual int RunExtraCallBack(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;
};

#endif // __NF_I_CLASS_H__
