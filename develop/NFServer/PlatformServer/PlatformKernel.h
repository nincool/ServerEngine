///--------------------------------------------------------------------
/// �ļ���:		PlatformKernel.h
/// ��  ��:		ƽ̨������ģ��
/// ˵  ��:		
/// ��������:	2019��10��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __PLATFORM_KERNEL_H
#define __PLATFORM_KERNEL_H
#include "NFComm/NFPluginModule/IPlatformKernel.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "PlatformServerState.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPlatformKernel : public IPlatformKernel
{
public:
	CPlatformKernel(NFIPluginManager* p);
    virtual ~CPlatformKernel();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();

	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel();

	// �������������
	virtual NF_SHARE_PTR<NFIObject> GetPlatformHelper() { return m_pPlatformObject; }

	// ����ƽ̨������
	virtual bool SavePlatformData(const NFGUID& object_id);

	// ɾ��ƽ̨������
	virtual bool DeletePlatformData(const NFGUID& object_id);

	// ����ս��
	virtual NF_SHARE_PTR<NFIObject> OnStartBattle(const std::string& sBattleName, const NFDataList& args);

	// ������Ϣ����Ϸ��GUID
	virtual bool SendCommandToGameGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args);

	// ������Ϣ��Game
	virtual bool SendCommandToGame(int nCommandId, int world_id, const NFGUID& sendobj, const NFDataList& args);

	// ������Ϣ������Game
	virtual bool SendCommandToAllGame(int nCommandId, const NFGUID& sendobj, const NFDataList& args);

	// ������Ϣ��ս����
	virtual bool SendCommandToBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& battle_guid, const NFDataList& args);
	
	bool SaveAllPlatformData();
	
private:
	void OnAckStartLoadPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckCreatePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadPlatformData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnAckLoadPlatformComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	int SavePlatformDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount);
	// battlehelper�������
	int OnBattleDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	bool OnMapAddIndex(map<NFGUID, std::string>& mapInfo, const NFGUID& key, const std::string& id);

private:
	//����Ŵ洢
	map<NFGUID, std::string> mxMapPlatformGuid;
	// ����������
	NF_SHARE_PTR<NFIObject> m_pPlatformObject;

	//ͨ���ں�ģ��
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	CPlatformServerState* m_pPlatformServerState = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif
