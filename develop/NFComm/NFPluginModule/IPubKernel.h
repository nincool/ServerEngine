///--------------------------------------------------------------------
/// �ļ���:		IPubKernel.h
/// ��  ��:		���������ģ��ӿ�
/// ˵  ��:		
/// ��������:	2019��8��26��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef I_PUB_KERNEL_H
#define I_PUB_KERNEL_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFIKernelModule.h"

#define IPUB_LOGIC_VERSION 0x0001

class IPubKernel : public NFIModule
{
public:
	virtual ~IPubKernel() {};

	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel() = 0;

	// ��������
	virtual NF_SHARE_PTR<NFIObject> CreatePub(const std::string& pub_name, bool bSave = false) = 0;
	// ��������
	virtual NF_SHARE_PTR<NFIObject> CreateGuild(const std::string& guild_name) = 0;
	// ɾ��������
	virtual bool DeletePub(const std::string& pub_name) = 0;
	// ɾ������
	virtual bool DeleteGuild(const std::string& guild_name) = 0;
	// ��ù���GUID
	virtual const NFGUID& GetPubGuid(const std::string& name) = 0;
	// ��ù���GUID
	virtual const NFGUID& GetGuildGuid(const std::string& name) = 0;
	// �ֶ�����������
	virtual bool SavePubData(const std::string& name, const NFGUID& object_id) = 0;
	// �ֶ����湫������
	virtual bool SaveGuildData(const NFGUID& object_id) = 0;
	// Command�¼������
	virtual bool CommandToPlayer(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg) = 0;
	// Command�¼���Game��������
	virtual bool CommandToGame(int nCommandId, const NFGUID& sendobj, const NFDataList& msg) = 0;
	// Command�¼����ڲ�����
	virtual bool Command(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg) = 0;
	// Command�¼�����������
	virtual bool CommandToHelper(int nCommandId, const NFGUID& sendobj, const NFDataList& msg) = 0;
};

#endif
