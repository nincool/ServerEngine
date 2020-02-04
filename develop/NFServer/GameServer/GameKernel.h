///--------------------------------------------------------------------
/// �ļ���:		GameKernel.h
/// ��  ��:		GameServer����ģ��
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_GAMEKERNEL_H__
#define __H_GAMEKERNEL_H__

#include "NFComm/NFPluginModule/IGameKernel.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "NFComm/NFPluginModule/NFILetterModule.h"
#include "GameServer.h"
#include "PlayerRenameModule.h"
#include "PubDataModule.h"
#include "NFComm/NFPluginModule/ICreateRoleModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NFCore/NFPlayer.h"

class CGameKernel : public IGameKernel
{
public:
	typedef std::unordered_map<NFGUID, NF_SHARE_PTR<NFPlayer>> HashPlayer;
public:
	CGameKernel(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual ~CGameKernel() {};

	//��ʼ��
	virtual bool Init();

	//��ʼ����
	virtual bool AfterInit();

	virtual NFIKernelModule* CKernel();

	//��COMMADND���������
	virtual bool Command(int nCommandId, const NFGUID& self, const NFGUID& target, const NFDataList& msg);

	//��COMMADND�����������Ҷ���
	virtual bool CommandByWorld(int nCommandId, const NFGUID& self, const NFDataList& msg);

	/// \breif   AddCustomCallBack ��ӿͻ�����Ϣ�ص�
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod);

	/// \breif   Custom ����Ϣ������
	virtual bool Custom(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg);
	virtual bool Custom(int nMsgId, const NFGUID& self, const std::string& msg);

	/// \breif   CustomByWorld ����Ϣ��������Ҷ���
	virtual bool CustomByWorld(int nMsgId, const google::protobuf::Message& msg);

	///////////////////////////////////////////////////////////////////�ż���� BEGIN////////////////////////////////////////////////////////////////////
	/// \brief �����ż�
	/// \param sender ������(�����Ч)
	/// \param sendName ������
	/// \param receiver ������
	/// \param strTitle ����
	/// \param strContent ����
	/// \param params ���ݲ���
	/// \param strAppend ����
	virtual bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend);

	/// \brief �鿴�ż�����
	/// \param self ����
	virtual bool QueryLetter(const NFGUID& self);

	/// \brief �鿴�ż���Ϣ
	/// \param self ����
	/// \param num ����
	virtual bool LookLetter(const NFGUID& self, const int num);

	/// \brief ɾ���ż�
	/// \param self ����
	/// \param strLetterID �ż�id(Ϊ��ɾ��ȫ��)
	virtual bool DeleteLetter(const NFGUID& self, std::string strLetterID);
	///////////////////////////////////////////////////////////////////�ż���� END////////////////////////////////////////////////////////////////////

	// ��ù���GUID
	virtual const NFGUID& GetPubGuid(const std::string& name);
	// ��ù���GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name);
	// Message�¼���������
	virtual bool CommandToPub(int nCommandId, const NFGUID& sendobj, const NFDataList& msg);
	// Message�¼���������Guid
	virtual bool CommandToGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg);

	//д�����ݿ���־
	virtual void LogToDB(const std::string& recName, const NFDataList& col);

	// �༭�������
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp);

	//��ȡ����/Guid
	virtual const std::string& SeekRoleName(const NFGUID& self) const;
	virtual const NFGUID& SeekRoleGuid(const std::string& name) const;
	// ��ȡ�������Guid
	virtual const std::unordered_map<NFGUID, std::string>& GetRoleGuids() const;
	// ��ȡ�����������
	virtual const std::unordered_map<std::string, NFGUID>& GetRoleNames() const;

	// ��ȡ���
	virtual NF_SHARE_PTR<NFPlayer> GetPlayer(const NFGUID& player) const;
	// ��ȡ�������
	virtual const HashPlayer& GetPlayerAll() const;
	// ������Ǽ�
	virtual bool AddPlayer(NF_SHARE_PTR<NFPlayer> pPlayer);
	// ɾ�����
	virtual bool RemovePlayer(const NFGUID& player);
	// ��ͣ��ɫ
	virtual bool ClosurePlayer(const NFGUID& player, int64_t dnOpenTime);

protected:
	// ͨ�ý�����Ϣ
	void OnRecvMsgFunction(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	/// \breif   PlayerRename ��Ҹ���
	virtual bool PlayerRename(const NFGUID& self, const std::string& strName);

	// ����GUID Name
	void OnLoadPlayerGuidName(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// �����ʼ���ɹ�
	int OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

	int OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	int OnPlayerDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args);

private:

	// ����ӳ��GUID
	std::unordered_map<std::string, NFGUID> m_mapNameGuid;
	// GUIDӳ������
	std::unordered_map<NFGUID, std::string> m_mapGuidName;

	// ��Ҽ���
	HashPlayer m_hashPlayer;

	//gameServer ��������ģ��
	CGameServer* m_pGameServer = nullptr;
	//ͨ���ں�ģ��
	NFIKernelModule* m_pKernelModule = nullptr;
	//����������������
	NFIClassModule* m_pClassModule = nullptr;
	NFILetterModule* m_pLetterModule = nullptr;
	//��Ҹ���
	PlayerRenameModule* m_pPlayerRenameModule = nullptr;
	// pub����
	CPubDataModule* m_pPubDataModule = nullptr;

	NF_SHARE_PTR<NFIClass> pPlayerClass = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	ICreateRoleModule* m_pCreateRoleModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif //__H_KernelGameServer_H__