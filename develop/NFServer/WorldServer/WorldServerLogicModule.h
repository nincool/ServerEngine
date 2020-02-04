///--------------------------------------------------------------------
/// �ļ���:		WorldServerLogicModule.h
/// ��  ��:		world��Ϊ�������߼�ģ��
/// ˵  ��:		
/// ��������:	2019��9��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef _WORLD_SERVER_LOGIC_MODULE_H__
#define _WORLD_SERVER_LOGIC_MODULE_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "WorldPlayers.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CWorldServerLogicModule : public NFIModule
{
public:
	CWorldServerLogicModule(NFIPluginManager* p);
	~CWorldServerLogicModule();

	virtual bool Init();
	virtual bool AfterInit();

	virtual const InsideServerData* GetSuitProxyForEnter();
private:
	//��Command��Ϣת������
	void OnCommandTransmit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	//����ͨ�Ų���
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NetClientModule* m_pNetClientModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CWorldPlayers* m_pWorldPlayers = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif 