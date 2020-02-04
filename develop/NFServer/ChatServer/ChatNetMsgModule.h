///--------------------------------------------------------------------
/// 文件名:		ChatNetMsgModule.h
/// 内  容:		聊天服网络消息处理
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_ChatNetMsgModule_H__
#define __H_ChatNetMsgModule_H__

#include <map>
#include "ChatModule.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "ChatServerInit.h"

class ChatNetMsgModule : public NFIModule
{
public:
	ChatNetMsgModule(NFIPluginManager* p);

	//管理器调用初始化
	virtual bool Init();

	//管理器调用初始化后
	virtual bool AfterInit();

	//管理器调用帧循环
	virtual bool Execute();

private:
	void OnSocketClientEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);

	//监听服务内部消息
	void OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//移除一个聊天房间
	void OnRemoveRoomProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//增加玩家到一个聊天房间
	void OnAddPlayerToRoomProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//移除聊天房间一个玩家（-1为玩家退出所有房间）
	void OnRemoveRoomPlayerProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//更新玩家信息
	void OnUpdataPlayerInfo(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);
	//禁言一个玩家
	void OnBannedChange(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg);

	//更新客户端信息
	void OnUpdataClient(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//客户端发消息到指定房间
	void OnClientMsgRoom(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//客户端发消息给指定玩家
	void OnClientMsgPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//客户端取聊天记录
	void OnGetChatRecording(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	//客户端断开
	void OnDisconnectClient(const NFSOCK nSockIndex, const NET_EVENT eEvent);

	//踢出长时间没有客户端的玩家对象
	void CheckPlayer();

	//删除长时间没玩家的房间
	void CheckRoom();

private:
	ChatModule* m_pChatModule = nullptr;
	NFINT64 mLastCheckTime = 0;
	NFINT64 mLastRoomCheckTime = 0;

	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	ChatServerInit* m_pChatServerInit = nullptr;
};

#endif	//__H_ChatNetMsgModule_H__