///--------------------------------------------------------------------
/// 文件名:		ChatNetMsgModule.cpp
/// 内  容:		聊天服网络消息处理
/// 说  明:		
/// 创建日期:	2019年9月27日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ChatNetMsgModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/NFDefine.pb.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include <list>

ChatNetMsgModule::ChatNetMsgModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

//管理器调用初始化
bool ChatNetMsgModule::Init()
{
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetServerOuterModule = pPluginManager->FindModule<NetServerOuterModule>();
	m_pChatModule = pPluginManager->FindModule<ChatModule>();
	m_pChatServerInit = pPluginManager->FindModule<ChatServerInit>();

	return true;
}

//管理器调用初始化后
bool ChatNetMsgModule::AfterInit()
{
	//监听服务内部消息
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_EXTRA_SERVER, this, (NetMsgFun)&ChatNetMsgModule::OnExtraMsg);

	//监听客户端发来的消息
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_CHAT_UPDATA_CLIENT, this, (NetMsgFun)&ChatNetMsgModule::OnUpdataClient);
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_CHAT_MSG_ROOM, this, (NetMsgFun)&ChatNetMsgModule::OnClientMsgRoom);
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_CHAT_MSG_PLAYER, this, (NetMsgFun)&ChatNetMsgModule::OnClientMsgPlayer);
	m_pNetServerOuterModule->AddReceiveCallBack(OuterMsg::CS_CHAT_RECORDING, this, (NetMsgFun)&ChatNetMsgModule::OnGetChatRecording);

	//客户端接入事件
	m_pNetServerOuterModule->AddEventCallBack(this, (NetEventFun)&ChatNetMsgModule::OnSocketClientEvent);

	return true;
}

//管理器调用帧循环
bool ChatNetMsgModule::Execute()
{
	CheckPlayer();
	CheckRoom();
	return true;
}

void ChatNetMsgModule::OnSocketClientEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent)
{
	if (!((int)eEvent & (int)NET_EVENT::NET_EVENT_CONNECTED))
	{
		OnDisconnectClient(nSockIndex, eEvent);
	}
}

//监听服务内部消息
void ChatNetMsgModule::OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ExtraMsg ParseFromArray failed";
		return;
	}

	std::string ackMsg;
	switch (xMsgBase.msg_id())
	{
	case OuterMsg::SS_CHAT_REMOVE_ROOM:
		OnRemoveRoomProcess(xMsgBase, ackMsg);
		break;
	case OuterMsg::SS_CHAT_ADD_PLAYER_ROOM:
		OnAddPlayerToRoomProcess(xMsgBase, ackMsg);
		break;
	case OuterMsg::SS_CHAT_REMOVE_PLAYER_ROOM:
		OnRemoveRoomPlayerProcess(xMsgBase, ackMsg);
		break;
	case OuterMsg::SS_CHAT_PLAYER_INFO:
		OnUpdataPlayerInfo(xMsgBase, ackMsg);
		break;
	case OuterMsg::SS_CHAT_PLAYER_BANNED:
		OnBannedChange(xMsgBase, ackMsg);
		break;
	}

	// 回应消息
	xMsgBase.set_data(ackMsg.c_str());
	std::string strMsg;
	if (!xMsgBase.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " ExtraMsg SerializeToString failed";
		return;
	}

	m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_EXTRA_SERVER, strMsg, nSockIndex);
}

void ChatNetMsgModule::OnRemoveRoomProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg)
{
	OuterMsg::ChatRemoveRoom REQRemoveRoom;
	if (!REQRemoveRoom.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << " ChatRemoveRoom ParseFromString failed";
		return;
	}

	m_pChatModule->RemoveRoom(REQRemoveRoom.key());

	OuterMsg::MsgReply ack;
	ack.set_ret_code(OuterMsg::EGEC_SUCCESS);
	ack.SerializeToString(&ackMsg);
}

//增加玩家到一个聊天房间
void ChatNetMsgModule::OnAddPlayerToRoomProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg)
{
	OuterMsg::ChatAddPlayerToRoom AddPlayerToRoom;
	if (!AddPlayerToRoom.ParseFromString(xMsgBase.data()))
	{
		AddPlayerToRoom.set_room_id(-1);
		AddPlayerToRoom.SerializeToString(&ackMsg);

		QLOG_WARING << __FUNCTION__ << " ChatAddPlayerToRoom ParseFromString failed";
		return;
	}

	//玩家UID
	NFGUID uid;
	PBConvert::ToNF(AddPlayerToRoom.player(), uid);

	//检查玩是否存在，不存在就增加
	if (nullptr == m_pChatModule->GetPlayerData(uid))
	{
		m_pChatModule->AddPlayerData(uid);
	}

	//检查房间是否存在，不存在就创建
	int roomID = m_pChatModule->GetRoomID(AddPlayerToRoom.key());
	if (-1 == roomID)
	{
		roomID = m_pChatModule->CreateRoom(AddPlayerToRoom.key(), (bool)AddPlayerToRoom.chat_recording());
	}

	//增加玩家到房间
	m_pChatModule->AddPlayerToRoom(roomID, uid);

	//设置回复
	AddPlayerToRoom.set_room_id(roomID);
	AddPlayerToRoom.SerializeToString(&ackMsg);
}

//聊天房间移除一个玩家
void ChatNetMsgModule::OnRemoveRoomPlayerProcess(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg)
{
	OuterMsg::ChatRemovePlayerToRoom RemovePlayerToRoom;
	if (!RemovePlayerToRoom.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << " ChatRemovePlayerToRoom ParseFromString failed";
		return;
	}

	//玩家UID
	NFGUID uid;
	PBConvert::ToNF(RemovePlayerToRoom.player(), uid);

	if (RemovePlayerToRoom.key().empty())
	{
		//退出所有房间
		m_pChatModule->RemovePlayerData(uid);
	}
	else
	{
		//退出指定房间
		int roomID = m_pChatModule->GetRoomID(RemovePlayerToRoom.key());
		m_pChatModule->RemoveRoomPlayer(roomID, uid);
	}

	//设置回复
	RemovePlayerToRoom.SerializeToString(&ackMsg);
}

//更新玩家信息
void ChatNetMsgModule::OnUpdataPlayerInfo(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg)
{
	OuterMsg::ChatPlayerInfo playerInfo;
	if (!playerInfo.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << " ChatPlayerInfo ParseFromString failed";
		return;
	}

	//玩家UID
	NFGUID uid;
	PBConvert::ToNF(playerInfo.player(), uid);

	//不存在就增加一个
	PlayerData* pPlayerData = m_pChatModule->GetPlayerData(uid);
	if (nullptr == pPlayerData)
	{
		pPlayerData = m_pChatModule->AddPlayerData(uid);
	}

	if (nullptr == pPlayerData)
	{
		QLOG_WARING << __FUNCTION__ << " pPlayerData == NULL";
		return;
	}

	//高置自定信息
	pPlayerData->Info = playerInfo.custom_info();
}

//禁言一个玩家
void ChatNetMsgModule::OnBannedChange(OuterMsg::ExtraMsg xMsgBase, std::string& ackMsg)
{
	OuterMsg::ChatPlayerBanned playerBanned;
	if (!playerBanned.ParseFromString(xMsgBase.data()))
	{
		QLOG_WARING << __FUNCTION__ << " ChatPlayerBanned ParseFromString failed";
		return;
	}

	//玩家UID
	NFGUID uid;
	PBConvert::ToNF(playerBanned.player(), uid);

	PlayerData* pPlayerData = m_pChatModule->GetPlayerData(uid);
	if (nullptr == pPlayerData)
	{
		QLOG_WARING << __FUNCTION__ << " playerGUID not exist:" << uid;
		return;
	}

	pPlayerData->Banned = (bool)playerBanned.banned();
}

//更新客户端信息
void ChatNetMsgModule::OnUpdataClient(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ChatClientData clientData;
	if (!clientData.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ChatClientData ParseFromArray failed";
		return;
	}

	NFGUID uid;
	PBConvert::ToNF(clientData.player(), uid);

	//设置网络ID
	if (!m_pChatModule->SetPlayerSockIndex(uid, nSockIndex))
	{
		return;
	}

	//标记网络连接合法
	NetObj* pNetObj = m_pNetServerOuterModule->GetNetObj(nSockIndex);
	if (nullptr == pNetObj)
	{
		return;
	}
	pNetObj->SetConnectKeyState(1);
}

//客户端发消息到指定房间
void ChatNetMsgModule::OnClientMsgRoom(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ChatMsgRoom msgRoom;
	if (!msgRoom.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ChatMsgRoom ParseFromArray failed";
		return;
	}

	PlayerData* pSender = m_pChatModule->GetPlayerData(nSockIndex);
	if (nullptr == pSender)
	{
		return;
	}

	if (pSender->Banned)
	{
		return;
	}

	PBConvert::ToPB(pSender->GetGUID(), *msgRoom.mutable_sender_player());
	msgRoom.set_sender_info(pSender->Info);

	m_pChatModule->SendMsgToRoom(msgRoom.room_id(), msgRoom);
}

//客户端发消息给指定玩家
void ChatNetMsgModule::OnClientMsgPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ChatMsgPlayer msgPlayer;
	if (!msgPlayer.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ChatMsgPlayer ParseFromArray failed";
		return;
	}

	//设置发送者
	PlayerData* pSender = m_pChatModule->GetPlayerData(nSockIndex);
	if (nullptr == pSender)
	{
		return;
	}

	PBConvert::ToPB(pSender->GetGUID(), *msgPlayer.mutable_sender_player());
	msgPlayer.set_sender_info(pSender->Info);

	m_pChatModule->SenddMsgToPlayer(msgPlayer);
}

//客户端断开
void ChatNetMsgModule::OnDisconnectClient(const NFSOCK nSockIndex, const NET_EVENT eEvent)
{
	PlayerData* pSenderPlayer = m_pChatModule->GetPlayerData(nSockIndex);
	if (nullptr == pSenderPlayer)
	{
		return;
	}

	m_pChatModule->ClearPlayerSockIndex(pSenderPlayer->GetGUID());
}

//踢出长时间没有客户端的玩家对象
void ChatNetMsgModule::CheckPlayer()
{
	if (mLastCheckTime + 10 > pPluginManager->GetNowTime())
	{
		return;
	}
	mLastCheckTime = pPluginManager->GetNowTime();

	const std::map<PlayerData*, time_t>& noClientPlayerMap = m_pChatModule->GetNoClientPlayerMap();
	time_t curTime = time(nullptr);

	std::list<NFGUID> removeList;

	auto it = noClientPlayerMap.begin();
	auto itEnd = noClientPlayerMap.end();
	for (; it != itEnd; ++it)
	{
		if (curTime - it->second < m_pChatServerInit->GetDelayDeletePlayerTime())
		{
			continue;
		}

		removeList.push_back(it->first->GetGUID());
	}

	auto remove_it = removeList.begin();
	auto remove_itEnd = removeList.end();
	for (; remove_it != remove_itEnd; ++remove_it)
	{
		m_pChatModule->RemovePlayerData(*remove_it);
	}
}

//删除长时间没玩家的房间
void ChatNetMsgModule::CheckRoom()
{
	if (mLastRoomCheckTime + 15 > pPluginManager->GetNowTime())
	{
		return;
	}
	mLastRoomCheckTime = pPluginManager->GetNowTime();

	time_t curTime = pPluginManager->GetNowTime();
	std::list<int> removeList;
	const std::map<int, time_t>& roomMap = m_pChatModule->GetNoPlayerRoomMap();

	auto it = roomMap.begin();
	auto itEnd = roomMap.end();
	for (; it != itEnd; ++it)
	{
		//1天
		if (curTime - it->second < m_pChatServerInit->GetDelayDeleteRoomTime())
		{
			continue;
		}

		removeList.push_back(it->first);
	}

	auto remove_it = removeList.begin();
	auto remove_itEnd = removeList.end();
	for (; remove_it != remove_itEnd; ++remove_it)
	{
		m_pChatModule->RemoveRoom(m_pChatModule->GetRoomKey(*remove_it));
	}
}

//客户端取聊天记录
void ChatNetMsgModule::OnGetChatRecording(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ChatRecording xRecording;
	if (!xRecording.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ChatRecording ParseFromArray failed";
		return;
	}

	const std::vector<Room::RowData*>* pRecordingVec = m_pChatModule->GetChatRecording(xRecording.room_id());
	if (nullptr == pRecordingVec)
	{
		return;
	}

	int start = xRecording.start();
	int count = xRecording.count();
	if (start < 0 || start >= pRecordingVec->size())
	{
		return;
	}

	int vecSize = (int)pRecordingVec->size();
	for (int i = (vecSize - 1) - start; i >= 0; --i)
	{
		if (count == 0)
		{
			break;
		}
		*xRecording.add_msg_list() = pRecordingVec->at(i)->content;
		--count;
	}

	m_pNetServerOuterModule->SendMsg(OuterMsg::CS_CHAT_RECORDING, xRecording, nSockIndex);
}