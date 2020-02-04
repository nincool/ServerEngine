///--------------------------------------------------------------------
/// �ļ���:		NetServerInsideModule.h
/// ��  ��:		�ڲ�����֮���õ��������ģ��
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
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

	// ȡ�����з�����ķ����������б�
	virtual bool GetTypeServerAll(int nType, std::vector<const InsideServerData*>& mapValue);
	// ȡ��ָ��������ķ����������б�
	virtual bool GetTypeServer(int serverID, int nType, std::vector<const InsideServerData*>& mapValue);

	// ͨ��SOCKID ����ĳ���̶�������
	virtual const InsideServerData* GetSockIDServer(int nSockIndex);
	// ȡ��ͬ��ָ����������Ϣ
	virtual const InsideServerData* GetSameGroupServer(int app_id, int type);

	// ��ͬ�����ͷ�����������Ϣ
	virtual bool SendMsgToTypeServer(const NF_SERVER_TYPES eType, int nMsgID, std::string& strData);
	// ��ͬ�����ͷ�����������Ϣ
	virtual bool SendMsgToTypeServer(const NF_SERVER_TYPES eType, int nMsgID, google::protobuf::Message& xData);

	virtual void SetReport(std::string& report);
	virtual void UpReport(bool force = false);
private:
	void OnSocketEvent(const NFSOCK nSockIndex, const NET_EVENT eEvent);
	void OnReport(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// ����ĳ���̶�������
	virtual InsideServerData* GetServerCfg(int server_id, int app_id, int district_id, int type);

private:
	std::map<int, InsideServerData*> mServerMapSock;
	//SERVER_ID, SERVER_TYPE, SERVER
	std::map<int, std::map<int, std::vector<InsideServerData*>>> mServerMap;

	//����
	std::string mstrReport;
	//�ϴα���
	NFINT64 mnLastReportTime = 0;
	//���İ汾��
	int64_t mReportVersion = 0;

	int64_t mlastTime = 0;

	NFIClassModule* m_pClassModule = nullptr;
};

#endif //__H_NetServerInsideModule_H__