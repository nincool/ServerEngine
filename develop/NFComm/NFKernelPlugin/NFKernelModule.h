///--------------------------------------------------------------------
/// 文件名:		NFKernelModule.h
/// 内  容:		核心模块
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_KERNEL_MODULE_H
#define NF_KERNEL_MODULE_H

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <unordered_map>
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFIRecord.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFPluginModule/NFIEventModule.h"
#include "NFComm/NFPluginModule/NFIResModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFMidWare/NFWordsFilterPlugin/WordsFilterModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class NFKernelModule : public NFIKernelModule
{
public:
	typedef std::unordered_map<NFGUID, NF_SHARE_PTR<NFIObject>> HashObject;
public:
    NFKernelModule(NFIPluginManager* p);
    virtual ~NFKernelModule();

    virtual bool Init();
    virtual bool Shut();

    virtual bool BeforeShut();
    virtual bool AfterInit();

    virtual bool Execute();

	virtual bool ExistObject(const NFGUID& ident) const;
	virtual const HashObject& GetObjectAll() const;
    virtual NF_SHARE_PTR<NFIObject> GetObject(const NFGUID& ident) const;
    virtual NF_SHARE_PTR<NFIObject> CreateObject(const std::string& strClassName, const std::string& strConfigID, const NFDataList& args);
	virtual NF_SHARE_PTR<NFIObject> CreateObject(const NFGUID& self, const std::string& strClassName, const std::string& strConfigID, const NFDataList& args, std::function<void(NF_SHARE_PTR<NFIObject>)> pf = nullptr);
	virtual NF_SHARE_PTR<NFIObject> CreateContainer(const NFGUID& self, std::string& strName, int nCap, std::string& strClassName);

	virtual bool DestroyAll();
    virtual bool DestroySelf(const NFGUID& self);
	// 销毁对象，不用引用参数, 避免循环引用，删除流程中断
	virtual bool DestroyObject(const NFGUID self);
    virtual bool DestroyObjectAny(const NFGUID self);
	virtual bool DestroyObject(NFObject* pObj);
	virtual bool DestroyPlayer(const NFGUID self);

	// 对象不可用(通过GetObject获取不到)
	virtual bool DisableObject(const NFGUID& self);
	virtual bool EnableObject(const NFGUID& self);
	virtual NF_SHARE_PTR<NFIObject> GetDisableObject(const NFGUID& self);

	virtual NFGUID CreateGUID();

	/// \breif   IsBaseOf 逻辑类继承关系判定
	virtual bool IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName);

	///////////////////////////////////////////////////////////////////容器相关 BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief 给玩家添加容器视窗
	/// \param player 玩家对象
	/// \param id 容器编号
	/// \param container 容器对象
	virtual bool AddViewport(const NFGUID& player, int id, const NFGUID& container);
	/// \brief 玩家删除容器视窗
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual bool RemoveViewport(const NFGUID& player, int id);
	/// \brief 玩家的视窗是否存在
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual bool FindViewport(const NFGUID& player, int id);
	/// \brief 获得玩家视窗对应的容器
	/// \param player 玩家对象
	/// \param id 容器编号
	virtual const NFGUID& GetViewportContainer(const NFGUID& player, int id);
	/// \brief 清除玩家所有视窗
	/// \param player 玩家对象
	virtual bool ClearViewport(const NFGUID& player);
	/// \brief 取容器的视窗数
	/// \param container 容器对象
	virtual int GetViewerCount(const NFGUID& container);
	/// \brief 关闭容器的所有视窗
	/// \param container 容器对象
	virtual bool CloseViewers(const NFGUID& container);

	/// \brief 将对象移动另一个容器中
	/// \param obj 被移动的对象
	/// \param container 容器对象
	virtual bool Place(const NFGUID& obj, const NFGUID& container);
	/// \brief 将对象放到容器的指定位置
	/// \param obj 被移动的对象
	/// \param container 容器对象
	/// \param index 容器中的位置（从0开始）
	virtual bool PlaceIndex(const NFGUID& obj, const NFGUID& container, int index);
	/// \brief 容器之间交换指定位置上的子对象
	/// \param container1 容器一
	/// \param index1 容器一的位置
	/// \param container2 容器二
	/// \param index2 容器二的位置
	virtual bool Exchange(const NFGUID& container1, int index1, const NFGUID& container2, int index2);
	/// \brief 移动对象到容器的指定位置
	/// \param obj 被移动的对象
	/// \param newIndex 新的容器位置（从0开始）
	virtual bool ChangeIndex(const NFGUID& obj, int newIndex);

	/// \brief 返回容器的容量
	/// \param container 容器对象
	virtual int GetCapacity(const NFGUID& container);
	/// \brief 扩容
	/// \param container 容器对象
	virtual int ExtendCapacity(const NFGUID& container, int nCap);
	/// \brief 取对象在容器中的位置
	/// \param obj 对象
	/// \return 从0开始 -1表示没有
	virtual int GetIndex(const NFGUID& obj);

	/// \brief 取容器指定位置的子对象
	/// \param obj 容器对象
	/// \param index 容器中的位置（从0开始）
	virtual const NFGUID& GetChild(const NFGUID& obj, int index);
	/// \brief 用名字查找子对象
	/// \param obj 容器对象
	/// \param name 子对象的名字
	virtual const NFGUID& GetChild(const NFGUID& obj, const std::string& name);
	/// \brief 返回子对象数量
	/// \param obj 容器对象
	virtual int GetChildCount(const NFGUID& obj);
	/// \brief 获得子对象列表
	/// \param obj 容器对象
	virtual const std::vector<NFGUID>& GetChildList(const NFGUID& obj);
	/// \brief 清除所有子对象
	/// \param obj 容器对象
	virtual bool ClearChild(const NFGUID& obj);

	/// \breif   GetPropertyList 获取所有属性名
	virtual bool GetPropertyList(const NFGUID& obj, NFDataList& args);
	/// \breif   GetRecordList 获取所有表名
	virtual bool GetRecordList(const NFGUID& obj, NFDataList& args);

	///////////////////////////////////////////////////////////////////容器相关 END////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////心跳相关 BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief 添加心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	/// \param time 心跳时长
	/// \param count 心跳次数
	/// 注：所加的函数参数是固定的，如下
	/// int func(const NFGUID&, const std::string&, const int, const int) 
	/// time的单位为毫秒
	virtual bool AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count);
	/// \brief 删除心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool RemoveHeartBeat(const NFGUID& obj, const char* func);
	/// \brief 清除心跳函数
	/// \param obj 对象
	virtual bool ClearHeartBeat(const NFGUID& obj);
	/// \brief 查找心跳函数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual bool FindHeartBeat(const NFGUID& obj, const char* func);
	/// \brief 获得心跳最大次数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetHeartBeatCount(const NFGUID& obj, const char* func);
	/// \brief 设置心跳最大次数
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual void SetHeartBeatCount(const NFGUID& obj, const char* func, int nCount);
	/// \brief 获得心跳时间
	/// \param obj 对象
	/// \param func 心跳函数名
	virtual int GetHeartBeatTime(const NFGUID& obj, const char* func);
	///////////////////////////////////////////////////////////////////心跳相关 END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////事件相关 BEGIN//////////////////////////////////////////////////////////////////////
	/// \breif   AddEventCallBack 添加事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   NFIModule * pOwner 事件回调对象
	/// \param   EVENT_HOOK pHook 事件回调
	/// \param   int nPrior 优先级
	/// \return  bool 是否成功
	virtual bool AddEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);

	/// \breif   RemoveEventCallBack 移除事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   void * pOwner 事件回调对象
	/// \param   EVENT_HOOK pHook 事件回调
	/// \return  bool 是否成功
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);

	/// \breif   RemoveEventCallBack 移除事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   void * pOwner 事件回调对象
	/// \return  bool 是否成功
	virtual bool RemoveEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner);
	/// \breif   RemoveEventCallBack 移除事件对象的所有事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   void * pOwner 事件回调对象
	/// \return  bool 是否成功
	virtual bool RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner);
	/// \breif   RemoveEventCallBack 移除事件对象的所有事件
	/// \Access  virtual public 
	/// \param   void * pOwner 事件回调对象
	/// \return  bool
	virtual bool RemoveEventCallBack(NFIModule* pOwner);

	/// \breif   FindEventCallBack 查找事件
	/// \Access  virtual public 
	/// \param   const std::string & strClassName 逻辑类名
	/// \param   EventType eType 事件类型
	/// \param   void * pOwner 事件回调对象
	/// \param   EVENT_HOOK pHook 事件回调
	/// \return  bool 是否成功
	virtual bool FindEventCallBack(const std::string& strClassName, EventType eType, NFIModule* pOwner, EVENT_HOOK pHook);

	/// \breif   AddCommondCallBack 添加内部消息回调
	virtual bool AddCommondCallBack(const std::string& strClassName, int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior = 0);

	/// \breif   AddExtraCallBack 添加附加服务器消息回调
	virtual bool AddExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);
	/// \breif   SendExtraMessage 发送消息到附加服务器
	virtual bool SendExtraMessage(NF_SERVER_TYPES eServerType, int nMsgId, const NFGUID& self, const google::protobuf::Message& msg);

	/// \breif   添加维护端口发来消息回调
	virtual void AddMaintainCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun);
	/// \breif   发送消息给维护端口
	virtual bool SendMaintainMessage(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex);

	///////////////////////////////////////////////////////////////////事件相关 END//////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////代理模块接口 BEGIN////////////////////////////////////////////////////////////////////
	// 加载配置
	virtual const google::protobuf::Message* LoadRes(const char* szPath, ResReloadFunction pfReload = nullptr);
	/// \breif   ReloadRes 重载已经加载过的配置
	virtual bool ReloadRes(const char* szPath);
	/// \breif   ReloadResAll 重载所有已经加载过的配置
	virtual bool ReloadResAll();

	/// \breif   ConvertMapData 将 pb 数据转换为map
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName);
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName);

	virtual int64_t GetResInt(const std::string& strConfigID, const std::string& strPropName);
	virtual double GetResFloat(const std::string& strConfigID, const std::string& strPropName);
	virtual const std::string& GetResString(const std::string& strConfigID, const std::string& strPropName);

	//增加一个敏感词
	virtual bool AddSensitiveWord(const std::string& strSensitiveWord);

	//查找字符串里所有的敏感词
	virtual void FindAllSensitiveWords(const std::string& text, std::set<std::string>& out);
	//把字符串里所有的敏感词替换 返回新串
	virtual void ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar = '*');

	//检测字符串里是否有敏感词
	virtual bool ExistSensitiveWord(const std::string& text) const;
	///////////////////////////////////////////////////////////////////代理模块接口 END//////////////////////////////////////////////////////////////////////

	// 离开游戏
	virtual bool PlayerLeaveGame(const NFGUID& player);
	
	// prometheus数据采集
	virtual prometheus::Family<prometheus::Counter>& GetMetricsCounter(int nTempIndex);
	virtual prometheus::Family<prometheus::Gauge>& GetMetricsGauge(int nTempIndex);
	virtual prometheus::Family<prometheus::Histogram>& GetMetricsHistogram(int nTempIndex);
	virtual prometheus::Family<prometheus::Summary>& GetMetricsSummary(int nTempIndex);
protected:
    virtual bool RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb);
    virtual bool RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb);

	virtual bool RegisterClassPropertyEvent(const std::string& strClassName, const PROPERTY_EVENT_FUNCTOR_PTR& cb);
	virtual bool RegisterClassRecordEvent(const std::string& strClassName, const RECORD_EVENT_FUNCTOR_PTR& cb);

protected:
    int OnPropertyCommonEvent(const NFGUID& self, const std::string& strPropertyName, const NFData& oldVar, const NFData& newVar);
    int OnRecordCommonEvent(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar);

protected:

    std::list<NFGUID> mtDeleteSelfList;
    std::list<PROPERTY_EVENT_FUNCTOR_PTR> mtCommonPropertyCallBackList;
    std::list<RECORD_EVENT_FUNCTOR_PTR> mtCommonRecordCallBackList;

	std::map<std::string, std::list<PROPERTY_EVENT_FUNCTOR_PTR>> mtClassPropertyCallBackList;
	std::map<std::string, std::list<RECORD_EVENT_FUNCTOR_PTR>> mtClassRecordCallBackList;

private:
	// 对象集合
	HashObject m_hashObject;
	// 不可用的对象集合
	std::map<NFGUID, NF_SHARE_PTR<NFIObject>> m_mapDisableObject;
    NFGUID mnCurExeObject;

    NFIClassModule* m_pClassModule = nullptr;
    NFIElementModule* m_pElementModule = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	NFIEventModule* m_pEventModule = nullptr;
	NFIResModule* m_pResModule = nullptr;
	//敏感词
	IWordsFilterModule* m_pWordsFilterModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NetServerInsideModule* m_pNetServerInside = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif
