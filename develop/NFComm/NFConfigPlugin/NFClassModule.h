///--------------------------------------------------------------------
/// 文件名:		NFClassModule.h
/// 内  容:		逻辑类管理模块
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_LOGICCLASS_MODULE_H
#define NF_LOGICCLASS_MODULE_H

#include <string>
#include <map>
#include <vector>
#include "NFElementModule.h"
#include "Dependencies/RapidXML/rapidxml.hpp"
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFList.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFRecord.h"
#include "NFComm/NFCore/NFPropertyManager.h"
#include "NFComm/NFCore/NFRecordManager.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

//引擎类型描述管理
class NFClassModule : public NFIClassModule
{
public:
    NFClassModule(NFIPluginManager* p);
    virtual ~NFClassModule();
	
	virtual bool Awake();
    virtual bool Init();
    virtual bool Shut();
    virtual bool Load();

    virtual NF_SHARE_PTR<NFIPropertyManager> GetClassPropertyManager(const std::string& strClassName);
    virtual NF_SHARE_PTR<NFIRecordManager> GetClassRecordManager(const std::string& strClassName);

	// 添加逻辑类
    virtual bool AddClass(const std::string& strClassName, const std::string& strParentName);

	// 添加事件
	virtual bool AddEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);
	// 移除事件
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner);
	virtual bool RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner);
	virtual bool RemoveEventCallBack(NFIModule* pOwner);
	// 查找事件
	virtual bool FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);
	// 执行事件
	virtual int RunEventCallBack(const std::string& strClassName, EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 执行内部消息回调
	virtual int RunCommandCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 执行客户端消息回调
	virtual int RunCustomCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const std::string& strMsg);
	/// \breif   RunExtraCallBack 执行附加服务器消息回调
	virtual int RunExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg);
	/// \breif   OpenEvent 开启事件
	virtual void OpenEvent();
	/// \breif   CloseEvent 关闭事件
	virtual void CloseEvent();
	/// \breif   IsOpenEvent 获取事件是否开启
	virtual bool IsOpenEvent();
	/// \breif   IsBaseOf 逻辑类继承关系判定
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName);
protected:
	virtual bool Load(rapidxml::xml_node<>* attrNode, NF_SHARE_PTR<NFIClass> pParentClass);

    virtual NFDATA_TYPE ComputerType(const char* pstrTypeName, NFData& var);
    virtual bool AddPropertys(rapidxml::xml_node<>* pPropertyRootNode, NF_SHARE_PTR<NFIClass> pClass);
    virtual bool AddRecords(rapidxml::xml_node<>* pRecordRootNode, NF_SHARE_PTR<NFIClass> pClass);
    virtual bool AddClassInclude(const char* pstrClassFilePath, NF_SHARE_PTR<NFIClass> pClass);
    virtual bool AddClass(const char* pstrClassFilePath, NF_SHARE_PTR<NFIClass> pClass);

	std::string GetNodeValue(rapidxml::xml_attribute<>* pAttr);

protected:
    NFIElementModule* m_pElementModule = nullptr;

    std::string m_strLogicCalssPath = "";

	// 事件是否开启
	bool m_bOpenEvent = true;
};

#endif
