///--------------------------------------------------------------------
/// 文件名:		MasterServer.h
/// 内  容:		Master服务模块逻辑
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _MASTER_SERVER_H
#define _MASTER_SERVER_H

#include "NFComm/NFPluginModule/NFILogModule.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFServer/GameServer/MaintainMsgDataDefine.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

class CMasterServer : public NFIModule
{
public:
	CMasterServer(NFIPluginManager* p)
    {
		pPluginManager = p;
    }

    virtual bool Init();
	virtual bool InitLua();
    virtual bool Shut();
	virtual bool Execute();
    virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// 服务器备注信息存储
	void SetModifyServerFile(std::string& value) { m_nModifyServerFile = value; }
	std::string& GetModifyServerFile() { return m_nModifyServerFile; }

	// 是否是主服务器
	void SetIsMaster(bool value) { m_bMaster = value; }
	bool GetIsMaster() const { return m_bMaster; }

	// 创建服务
	void CreateServer(const NetData& net_data);
	// 添加链接信息
	void ConnectServer(const ConnectCfg& connect_data);

	// 获取某类型IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// 获取某类型Port
	int GetTypePort(ENUM_NET_TYPE type);

	//更新程序报文
	void UpReport(bool force = false);
private:
    void OnSelectWorldProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
    void OnSelectServerResultProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnWorldInfoToLogin();

	// 服务网络更改
	int OnServerStateChange(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	//读取配置信息
	void OnLoadModifyFile();
	//请求修改某个服务器备注信息
	bool OnModifyWorldServer(int server_id, std::string& value);
	//取得服务器信息列表
	void OnGetServerList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	bool GetServerList(const REQGetServerInfoList& req, ACKGetServerInfoList& ack);

	//服务器操作(修改状态)
	void OnOptSetServerNotes(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	//网络通信测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NFINT64 m_nCheckTime = 0;
	// 缓存数据，防止频繁创建
	std::vector<const InsideServerData*> mapValue;
	//备注信息修改
	std::map<int, std::string> mapModifyInfo;
	
	//服务器备注文件
	std::string m_nModifyServerFile = "";
	//是否是主Master
	bool m_bMaster = false;
	
	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

    NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerMaintainModule* m_pNetServerMaintainModule = nullptr;
};

#endif