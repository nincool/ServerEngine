///--------------------------------------------------------------------
/// 文件名:		NFPlayer.h
/// 内  容:		玩家
/// 说  明:		
/// 创建日期:	2019年7月30日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NF_PLAYER_H__
#define __NF_PLAYER_H__

#include "NFVisible.h"
#include "NFViewport.h"

 class _NFExport NFPlayer : public NFVisible
 {
 public:
	 enum {VIEWPORT_MAX_NUM = 128};
 private:
	 NFPlayer() : NFVisible(NFGUID(), nullptr) {};

 public:
	 NFPlayer(NFGUID self, NFIPluginManager* pPluginManager);
	virtual ~NFPlayer();

	virtual bool IsPlayer() const { return true; }

	// 添加容器视窗
	bool AddViewport(int id, NFVisible* pContainer);
	// 删除容器视窗
	bool RemoveViewport(int id);
	// 查找容器视窗
	bool FindViewport(int id);
	// 删除所有容器视窗
	void ClearViewport();
	// 获取容器视窗对应的容器
	NFVisible* GetViewContainer(int id);
	// 重新发送所有容器视窗
	void ResendViewports();

	// 触发客户端消息
	int OnCustom(int nMsgId, const std::string& strMsg);

	// 内部属性
	RAW_INT(GameID); // 游戏服ID
	RAW_INT(GateID); // 网关服ID
	RAW_OBJECT(ClientID); // 客户端连接ID
	RAW_STR(Account); // 账号
	RAW_STR(IP); // ip
	RAW_INT(CreateTime); // 角色创建时间
	RAW_INT(OpenTime); // 允许登录时间
	RAW_INT(TotalSec); // 总在线时间
	RAW_INT(OnlineTime); // 上线时间戳
	RAW_INT(OfflineTime);   // 下线时间戳
	RAW_STR(LiteData); // 摘要数据 登录时显示
	RAW_STR(DeviceID); // 设备ID
	RAW_STR(DeviceMAC); // 设备MAC地址
	RAW_INT(OsType);   // 设备系统类型
	RAW_INT(OnlineState);   // 在线状态

 private:
 	// 视窗
	 std::vector<NF_SHARE_PTR<NFViewport>> m_vecViewport;
 };

#endif // __NF_PLAYER_H__