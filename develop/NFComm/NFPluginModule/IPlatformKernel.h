///--------------------------------------------------------------------
/// �ļ���:		IPlatformKernel.h
/// ��  ��:		ƽ̨������ģ��ӿ�
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef I_PLATFORM_KERNEL_H
#define I_PLATFORM_KERNEL_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFIKernelModule.h"

#define IPLATFORM_LOGIC_VERSION 0x0001

class IPlatformKernel : public NFIModule
{
public:
	virtual ~IPlatformKernel() {};

	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel() = 0;

	// �������������
	virtual NF_SHARE_PTR<NFIObject> GetPlatformHelper() = 0;

	// ����ƽ̨������
	virtual bool SavePlatformData(const NFGUID& object_id) = 0;

	// ɾ��ƽ̨������
	virtual bool DeletePlatformData(const NFGUID& object_id) = 0;

	// ����ս��
	virtual NF_SHARE_PTR<NFIObject> OnStartBattle(const std::string& sBattleName, const NFDataList& args) = 0;

	// ������Ϣ����Ϸ��GUID
	virtual bool SendCommandToGameGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args) = 0;

	// ������Ϣ��Game
	virtual bool SendCommandToGame(int nCommandId, int world_id, const NFGUID& sendobj, const NFDataList& args) = 0;

	// ������Ϣ������Game
	virtual bool SendCommandToAllGame(int nCommandId, const NFGUID& sendobj, const NFDataList& args) = 0;

	// ������Ϣ��ս����
	virtual bool SendCommandToBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& battle_guid, const NFDataList& args) = 0;
};

#endif
