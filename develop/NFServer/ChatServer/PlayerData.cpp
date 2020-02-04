///--------------------------------------------------------------------
/// 文件名:		PlayerData.cpp
/// 内  容:		聊天服务器在线家玩数据
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "PlayerData.h"

//初始化
void PlayerData::Init(const NFGUID& uid)
{
	mGUID = uid;
}

const NFGUID& PlayerData::GetGUID()
{
	return mGUID;
}

//所在房间ID
const std::set<int>& PlayerData::GetRoomIdSet()
{
	return mHaveRoomIdSet;
}

//增加新房间ID
void PlayerData::EnterRoom(int nRooId)
{
	mHaveRoomIdSet.insert(nRooId);
}

//删除一个加入房间ID记录
void PlayerData::LeaveRoom(int nRooId)
{
	mHaveRoomIdSet.erase(nRooId);
}

//是否加入过指定的房间
bool PlayerData::IsMemberByRoom(int nRooId)
{
	if (mHaveRoomIdSet.end() == mHaveRoomIdSet.find(nRooId))
	{
		return false;
	}

	return true;
}

//设置网络ID
void PlayerData::SetSockIndex(NFSOCK nSockIndex)
{
	mSockIndex = nSockIndex;
}

//取得网络ID
NFSOCK PlayerData::GetSockIndex()
{
	return mSockIndex;
}

//重置
void PlayerData::Rese()
{
	mSockIndex = -1;
	mGUID = NFGUID();
	mHaveRoomIdSet.clear();
	Banned = false;
	Info = "";
}