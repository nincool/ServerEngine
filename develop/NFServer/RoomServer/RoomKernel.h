///--------------------------------------------------------------------
/// �ļ���:		RoomKernel.h
/// ��  ��:		���������ģ��
/// ˵  ��:		
/// ��������:	2019��10��9��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __ROOM_KERNEL_H
#define __ROOM_KERNEL_H
#include "NFComm/NFPluginModule/IRoomKernel.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class CRoomKernel : public IRoomKernel
{
public:
	CRoomKernel(NFIPluginManager* p);
    virtual ~CRoomKernel();

    virtual bool Init();
    virtual bool Shut();
    virtual bool AfterInit();
    virtual bool Execute();
	
	// ��ȡ���ú���
	virtual NFIKernelModule* CKernel();
	
	// ������Ϣ��ƽ̨��
	virtual bool SendCommandToPlatform(int nCommandId, const NFGUID& sendobj, const NFDataList& args);

	// ������Ϣ��ƽ̨����������
	virtual bool SendCommandToPlatformBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args);

private:
	//ͨ���ں�ģ��
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
};

#endif
