///--------------------------------------------------------------------
/// �ļ���:		MasterServer.h
/// ��  ��:		Master����ģ���߼�
/// ˵  ��:		
/// ��������:	2019��9��7��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
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

	// ��������ע��Ϣ�洢
	void SetModifyServerFile(std::string& value) { m_nModifyServerFile = value; }
	std::string& GetModifyServerFile() { return m_nModifyServerFile; }

	// �Ƿ�����������
	void SetIsMaster(bool value) { m_bMaster = value; }
	bool GetIsMaster() const { return m_bMaster; }

	// ��������
	void CreateServer(const NetData& net_data);
	// ���������Ϣ
	void ConnectServer(const ConnectCfg& connect_data);

	// ��ȡĳ����IP
	const std::string& GetTypeIP(ENUM_NET_TYPE type);
	// ��ȡĳ����Port
	int GetTypePort(ENUM_NET_TYPE type);

	//���³�����
	void UpReport(bool force = false);
private:
    void OnSelectWorldProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
    void OnSelectServerResultProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnWorldInfoToLogin();

	// �����������
	int OnServerStateChange(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	//��ȡ������Ϣ
	void OnLoadModifyFile();
	//�����޸�ĳ����������ע��Ϣ
	bool OnModifyWorldServer(int server_id, std::string& value);
	//ȡ�÷�������Ϣ�б�
	void OnGetServerList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	bool GetServerList(const REQGetServerInfoList& req, ACKGetServerInfoList& ack);

	//����������(�޸�״̬)
	void OnOptSetServerNotes(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	//����ͨ�Ų���
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NFINT64 m_nCheckTime = 0;
	// �������ݣ���ֹƵ������
	std::vector<const InsideServerData*> mapValue;
	//��ע��Ϣ�޸�
	std::map<int, std::string> mapModifyInfo;
	
	//��������ע�ļ�
	std::string m_nModifyServerFile = "";
	//�Ƿ�����Master
	bool m_bMaster = false;
	
	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
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