///--------------------------------------------------------------------
/// 文件名:		PlayerData.h
/// 内  容:		聊天服务器在线家玩数据
/// 说  明:		
/// 创建日期:	2019年8月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_PlayerData_H__
#define __H_PlayerData_H__

#include "NFComm\NFPluginModule\NFGUID.h"
#include <set>

class PlayerData
{
public:
	//初始化
	void Init(const NFGUID& uid);

	const NFGUID& GetGUID();

	//所在房间ID
	const std::set<int>& GetRoomIdSet();

	//加入房间ID记录
	void EnterRoom(int nRooId);

	//删除一个加入房间ID记录
	void LeaveRoom(int nRooId);

	//是否加入过指定的房间
	bool IsMemberByRoom(int nRooId);

	//设置网络ID
	void SetSockIndex(NFSOCK nSockIndex);
	//取得网络ID
	NFSOCK GetSockIndex();

	//重置
	void Rese();

private:
	NFGUID mGUID;
	//所在房间ID
	std::set<int> mHaveRoomIdSet;
	//网络链接ID
	NFSOCK mSockIndex = -1;
public:

	//禁言状态
	bool Banned = false;
	//自定义数据
	std::string Info;
};

#endif	//__H_PlayerData_H__