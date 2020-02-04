///--------------------------------------------------------------------
/// �ļ���:		RankModule.h
/// ��  ��:		
/// ˵  ��:		
/// ��������:	2019��9��20��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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
	RANK_STATE_INIT_REQ,  // �����ʼ��
	RANK_STATE_INIT_ING,	// ���ڳ�ʼ��
	RANK_STATE_INIT_FINISH, // ��ʼ�����
};

class RankModule : public NFIModule
{
public:
	RankModule(NFIPluginManager* p)
    {
        pPluginManager = p;

		// ��ʼ���������
		srand(GetTickCount());
    }
    virtual bool Init();
	virtual bool InitLua();
    virtual bool Shut();
    virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

	// ��������
	void CreateServer(const NetData& net_data);
	// ���������Ϣ
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
	
	// �����������ڲ��ύ��������������ڰ���ʹ�ÿշ������ۼ�1��ʱ��������ϰ�
	// ����SetScoreͬʱʹ�ã���Ȼ��õ����ҵ�����
	// ֻ�ô˺����������ݣ���ض�ֻ����ʱ�������ÿ�β�������ʱ��������1,��֤���ظ�
	bool SwapRank(OuterMsg::ExtraMsg& xMsgBase);
	// ��ȡ�������ݣ�������ڰ��򷵻���Ч�Ŀշ��������ۼ�1��ʱ������ҷ���ֵΪfalse
	bool GetRankListScore(const std::string& strListName, const NFGUID& key, OuterMsg::RankListScore& xScore);

	bool OnHttpGetData(const NFHttpRequest& req);
	bool OnHttpGetDataArray(const NFHttpRequest& req);
	bool OnHttpGetRank(const NFHttpRequest& req);
	bool OnHttpGetRange(const NFHttpRequest& req);

	void SetNetClientReport(bool bInitComplete);
private:
	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
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
