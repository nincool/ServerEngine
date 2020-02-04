
///--------------------------------------------------------------------
/// �ļ���:		CreateRoleModule.h
/// ��  ��:		��ɫ����
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _CREATEROLE_MODULE_H
#define _CREATEROLE_MODULE_H

#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/ICreateRoleModule.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "GameServer.h"
#include "NFComm/NFPluginModule/IGameKernel.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

// ���������Ϣ
struct PlayerConnInfo
{
	int nGateID = 0;
	NFGUID xClientID = NULL_OBJECT;
	std::string strIP = "";
	std::string strAccount = "";
	std::string strDeviceMac = "";
	std::string strDeviceID = "";
	int nOsType = 0;
};

struct AlterPlayerInfo
{
	std::function<void (const NFGUID& self, const NFDataList& args)> fp;
	NFDataList args;// �����Ĳ���
	int64_t dnDeleteTime = 0; // ��ʱɾ��ʱ��
}; 

class CCreateRoleModule : public ICreateRoleModule
{
public:
	CCreateRoleModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual ~CCreateRoleModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();
    virtual bool AfterInit();

	// ��ȡ������Ϣ
	const PlayerConnInfo* GetConnInfo(const NFGUID& player) const;
	// ��ȡ����������Ϣ
	const std::map<NFGUID, PlayerConnInfo>& GetConnInfoMap() const;

	// �༭�������
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp);

protected:
	void OnReqiureRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnReposeRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreateRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckPreCreateRole(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnClienEnterGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDBLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnClientEnterGameFinishProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCommandPlayerRemove(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCommandPlayerOffline(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ���ؽ�ɫ
	bool LoadRole(const OuterMsg::ObjectDataPack& xMsg);

	// ��Ҵ����¼�
	int OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	// �༭�������
	virtual void OnAlterPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	virtual bool EnableAlterPlayer(const NFGUID& player);
	virtual bool DisableAlterPlayer(const NFGUID& player);
	// ��ʱ����༭�������
	int CB_AlterPlayerSaveData(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	// ��ʱ����༭���
	int CB_DestroyAlterPlayer(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);

private:
	// ɾ�����������Ϣ
	bool RemoveConnInfo(const NFGUID& nRoleID);
	// ��ȡ��Ҵ洢����
	bool GetData(const NFGUID& self, OuterMsg::ObjectDataPack& xRoleDataPack);
	// �洢�������
	void SaveData(const NFGUID& self, bool bExit = false);
	// ��ʱ�洢�������
	int SaveDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	// ֪ͨworld�������
	void SendOnline(const NFGUID& self);
	// ��ҽ�������
	bool PlayerConnect(const NFGUID& self);
	// ��ҶϿ�����
	bool PlayerDisconnect(const NFGUID& self);
private:
	std::map<NFGUID, PlayerConnInfo> m_mapConnInfo;
	std::map<NFGUID, std::list<AlterPlayerInfo>> m_mapAlterPlayer;

	NFIClassModule* m_pClassModule = nullptr;
	NFIElementModule* m_pElementModule = nullptr;
	NFKernelModule* m_pKernelModule = nullptr;
	IGameKernel* m_pGameKernel = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif