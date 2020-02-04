///--------------------------------------------------------------------
/// 文件名:		ChatServerInit.h
/// 内  容:		聊天服务器初始化
/// 说  明:		
/// 创建日期:	2019年11月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_ChatServerInit_H__
#define __H_ChatServerInit_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerOuterModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"

class NFMysqlModule;
class ChatServerInit : public NFIModule
{
public:
	ChatServerInit(NFIPluginManager* p);

	//管理器调用初始化
	virtual bool Init();

	// 脚本初始化
	virtual bool InitLua();

	// 服务网络初始化
	virtual bool InitNetServer();

	// 客户网络初始化
	virtual bool InitNetClient();

	// 使用加密
	void SetUseEncrypt(bool value) { m_bUseEncrypt = value; }
	bool GetUseEncrypt() const { return m_bUseEncrypt; }

	// Mysql连接
	void SetMysqlConnect(std::string& value) { m_nMysqlConnect = value; }
	std::string& GetMysqlConnect() { return m_nMysqlConnect; }

	const std::string& GetDBName() { return m_strDBName; }

	//每个房间聊天记录保存最大条数
	void SetRecordingMax(int value) { m_RecordingMax = value; }
	int GetRecordingMax() const { return m_RecordingMax; }

	//每个玩家最大离线消息条数
	void SetOfflineMsgMax(int value) { m_OfflineMsgMax = value;}
	int GetOfflineMsgMax() const { return m_OfflineMsgMax; }

	//每个玩家离线消息最大保存时长(秒)
	void SetOfflineMsgCacheTimeMax(int value) { m_OfflineMsgCacheTimeMax = value; }
	int GetOfflineMsgCacheTimeMax() const { return m_OfflineMsgCacheTimeMax; }

	//发消息最大频率(秒)
	void SetSendFrequencyMax(int value) { m_SendFrequencyMax = value; }
	int GetSendFrequencyMax() const { return m_SendFrequencyMax; }

	//没客户端的最大保存时间
	void SetDelayDeletePlayerTime(int value) { mDelayDeletePlayerTime = value; }
	time_t GetDelayDeletePlayerTime() const { return mDelayDeletePlayerTime; }

	//删除长时间没玩家的房间
	void SetDelayDeleteRoomTime(int value) { mDelayDeleteRoomTime = value; }
	time_t GetDelayDeleteRoomTime() const { return mDelayDeleteRoomTime; }

	// 创建服务
	void CreateServer(const NetData& net_data);
	// 添加链接信息
	void ConnectServer(const ConnectCfg& connect_data);

	//更新程序报文
	void UpReport(bool force = false);
private:
	// 开启数据库连接
	void InitDBConnect();

	// 解析数据库连接
	bool ParseDBConnect(std::string& DBConnect, std::string& strDBPassword,
		std::string& strDBUser, std::string& strDBIP, int& nDBPort);

private:
	//是否加密
	bool m_bUseEncrypt = false;
	//Mysql连接
	std::string m_nMysqlConnect = "";
	//数据库名
	std::string m_strDBName;
	//每个房间聊天记录保存最大条数
	int m_RecordingMax = 500;
	//每个玩家最大离线消息条数
	int m_OfflineMsgMax = 60;
	//每个玩家离线消息最大保存时长(秒)
	int m_OfflineMsgCacheTimeMax = 604800;
	//发消息最大频率(秒)
	int m_SendFrequencyMax = 2;
	//没客户端的最大保存时间
	time_t mDelayDeletePlayerTime = 180;
	//删除长时间没玩家的房间
	time_t mDelayDeleteRoomTime = 86400;

	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerOuterModule* m_pNetServerOuterModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFMysqlModule* m_pMysqlModule = nullptr;
};

#endif	//__H_ChatServerInit_H__