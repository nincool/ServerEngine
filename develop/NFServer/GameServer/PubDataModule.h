///--------------------------------------------------------------------
/// �ļ���:		PubDataModule.h
/// ��  ��:		gameӳ���pub����ģ��
/// ˵  ��:		
/// ��������:	2019��9��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NF_PUB_DATA_MODULE_H
#define NF_PUB_DATA_MODULE_H

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CPubDataModule : public NFIModule
{
public:
	CPubDataModule(NFIPluginManager* p);
    virtual ~CPubDataModule();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();
	
	// ��ù���GUID
	virtual const NFGUID& GetPubGuid(const std::string& name);
	// ��ù���GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name);
	// Command�¼���PUB
	virtual bool CommandToPub(int nCommandID, const NFGUID& sendobj, const NFDataList& msg);
	// Command�¼���GUID
	virtual bool CommandToGuid(int nCommandID, const NFGUID& sendobj, const NFGUID& targetobj, const std::string& strTargetClassName, const NFDataList& msg);

private:
	int OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	void AckStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void AckLoadData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeletePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeleteGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	// �������
	bool OnMapAddIndex(map<std::string, NFGUID>& mapInfo, const std::string& key, NFGUID& id);
	NF_SHARE_PTR<NFObject> LoadData(OuterMsg::ObjectDataPack* xObjectDataPack);
private:
	int64_t mLastCheckTime = 0;

	//����Ŵ洢
	map<std::string, NFGUID> mxMapPubGuid;
	map<std::string, NFGUID> mxMapGuildGuid;

	NetClientModule* m_pNetClientModule = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
};

#endif
