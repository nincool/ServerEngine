///--------------------------------------------------------------------
/// �ļ���:		NetClientModule.h
/// ��  ��:		��Ϊ�ͻ������ӷ�����������
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_NetClientModule_H__
#define __H_NetClientModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NetClient.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include <map>
#include <queue>
#include "../NFPluginModule/ConnectData.h"

//���Ӷ����������
struct ConnectOBJ
{
	ConnectCfg ConnectCfg;
	ConnectState eState = ConnectState::DISCONNECT;
	NetClient* mpClientNet = nullptr;
	OuterMsg::ServerInfoReport report;
	int64_t mReportVersion = 0;
};

class NetClientModule : public NFIModule
{
public:
	NetClientModule(NFIPluginManager* p);

	bool Init();
	bool AfterInit();
	bool Execute();

	virtual void AddServerConnect(ConnectCfg& conCfg);
	virtual void RemoveServerConnect(ConnectCfg& conCfg);

	virtual void AddReceiveCallBack(const NF_SERVER_TYPES eType, NFIModule* pBase, NetMsgFun fun);
	virtual void AddReceiveCallBack(const NF_SERVER_TYPES eType, const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun);

	virtual void AddEventCallBack(const NF_SERVER_TYPES eType, NFIModule* pBase, NetEventFun fun);

	virtual bool SendMsgByType(const NF_SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData);
	virtual bool SendMsgByType(const NF_SERVER_TYPES eType, const uint16_t nMsgID, const google::protobuf::Message& msg);
	//��ͬSERVER_ID
	virtual bool SendMsg(const int appID, const NF_SERVER_TYPES eType, const uint16_t nMsgID, const std::string& strData);
	virtual bool SendMsg(const int appID, const NF_SERVER_TYPES eType, const uint16_t nMsgID, const google::protobuf::Message& msg);

	virtual bool SendMsgBySock(const int64_t nSockIndex, const uint16_t nMsgID, const std::string& strData);
	virtual bool SendMsgBySock(const int64_t nSockIndex, const uint16_t nMsgID, const google::protobuf::Message& msg);

	virtual const ConnectOBJ* GetConnectBySock(const int64_t nSockIndex);
	virtual const ConnectOBJ* GetConnect(const int serverID, const int serverType, const int serverAppID);
	virtual void GetConnectByType(const NF_SERVER_TYPES eType, std::vector<const ConnectOBJ*>& outVec);
	virtual void GetConnectAll(std::vector<const ConnectOBJ*>& outVec);

	virtual void SetReport(std::string& report);
	virtual void UpReport(bool force = false);

private:
	void OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);
	void OnReport(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	void ProcessConnect();
	ConnectOBJ* GetConnectData(const int64_t nSockIndex);
	ConnectOBJ* GetConnectData(const int serverID, const int serverType, const int serverAppID);

private:
	//connect_list;
	std::vector<ConnectOBJ*> mConnectVec;
	//server_id, server_type, server_appid, server
	std::map<int, std::map<int, std::map<int, ConnectOBJ*>>> mServerMap;
	//sockIndex server_data
	std::map<int64_t, ConnectOBJ*> mServerSockIndexMap;

	//�ص�
	std::map<int, NetCallback> mCallBackMap;
	//�ȴ������ж�
	std::queue<ConnectOBJ*> mWaitConnectQue;
	//�ϴ�����ʱ��
	NFINT64 mnLastConnectTime = 0;

	//����
	std::string mstrReport;
	//�ϴα���
	NFINT64 mnLastReportTime = 0;
	//���İ汾��
	int64_t mReportVersion = 0;

	NFIClassModule* m_pClassModule = nullptr;

};

#endif //__H_NetClientModule_H__