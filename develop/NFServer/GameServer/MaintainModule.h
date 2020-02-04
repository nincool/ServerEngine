///--------------------------------------------------------------------
/// 文件名:		MaintainModule.h
/// 内  容:		维护模块
/// 说  明:		
/// 创建日期:	2019年10月14日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_MaintainModule_H__
#define __H_MaintainModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "MaintainMsgDataDefine.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"
#include "GameKernel.h"

class MaintainModule : public NFIModule
{
public:
	MaintainModule(NFIPluginManager* p);
	~MaintainModule();

	virtual bool Init();
	virtual bool AfterInit();
private:
	//取得对象列表
	bool GetObjList(const REQGetObjList& req, ACKGetOBJList& ack);
	//取得对象属性列表
	bool GetObjPropList(const REQSelectObj& req, ACKGetObjPropList& ack);
	//取得对象表格列表
	bool GetObjRecList(const REQSelectObj& req, ACKGetObjRecList& ack);
	//取得对象容器列表
	bool GetObjConList(const REQSelectObj& req, ACKGetOBJList& ack);
	//取得对象表格内容
	bool GetObjRec(const J_SelectObjRec& req, J_ObjRecData& ack);
	//取得对象定时器列表
	bool GetObjBeatList(const REQSelectObj& req, ACKGetObjBeatList& ack);
	//取得对象定时器内容
	bool GetObjBeat(const J_SelectObjBeat& req, J_ObjBeatData& ack);
	//取得对象属性回调列表
	bool GetObjPropCriticalList(const REQSelectObj& req, ACKGetObjPropCriticalList& ack);
	//取得对象表格回调列表
	bool GetObjRecCallBackList(const REQSelectObj& req, ACKGetObjRecCallBackList& ack);

	//修改对象属性
	bool SetObjProp(const J_ModifyObjProp& req);
	//修改对象表
	bool SetObjRec(J_ModifyObjRec& req);
private:
	//取得对象列表
	void OnGetObjList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象属性列表
	void OnGetObjPropList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象表格列表
	void OnGetObjRecList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象容器列表
	void OnGetObjConList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象表格内容
	void OnGetObjRec(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象定时器列表
	void OnGetObjBeatList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象定时器内容
	void OnGetObjBeat(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象属性回调列表
	void OnGetObjPropCriticalList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得对象表格回调列表
	void OnGetObjRecCallbackList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//修改对象属性
	void OnSetObjProp(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//修改对象表
	void OnSetObjRec(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//修改对容器
	void OnSetObjCon(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//取得玩家GUID Name列表
	void OnGetPlayerGNList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);

	std::string ExportOBJList(std::vector<std::string>& arg);
	std::string ExportOBJ(std::vector<std::string>& arg);

private:
	NetServerMaintainModule* m_pMaintainNetServer = nullptr;
	NFKernelModule* m_pKernelModule = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	CGameKernel* m_pGameKernel = nullptr;
};


#endif //__H_MaintainModule_H__