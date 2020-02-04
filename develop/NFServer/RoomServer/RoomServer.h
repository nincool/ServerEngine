///--------------------------------------------------------------------
/// �ļ���:		RoomServer.h
/// ��  ��:		�����
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef ROOM_SERVER_H
#define ROOM_SERVER_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "RoomServerState.h"
#include "NFComm/NFPluginModule/NetData.h"
#include "NFComm/NFPluginModule/ConnectData.h"
#include "Dependencies/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"

class CRoomServer : public NFIModule
{
public:
	CRoomServer(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();
	virtual bool InitLua();
	virtual bool Execute();
	virtual bool AfterInit();
	virtual bool InitNetServer();
	virtual bool InitNetClient();

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
	void OnServerCompleteAllNet(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnServerAllOpen(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCommandToRoom(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// �ͻ����������
	int OnClientNetState(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// ����Ƿ���ɱ������
	int CheckServerStatus();
private:
	bool bClientNetComplete = false;
	bool bServerNetComplete = false;

	//�����ķ���
	vector<NetData> mxNetList;
	//���ӵķ�����
	vector<ConnectCfg> mxConnectList;

	NetClientModule* m_pNetClientModule = nullptr;
	CRoomServerState* m_pRoomServerState = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NFIHttpServerModule* m_pHttpServerModule = nullptr;
};

#endif
