///--------------------------------------------------------------------
/// 文件名:		RankModule.h
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年9月20日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef NF_RANK_MODULE_H
#define NF_RANK_MODULE_H

#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFIEventModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "SkipList.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"

enum RankState
{
	RANK_STATE_INIT_REQ,  // 请求初始化
	RANK_STATE_INIT_ING,	// 正在初始化
	RANK_STATE_INIT_FINISH, // 初始化完毕
};

class RankModule : public NFIModule
{
public:
	RankModule(NFIPluginManager* p)
    {
        pPluginManager = p;

		// 初始化随机种子
		srand(GetTickCount());
    }
    virtual bool Init();
	virtual bool InitLua();
    virtual bool Shut();
    virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// 创建服务
	void CreateServer(const NetData& net_data);
	// 添加链接信息
	void ConnectServer(const ConnectCfg& connect_data);

	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	int GetTypePort(ENUM_NET_TYPE type);

protected:
	void OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDBRunturnMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void SendAckMessage(const std::string& strMsgBase, OuterMsg::ExtraMsg& xMsgBase);
	void OnDBRankListInit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnMaintaninMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	bool CreateRankList(OuterMsg::ExtraMsg& xMsgBase, bool bInit = false);
	bool DeleteRankList(OuterMsg::ExtraMsg& xMsgBase);
	bool SetScore(OuterMsg::ExtraMsg& xMsgBase);
	OuterMsg::RankListCode SetScore(OuterMsg::RankListScore& xScore, bool bInit = false);
	bool SetData(OuterMsg::ExtraMsg& xMsgBase);
	bool GetData(OuterMsg::ExtraMsg& xMsgBase);
	bool GetDataArray(OuterMsg::ExtraMsg& xMsgBase);
	bool GetRank(OuterMsg::ExtraMsg& xMsgBase);
	bool GetRankArray(OuterMsg::ExtraMsg& xMsgBase);
	bool GetRange(OuterMsg::ExtraMsg& xMsgBase);
	
	// 交换排名，内部会交换分数，如果不在榜，则使用空分数及累加1的时间戳尝试上榜
	// 不与SetScore同时使用，不然会得到混乱的排序
	// 只用此函数插入数据，则必定只根据时间戳排序，每次插入数据时间戳都会加1,保证不重复
	bool SwapRank(OuterMsg::ExtraMsg& xMsgBase);
	// 获取分数数据，如果不在榜，则返回有效的空分数，及累加1的时间戳，且返回值为false
	bool GetRankListScore(const std::string& strListName, const NFGUID& key, OuterMsg::RankListScore& xScore);

	bool OnHttpGetData(const NFHttpRequest& req);
	bool OnHttpGetDataArray(const NFHttpRequest& req);
	bool OnHttpGetRank(const NFHttpRequest& req);
	bool OnHttpGetRange(const NFHttpRequest& req);

	void SetNetClientReport(bool bInitComplete);
private:
	//创建的服务
	vector<NetData> mxNetList;
	//连接的服务器
	vector<ConnectCfg> mxConnectList;

	RankState m_state = RANK_STATE_INIT_REQ;

	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIHttpServerModule* m_pHttpNetModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetServerMaintainModule* m_pMaintainNetServer = nullptr;

	std::unordered_map<std::string, NF_SHARE_PTR<SkipList>> m_mapRankList;
};
#endif
