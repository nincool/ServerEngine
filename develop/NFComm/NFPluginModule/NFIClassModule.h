///--------------------------------------------------------------------
/// 文件名:		NFIClassModule.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _NF_I_CLASS_MODULE_H_
#define _NF_I_CLASS_MODULE_H_

#include "NFIModule.h"
#include "NFIClass.h"
#include "KConstDefine.h"

//逻辑类管理
class NFIClassModule 
	: public NFIModule
	, public NFMapEx<std::string, NFIClass>
{
public:
    virtual ~NFIClassModule() {}

    virtual NF_SHARE_PTR<NFIPropertyManager> GetClassPropertyManager(const std::string& strClassName) = 0;
    virtual NF_SHARE_PTR<NFIRecordManager> GetClassRecordManager(const std::string& strClassName) = 0;

	/// \breif   AddEventCallBack 添加事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   NFIModule * pOwner 事件回调对象
	/// \param   EVENT_HOOK pHook 事件回调
	/// \param   int nPrior 优先级
	/// \return  bool 是否成功
	virtual bool AddEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;

	/// \breif   RemoveEventCallBack 移除事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   void * pOwner 事件回调对象
	/// \param   EVENT_HOOK pHook 事件回调
	/// \return  bool 是否成功
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;

	/// \breif   RemoveEventCallBack 移除事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   void * pOwner 事件回调对象
	/// \return  bool 是否成功
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner) = 0;
	/// \breif   RemoveEventCallBack 移除事件对象的所有事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   void * pOwner 事件回调对象
	/// \return  bool 是否成功
	virtual bool RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner) = 0;
	/// \breif   RemoveEventCallBack 移除事件对象的所有事件
	/// \Access  virtual public 
	/// \param   void * pOwner 事件回调对象
	/// \return  bool
	virtual bool RemoveEventCallBack(NFIModule* pOwner) = 0;
	
	/// \breif   FindEventCallBack 查找事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   void * pOwner 事件回调对象
	/// \param   EVENT_HOOK pHook 事件回调
	/// \return  bool 是否成功
	virtual bool FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook) = 0;

	/// \breif   RunEventCallBack 执行事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   const NFGUID & self 响应事件对象
	/// \param   const NFGUID & sender 响应事件的相关对象
	/// \param   const NFDataList & args 参数
	/// \return  int 返回0 成功 其他值参照具体事件定义
	virtual int RunEventCallBack(const std::string& strClassName, EventType eType, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;	
	
	/// \breif   RunCommandCallBack 执行内部消息回调
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   int nMsgId 消息号
	/// \param   const NFGUID & self 接收消息对象
	/// \param   const NFGUID & sender 发送消息对象
	/// \param   const NFDataList & args 参数
	/// \return  int 
	virtual int RunCommandCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const NFGUID& sender, const NFDataList& args) = 0;

	/// \breif   RunCustomCallBack 执行客户端消息回调
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   int nMsgId 消息号
	/// \param   const NFGUID & self 接收消息对象
	/// \param   const NFGUID & sender 发送消息对象
	/// \param   const std::string& strMsg 消息PB数据流(不包含消息头) 
	/// \return  int 
	virtual int RunCustomCallBack(const std::string& strClassName, int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;

	/// \breif   RunExtraCallBack 执行附加服务器消息回调
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   NF_SERVER_TYPES eServerType
	/// \param   int nMsgId
	/// \param   const NFGUID & self
	/// \param   const std::string & strMsg
	/// \return  int
	virtual int RunExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const std::string& strMsg) = 0;

	/// \breif   OpenEvent 开启事件
	virtual void OpenEvent() = 0;
	/// \breif   CloseEvent 关闭事件
	virtual void CloseEvent() = 0;
	/// \breif   IsOpenEvent 获取事件是否开启
	virtual bool IsOpenEvent() = 0;

	/// \breif   IsBaseOf 逻辑类继承关系判定
	/// \Access  virtual public 
	/// \param   const std::string & strBaseClassName 基类名
	/// \param   const std::string & strClassName 派生类名
	/// \return  bool
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName) = 0;
};

#endif // _NF_I_CLASS_MODULE_H_