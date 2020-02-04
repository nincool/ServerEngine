///--------------------------------------------------------------------
/// �ļ���:		IRoomKernel.h
/// ��  ��:		���������ģ��ӿ�
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef I_ROOM_KERNEL_H
#define I_ROOM_KERNEL_H

#include "NFIModule.h"
#include "NFComm/NFCore/NFIObject.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFIKernelModule.h"

#define IROOM_LOGIC_VERSION 0x0001

class IRoomKernel : public NFIModule
{
public:
	virtual ~IRoomKernel() {};

	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel() = 0;

	// ������Ϣ��ƽ̨��
	virtual bool SendCommandToPlatform(int nCommandId, const NFGUID& sendobj, const NFDataList& args) = 0;

	// ������Ϣ��ƽ̨����������
	virtual bool SendCommandToPlatformBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args) = 0;
};

#endif
