///--------------------------------------------------------------------
/// 文件名:		NetServerInsideModule.h
/// 内  容:		内部进程之间用的网络服务模块
/// 说  明:		
/// 创建日期:	2019年11月5日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_NetServerInsideModule_H__
#define __H_NetServerInsideModule_H__
#include "NetServerModuleBase.h"
#include "NFComm/NFPluginModule/KConstDefine.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"

struct InsideServerData
{
	NFSOCK nSockIndex = -1;
	OuterMsg::ServerInfoReport report;
	int64_t mReportVersion = 0;
};

class NetServerInsideModule : public NetServerModuleBase
{
public:
	NetServerInsideModule(NFIPluginManager* p);

	virtual bool Init();

	virtual bool ReadyExecute();
	virtual bool Execute();

	virtual bool InitNet(const size_t bufferMax,
		const int nMaxClient, const std::string& ip, const int port);

	// 取得所有服务组的服务器类型列表
	virtual bool GetTypeServerAll(int nType, std::vector<const InsideServerData*>& mapValue);
	// 取得指定服务组的服务器类型列表
	virtual bool GetTypeServer(int serverID, int nType, std::vector<const InsideServerData*>& mapValue);

	// 通过SOCKID 查找某个固定服务器
	virtual const InsideServerData* GetSockIDServer(int nSockIndex);
	// 取得同组指定服务器信息
	virtual const InsideServerData* GetSameGroupServer(int app_id, int type);

	// 给同组类型服务器发送信息
	virtual bool SendMsgToTypeServer(const NF_SERVER_TYPES eType, int nMsgID, std::string& strData);
	// 给同组类型服务器发送信息
	virtual bool SendMsgToTypeServer(const NF_SERVER_TYPES eType, int nMsgID, google::protobuf::Message& xData);

	virtual void SetReport(std::string& report);
	virtual void UpReport(bool force = false);
private:
	void OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);
	void OnReport(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// 查找某个固定服务器
	virtual InsideServerData* GetServerCfg(int server_id, int app_id, int district_id, int type);

private:
	std::map<int, InsideServerData*> mServerMapSock;
	//SERVER_ID, SERVER_TYPE, SERVER
	std::map<int, std::map<int, std::vector<InsideServerData*>>> mServerMap;

	//报文
	std::string mstrReport;
	//上次报文
	NFINT64 mnLastReportTime = 0;
	//报文版本号
	int64_t mReportVersion = 0;

	int64_t mlastTime = 0;

	NFIClassModule* m_pClassModule = nullptr;
};

#endif //__H_NetServerInsideModule_H__