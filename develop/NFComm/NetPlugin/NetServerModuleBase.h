///--------------------------------------------------------------------
/// 文件名:		NetServerModuleBase.h
/// 内  容:		网络服务模式基类
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetServerModuleBase_H__
#define __H_NetServerModuleBase_H__
#include <list>
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NetServer.h"
#include "Dependencies/protobuf/src/google/protobuf/api.pb.h"
#include "../NFPluginModule/NFIPluginManager.h"

class NetServerModuleBase : public NFIModule
{
public:
	virtual bool Execute();

	virtual void AddReceiveCallBack(NFIModule* pBase, NetMsgFun fun);
	virtual void AddReceiveCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun);

	virtual void AddEventCallBack(NFIModule* pBase, NetEventFun fun);

	virtual bool SendMsg(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex);
	virtual bool SendMsg(const uint16_t nMsgID, const google::protobuf::Message& xData, const NFSOCK nSockIndex);

	virtual bool SendMsgToAll(const uint16_t nMsgID, const std::string& xData);
	virtual bool SendMsgToAll(const uint16_t nMsgID, const google::protobuf::Message& xData);

	virtual bool SendMsgToList(const uint16_t nMsgID, const std::string& xData, const std::list<NFSOCK>& lSockIndex);

	virtual NetObj* GetNetObj(const NFSOCK nSockIndex);

	virtual void CloseNetObj(const NFSOCK nSockIndex);

	virtual const std::string& GetClassName();

	virtual const std::string& GetIP();
	virtual const int GetPort();
protected:
	virtual bool Initialization(const size_t maxSendBufferSize, const size_t maxReadBufferSize,
		const int nMaxClient, const bool threads, const std::string& ip, const int port);

protected:
	NetServer* m_pNet = nullptr;
	//回调
	NetCallback mCallBackMap;
	std::string mClassName;

	std::string mIP;
	int mPort;
};

#endif //__H_NetServerModuleBase_H__