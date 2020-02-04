///--------------------------------------------------------------------
/// 文件名:		NFIKernelModule.h
/// 内  容:		核心模块接口
/// 说  明:		
/// 创建日期:	2019年8月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_KERNEL_MODULE_H
#define NFI_KERNEL_MODULE_H

#include <iostream>
#include <string>
#include <functional>
#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "KConstDefine.h"
#include "Dependencies/protobuf/src/google/protobuf/message.h"
 
class NFIKernelModule : public NFIModule
{

public:
	virtual ~NFIKernelModule() {};

	template<typename BaseType>
	bool AddPropertyCallBack(const NFGUID& self, const std::string& strPropertyName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
	{
		NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
		if (pObject.get())
		{
			return pObject->AddPropertyCallBack(strPropertyName, pBase, handler);
		}

		return false;
	}

    template<typename BaseType>
    bool AddRecordCallBack(const NFGUID& self, const std::string& strRecordName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
    {
        NF_SHARE_PTR<NFIObject> pObject = GetObject(self);
        if (pObject.get())
        {
            return pObject->AddRecordCallBack(strRecordName, pBase, handler);
        }

        return false;
    }

	template<typename BaseType>
	bool AddHeartBeat(const NFGUID self, const char* strHeartBeatName, BaseType* pBase, int (BaseType::* handler)(const NFGUID&, const char*, const int, const int), const int nIntervalTime, const int nCount)
	{
		OBJECT_HEARTBEAT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		OBJECT_HEARTBEAT_FUNCTOR_PTR functorPtr(NF_NEW OBJECT_HEARTBEAT_FUNCTOR(functor));
		return AddHeartBeat(self, strHeartBeatName, functorPtr, nIntervalTime, nCount);
	}
    
    template<typename BaseType>
    bool RegisterCommonPropertyEvent(BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
    {
        PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        PROPERTY_EVENT_FUNCTOR_PTR functorPtr(NF_NEW PROPERTY_EVENT_FUNCTOR(functor));
        return RegisterCommonPropertyEvent(functorPtr);
    }

    template<typename BaseType>
    bool RegisterCommonRecordEvent(BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
    {
        RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        RECORD_EVENT_FUNCTOR_PTR functorPtr(NF_NEW RECORD_EVENT_FUNCTOR(functor));
        return RegisterCommonRecordEvent(functorPtr);
    }

	template<typename BaseType>
	bool RegisterClassPropertyEvent(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const std::string&, const NFData&, const NFData&))
	{
		PROPERTY_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		PROPERTY_EVENT_FUNCTOR_PTR functorPtr(NF_NEW PROPERTY_EVENT_FUNCTOR(functor));
		return RegisterClassPropertyEvent(strClassName, functorPtr);
	}

	template<typename BaseType>
	bool RegisterClassRecordEvent(const std::string& strClassName, BaseType* pBase, int (BaseType::*handler)(const NFGUID&, const RECORD_EVENT_DATA&, const NFData&, const NFData&))
	{
		RECORD_EVENT_FUNCTOR functor = std::bind(handler, pBase, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
		RECORD_EVENT_FUNCTOR_PTR functorPtr(NF_NEW RECORD_EVENT_FUNCTOR(functor));
		return RegisterClassRecordEvent(strClassName, functorPtr);
	}

	virtual bool ExistObject(const NFGUID& ident) const = 0;

    virtual NF_SHARE_PTR<NFIObject> GetObject(const NFGUID& ident) const = 0;
    /// \breif   CreateObject 创建对象
    /// \Access  virtual public 
    /// \param   const std::string & strClassName 逻辑类名 空则根据strConfigID获取
    /// \param   const std::string & strConfigID 配置ID 不为空 则找到配置的ClassName覆盖上面参数
    /// \param   const NFDataList & args 设定属性值
    /// \return  NF_SHARE_PTR<NFIObject> 返回对象指针 失败则返回nullptr
    virtual NF_SHARE_PTR<NFIObject> CreateObject(const std::string& strClassName, const std::string& strConfigID, const NFDataList& args) = 0;
	
	// 创建容器
	// nCap 容量
	virtual NF_SHARE_PTR<NFIObject> CreateContainer(const NFGUID& self, std::string& strName, int nCap, std::string& strClassName) = 0;

	virtual bool DestroyObjectAny(const NFGUID self) = 0;
    virtual bool DestroyObject(const NFGUID self) = 0;

    virtual NFGUID CreateGUID() = 0;

	/// \breif   IsBaseOf 逻辑类继承关系判定
	/// \Access  virtual public 
	/// \param   const std::string & strBaseClassName 基类名
	/// \param   const std::string & strClassName 派生类名
	/// \return  bool
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName) = 0;

	///////////////////////////////////////////////////////////////////容器相关 BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief 给玩家添加容器视窗
	/// \param player 玩家对象
	/// \param id 容器编号
	/// \param container 容器对象
	virtual bool AddViewport(const NFGUID& player, int id, const NFGUID& container) = 0;
	/// \brief 玩家删除容器视窗
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual bool RemoveViewport(const NFGUID& player, int id) = 0;
	/// \brief 玩家的视窗是否存在
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual bool FindViewport(const NFGUID& player, int id) = 0;
	/// \brief 获得玩家视窗对应的容器
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual const NFGUID& GetViewportContainer(const NFGUID& player, int id) = 0;
	/// \brief 清除玩家所有视窗
	/// \param player 玩家对象
	virtual bool ClearViewport(const NFGUID& player) = 0;
	/// \brief 取容器的视窗数
	/// \param container 容器对象
	virtual int GetViewerCount(const NFGUID& container) = 0;
	/// \brief 关闭容器的所有视窗
	/// \param container 容器对象
	virtual bool CloseViewers(const NFGUID& container) = 0;

	/// \brief 将对象移动另一个容器中
	/// \param obj 被移动的对象
	/// \param container 容器对象
	virtual bool Place(const NFGUID& obj, const NFGUID& container) = 0;
	/// \brief 将对象放到容器的指定位置
	/// \param obj 被移动的对象
	/// \param container 容器对象
	/// \param index 容器中的位置（从0开始）
	virtual bool PlaceIndex(const NFGUID& obj, const NFGUID& container, int index) = 0;
	/// \brief 容器之间交换指定位置上的子对象
	/// \param container1 容器一
	/// \param index1 容器一的位置
	/// \param container2 容器二
	/// \param index2 容器二的位置
	virtual bool Exchange(const NFGUID& container1, int index1, const NFGUID& container2, int index2) = 0;
	/// \brief 移动对象到容器的指定位置
	/// \param obj 被移动的对象
	/// \param newIndex 新的容器位置（从0开始）
	virtual bool ChangeIndex(const NFGUID& obj, int newIndex) = 0;

	/// \brief 返回容器的容量
	/// \param container 容器对象
	virtual int GetCapacity(const NFGUID& container) = 0;
	/// \brief 扩容
	/// \param container 容器对象
	virtual int ExtendCapacity(const NFGUID& container, int nCap) = 0;
	/// \brief 取对象在容器中的位置
	/// \param obj 对象
	/// \return 从0开始 -1表示没有
	virtual int GetIndex(const NFGUID& obj) = 0;

	/// \brief 取容器指定位置的子对象
	/// \param obj 容器对象
	/// \param index 容器中的位置（从0开始）
	virtual const NFGUID& GetChild(const NFGUID& obj, int index) = 0;
	/// \brief 用名字查找子对象
	/// \param obj 容器对象
	/// \param name 子对象的名字
	virtual const NFGUID& GetChild(const NFGUID& obj, const std::string& name) = 0;
	/// \brief 返回子对象数量
	/// \param obj 容器对象
	virtual int GetChildCount(const NFGUID& obj) = 0;
	/// \brief 获得子对象列表
	/// \param obj 容器对象
	virtual const std::vector<NFGUID>& GetChildList(const NFGUID& obj) = 0;
	/// \brief 清除所有子对象
	/// \param obj 容器对象
	virtual bool ClearChild(const NFGUID& obj) = 0;

	/// \breif   GetPropertyList 获取所有属性名
	/// \Access  virtual public 
	/// \param   const NFGUID & obj 对象
	/// \param   NFDataList & args 属性名列表
	/// \return  bool
	virtual bool GetPropertyList(const NFGUID& obj, NFDataList& args) = 0;
	/// \breif   GetRecordList 获取所有表名
	/// \Access  virtual public 
	/// \param   const NFGUID & obj 对象
	/// \param   NFDataList & args 表名列表
	/// \return  bool
	virtual bool GetRecordList(const NFGUID& obj, NFDataList& args) = 0;
	///////////////////////////////////////////////////////////////////容器相关 END//////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////心跳相关 BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief 删除心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool RemoveHeartBeat(const NFGUID& obj, const char* func) = 0;
	/// \brief 清除心跳函数
	/// \param obj 对象
	virtual bool ClearHeartBeat(const NFGUID& obj) = 0;
	/// \brief 查找心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool FindHeartBeat(const NFGUID& obj, const char* func) = 0;
	/// \brief 获得心跳最大次数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetHeartBeatCount(const NFGUID& obj, const char* func) = 0;
	/// \brief 设置心跳最大次数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual void SetHeartBeatCount(const NFGUID& obj, const char* func, int nCount) = 0;
	/// \brief 获得心跳时间
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetHeartBeatTime(const NFGUID& obj, const char* func) = 0;
	///////////////////////////////////////////////////////////////////心跳相关 END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////事件相关 BEGIN//////////////////////////////////////////////////////////////////////
	/// \breif   AddEventCallBack 添加事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型 EVENT_OnDefault 为默认事件，会响应所有事件, 事件类型和触发的逻辑类名通过args{int 事件类型, string 逻辑类名, ...其他参数}来返回
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

	/// \breif   AddCommondCallBack 添加内部消息回调
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   int nMsgId 消息号 0为默认消息，会响应所有消息，消息Id通过args{int 消息Id, string 逻辑类名, ...其他参数}
	/// \param   NFIModule * pOwner 回调对象
	/// \param   EVENT_HOOK pHook 回调函数
	/// \param   int nPrior 优先级
	/// \return  bool 是否成功
	virtual bool AddCommondCallBack(const std::string& strClassName, int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0) = 0;

	/// \breif   AddExtraCallBack 添加附加服务器消息回调
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   NF_SERVER_TYPES eServerType 服务器类型
	/// \param   int nMsgId 消息ID
	/// \param   NF_SHARE_PTR<IMethodMsg> pMethod 消息回调
	/// \return  bool 是否成功
	virtual bool AddExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;

	/// \breif   SendExtraMessage 发送消息到附加服务器
	/// \Access  virtual public 
	/// \param   NF_SERVER_TYPES eServerType 附加服务器类型
	/// \param   int nMsgId	消息ID	
	/// \param   const NFGUID & self 发送者
	/// \param   const google::protobuf::Message & msg 数据
	/// \return  bool 是否成功
	virtual bool SendExtraMessage(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const google::protobuf::Message& msg) = 0;

	/// \breif   AddMaintainCallBack 添加维护端口发来消息回调
	/// \Access  virtual public 
	/// \param   int nMsgId 消息ID
	/// \param   NFIModule * pBase 回调对象
	/// \param   NetMsgFun fun 回调函数
	virtual void AddMaintainCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun) = 0;

	/// \breif   SendMaintainMessage 发送消息给维护端口
	/// \Access  virtual public 
	/// \param   int nMsgId 消息ID
	/// \param   string & xData 内容
	/// \param   NFSOCK nSockIndex 目标连接
	virtual bool SendMaintainMessage(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex) = 0;

	///////////////////////////////////////////////////////////////////事件相关 END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////功能代理 BEGIN////////////////////////////////////////////////////////////////////
	/// \breif   Load 加载配置文件, 存在则立即返回
	/// \Access  virtual public
	/// \param   const char * szPath 配置文件绝对路径
	/// \param   std::function<void(void)> pfReload = nullptr 重载时被调用的回调 默认空
	/// \return  google::protobuf::Message* 消息对象指针
	virtual const google::protobuf::Message* LoadRes(const char* szPath, ResReloadFunction pfReload = nullptr) = 0;

	/// \breif   ReloadRes 重载已经加载过的配置
	/// \Access  virtual public 
	/// \param   const char * szPath 配置文件绝对路径
	/// \return  bool 是否成功
	virtual bool ReloadRes(const char* szPath) = 0;

	/// \breif   ReloadResAll 重载所有已经加载过的配置
	/// \Access  virtual public 
	/// \return  bool
	virtual bool ReloadResAll() = 0;

	/// \breif   ConvertMapData 将 pb 数据转换为map
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName) = 0;
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName) = 0;

	// 获取配置文件中，逻辑类属性值
	virtual int64_t GetResInt(const std::string& strConfigID, const std::string& strPropName) = 0;
	virtual double GetResFloat(const std::string& strConfigID, const std::string& strPropName) = 0;
	virtual const std::string& GetResString(const std::string& strConfigID, const std::string& strPropName) = 0;

	//增加一个敏感词
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord) = 0;

	//查找字符串里所有的敏感词
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out) = 0;

	//把字符串里所有的敏感词替换 返回新串
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*') = 0;

	//检测字符串里是否有敏感词
	virtual bool ExistSensitiveWord(const std::string& text) const = 0;
	///////////////////////////////////////////////////////////////////功能代理 END//////////////////////////////////////////////////////////////////////

	/// \breif   PlayerLeaveGame 离开游戏
	/// \Access  virtual protected 
	/// \param   const NFGUID & player 玩家
	/// \return  bool
	virtual bool PlayerLeaveGame(const NFGUID& player) = 0;

	// prometheus数据采集
	virtual prometheus::Family<prometheus::Counter>& GetMetricsCounter(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Gauge>& GetMetricsGauge(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Histogram>& GetMetricsHistogram(int nTempIndex) = 0;
	virtual prometheus::Family<prometheus::Summary>& GetMetricsSummary(int nTempIndex) = 0;
protected:

	/// \brief 添加心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param time 心跳时长
	/// \param count 心跳次数
	/// 注：所加的函数参数是固定的，如下
	/// int func(const NFGUID&, const std::string&, const int, const int) 
	/// time的单位为毫秒
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count) = 0;
	///////////////////////////////////////////////////////////////////事件相关 END//////////////////////////////////////////////////////////////////////

protected:
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
    virtual bool RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
	
	virtual bool RegisterClassPropertyEvent(const std::string& strClassName, const PROPERTY_EVENT_FUNCTOR_PTR& cb) = 0;
	virtual bool RegisterClassRecordEvent(const std::string& strClassName, const RECORD_EVENT_FUNCTOR_PTR& cb) = 0;
};

#endif
