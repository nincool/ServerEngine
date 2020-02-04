///--------------------------------------------------------------------
/// �ļ���:		PubKernel.h
/// ��  ��:		���������ģ��
/// ˵  ��:		
/// ��������:	2019��8��26��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __PUBKERNEL_H
#define __PUBKERNEL_H

#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/IPubKernel.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "PubServerState.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

// ��������״̬
enum PUB_STATE_ENUM
{
	PUBSTATE_UNKNOWN,	// ��ʼ״̬
	PUBSTATE_SAVING,	// ������
};

// ��������
struct PubData
{
	int nState = PUBSTATE_UNKNOWN; // ״̬
	int nSavingSec = 0; // ����ȴ�����
};

class CPubKernel : public IPubKernel
{
public:
	CPubKernel(NFIPluginManager* p);
    virtual ~CPubKernel();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();
	
	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel();

	// ��������
	virtual NF_SHARE_PTR<NFIObject> CreatePub(const std::string& pub_name, bool bSave = false);
	// ��������
	virtual NF_SHARE_PTR<NFIObject> CreateGuild(const std::string& guild_name);
	// ɾ��������
	virtual bool DeletePub(const std::string& pub_name);
	// ɾ������
	virtual bool DeleteGuild(const std::string& guild_name);
	// ��ù���GUID
	virtual const NFGUID& GetPubGuid(const std::string& name);
	// ��ù���GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name);
	// �ֶ�����������
	virtual bool SavePubData(const std::string& name, const NFGUID& object_id);
	// �ֶ����湫������
	virtual bool SaveGuildData(const NFGUID& object_id);
	// Message�¼������
	virtual bool CommandToPlayer(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg);
	// Message����������
	virtual bool CommandToGame(int nCommandId, const NFGUID& sendobj, const NFDataList& msg);
	// Command�¼����ڲ�����
	virtual bool Command(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg);
	// Command�¼�����������
	virtual bool CommandToHelper(int nCommandId, const NFGUID& sendobj, const NFDataList& msg);

	// ��������Pub����
	bool SaveAllPubData();

	// ��ȡ�����������
	int GetSavePubCount() { return m_mapPubInfo.size(); }
private:
	void OnReqSynServerInfo(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadGuildData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadPubData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckSaveObjectData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// guild�������
	int OnGuildRecover(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// pub�������
	int OnPubRecover(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	void OnCommandTransmit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	int SavePubDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	int SaveGuildDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);

	// �������
	bool OnMapAddIndex(map<std::string, NFGUID>& mapInfo, const std::string& key, const NFGUID& id);
	// ����ͬ�����߼��˶�������
	void SynDataToGame(const NFGUID& id, const int nMsgID);
	// ��ʾ��������
	void SynAllMapData(map<std::string, NFGUID>& mapValue, const int nMsgID);
	// �·�����Guid����
	void SynAllGuidInfo(map<std::string, NFGUID>& mapInfo, const int nMsgID);

	bool RemoveObject(const NFGUID& self);
	bool InnerSavePubData(const std::string& name, const NFGUID& object_id, bool bExit = false);
	bool InnerSaveGuildData(const NFGUID& object_id, bool bExit = false);
private:
	//����Ŵ洢
	map<std::string, NFGUID> mxMapPubGuid;
	map<std::string, NFGUID> mxMapGuildGuid;

	NFINT64 nLastTime = 0;
	std::map<NFGUID, PubData> m_mapPubInfo;
	//������ɾ��������
	std::list<NFGUID> m_listRemovePub;

	//ͨ���ں�ģ��
	NFIKernelModule* m_pKernelModule = nullptr;
	//ClientNet
	NetClientModule* m_pNetClientModule = nullptr;
	//����
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	CPubServerState* m_pPubServerState = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
