///--------------------------------------------------------------------
/// �ļ���:		IKernelGameServer.h
/// ��  ��:		GameServer����ģ��
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __NFI_GAMEKERNEL__
#define __NFI_GAMEKERNEL__

#include "NFIKernelModule.h"

#define IGAME_LOGIC_VERSION 0x0001

class IGameKernel : public NFIModule
{
public:
	virtual ~IGameKernel() {};

	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel() = 0;

	/// \breif   Command ���ڲ���Ϣ������
	/// \Access  virtual public 
	/// \param   int nMsgId ��Ϣ��
	/// \param   const NFGUID & self ������
	/// \param   const NFGUID & target ������
	/// \param   const NFDataList & msg �����б�
	/// \return  bool �Ƿ�ɹ�
	virtual bool Command(int nMsgId, const NFGUID& self, const NFGUID& target, const NFDataList& msg) = 0;

	/// \breif   CommandByWorld ���ڲ���Ϣ��������Ҷ���
	/// \Access  virtual public 
	/// \param   int nMsgId ��Ϣ��
	/// \param   const NFGUID & self ������
	/// \param   const NFDataList & msg ��Ϣ�б�
	/// \return  bool �Ƿ�ɹ�
	virtual bool CommandByWorld(int nMsgId, const NFGUID& self, const NFDataList& msg) = 0;

	/// \breif   AddCustomCallBack ��ӿͻ�����Ϣ�ص�
	/// \Access  virtual public 
	/// \param   int nMsgId ��Ϣ��
	/// \param   NF_SHARE_PTR pMethod ��Ϣ�ص���װ���������ָ�� ͨ���� METHOD_MSG ����
	/// \return  bool
	virtual bool AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod) = 0;

	/// \breif   Custom ����Ϣ������
	/// \Access  virtual public 
	/// \param   int nMsgId ��Ϣ��
	/// \param   const NFGUID & self ������
	/// \param   const google::protobuf::Message& msg ��ϢPB�ṹ
	/// \return  bool �Ƿ�ɹ�
	virtual bool Custom(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg) = 0;
	virtual bool Custom(int nMsgId, const NFGUID& self, const std::string& msg) = 0;

	/// \breif   CustomByWorld ����Ϣ��������Ҷ���
	/// \Access  virtual public 
	/// \param   int nMsgId ��Ϣ��
	/// \param   const google::protobuf::Message& msg ��ϢPB�ṹ
	/// \return  bool �Ƿ�ɹ�
	virtual bool CustomByWorld(int nMsgId, const google::protobuf::Message& msg) = 0;

	/// \breif   PlayerRename ��Ҹ���
	/// \Access  virtual public 
	/// \param   const NFGUID& self ���
	/// \param   const std::string& strName ����
	/// \return  bool �Ƿ�ɹ�
	virtual bool PlayerRename(const NFGUID& self, const std::string& strName) = 0;

	///////////////////////////////////////////////////////////////////�ż���� BEGIN//////////////////////////////////////////////////////////////////////
	/// \brief �����ż�
	/// \param sender ������(�����Ч)
	/// \param sendName ������
	/// \param receiver ������
	/// \param strTitle ����
	/// \param strContent ����
	/// \param params ���ݲ���
	/// \param strAppend ����
	virtual bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend) = 0;

	/// \brief �鿴�ż�����
	/// \param self ����
	virtual bool QueryLetter(const NFGUID& self) = 0;

	/// \brief �鿴�ż���Ϣ
	/// \param self ����
	/// \param num ����
	virtual bool LookLetter(const NFGUID& self, const int num) = 0;

	/// \brief ɾ���ż�
	/// \param self ����
	/// \param strLetterID �ż�id(Ϊ��ɾ��ȫ��)
	virtual bool DeleteLetter(const NFGUID& self, std::string strLetterID) = 0;
	///////////////////////////////////////////////////////////////////�ż���� END//////////////////////////////////////////////////////////////////////

	/// \breif   GetPubGuid ��ù�����GUID
	/// \Access  virtual public 
	/// \param   const std::string& name ����
	/// \return  NFGUID& ��ӦGUID
	virtual const NFGUID& GetPubGuid(const std::string& name) = 0;

	/// \breif   GetGuildGuid ��ù���GUID
	/// \Access  virtual public 
	/// \param   const std::string& name ����
	/// \return  NFGUID& ��ӦGUID
	virtual const NFGUID& GetGuildGuid(const std::string& name) = 0;

	/// \breif   CommandToPub ����Ϣ��PUB
	/// \Access  virtual public 
	/// \param   int nCommandId ��Ϣ��
	/// \param   const NFGUID& sendobj ���Ͷ���
	/// \param   const NFDataList& msg ������Ϣ
	/// \return  bool �Ƿ�ɹ�
	virtual bool CommandToPub(int nCommandId, const NFGUID& sendobj, const NFDataList& msg) = 0;

	/// \breif   CommandToGuid ����Ϣ��������Guid
	/// \Access  virtual public 
	/// \param   int nCommandId ��Ϣ��
	/// \param   const NFGUID& sendobj ���Ͷ���
	/// \param   const NFGUID& targetobj ���ܶ���
	/// \param   const NFDataList& msg ������Ϣ
	/// \return  bool �Ƿ�ɹ�
	virtual bool CommandToGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg) = 0;

	//д�����ݿ���־
	virtual void LogToDB(const std::string& recName, const NFDataList& col) = 0;

	/// \breif   AlterPlayer �༭�������,�����Ҵ�����ֱ�ӻص���Ϣ������ˢ�´���ʱ��,
	/// \breif   ����ʹ�ô˹�����ò�Ҫ�ж�����Ƿ��ܹ���ȡ��,ֱ�ӵ���
	/// \Access  virtual public 
	/// \param   const NFGUID & player ��Ҫ�༭�����
	/// \param   int nMsgID �ص�Command��Ϣ
	/// \param   const NFGUID & sender 
	/// \param   const NFDataList & args �������� ֱ�Ӵ��ݸ��ص���Ϣ
	/// \return  bool
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp) = 0;

	/// \brief ��ȡ��ɫ����
	/// \Access  virtual public 
	/// \param   const NFGUID & self ��ɫ����
	/// \return  const std::string&
	virtual const std::string& SeekRoleName(const NFGUID& self) const = 0;

	/// \brief ��ȡ��ɫGuid
	/// \Access  virtual public 
	/// \param   const std::string& name ��ɫ����
	/// \return  const NFGUID
	virtual const NFGUID& SeekRoleGuid(const std::string& name) const = 0;

	// ��ȡ�������Guid
	virtual const std::unordered_map<NFGUID, std::string>& GetRoleGuids() const = 0;
	// ��ȡ�����������
	virtual const std::unordered_map<std::string, NFGUID>& GetRoleNames() const = 0;

	/// \breif   ClosurePlayer ��ͣ��ɫ
	/// \Access  virtual public 
	/// \param   const NFGUID & player ��ɫid
	/// \param   int64_t dnOpenTime �����¼ʱ��(��ͣ��ֹʱ���)
	/// \return  bool
	virtual bool ClosurePlayer(const NFGUID& player, int64_t dnOpenTime) = 0;

};

#endif //__NFI_GAMEKERNEL__