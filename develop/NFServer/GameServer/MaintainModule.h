///--------------------------------------------------------------------
/// �ļ���:		MaintainModule.h
/// ��  ��:		ά��ģ��
/// ˵  ��:		
/// ��������:	2019��10��14��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_MaintainModule_H__
#define __H_MaintainModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFKernelPlugin/NFKernelModule.h"
#include "NFComm/NFPluginModule/NFIHttpServerModule.h"
#include "NFComm/NFPluginModule/NFIHeartBeatModule.h"
#include "MaintainMsgDataDefine.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"
#include "GameKernel.h"

class MaintainModule : public NFIModule
{
public:
	MaintainModule(NFIPluginManager* p);
	~MaintainModule();

	virtual bool Init();
	virtual bool AfterInit();
private:
	//ȡ�ö����б�
	bool GetObjList(const REQGetObjList& req, ACKGetOBJList& ack);
	//ȡ�ö��������б�
	bool GetObjPropList(const REQSelectObj& req, ACKGetObjPropList& ack);
	//ȡ�ö������б�
	bool GetObjRecList(const REQSelectObj& req, ACKGetObjRecList& ack);
	//ȡ�ö��������б�
	bool GetObjConList(const REQSelectObj& req, ACKGetOBJList& ack);
	//ȡ�ö���������
	bool GetObjRec(const J_SelectObjRec& req, J_ObjRecData& ack);
	//ȡ�ö���ʱ���б�
	bool GetObjBeatList(const REQSelectObj& req, ACKGetObjBeatList& ack);
	//ȡ�ö���ʱ������
	bool GetObjBeat(const J_SelectObjBeat& req, J_ObjBeatData& ack);
	//ȡ�ö������Իص��б�
	bool GetObjPropCriticalList(const REQSelectObj& req, ACKGetObjPropCriticalList& ack);
	//ȡ�ö�����ص��б�
	bool GetObjRecCallBackList(const REQSelectObj& req, ACKGetObjRecCallBackList& ack);

	//�޸Ķ�������
	bool SetObjProp(const J_ModifyObjProp& req);
	//�޸Ķ����
	bool SetObjRec(J_ModifyObjRec& req);
private:
	//ȡ�ö����б�
	void OnGetObjList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö��������б�
	void OnGetObjPropList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö������б�
	void OnGetObjRecList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö��������б�
	void OnGetObjConList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö���������
	void OnGetObjRec(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö���ʱ���б�
	void OnGetObjBeatList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö���ʱ������
	void OnGetObjBeat(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö������Իص��б�
	void OnGetObjPropCriticalList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�ö�����ص��б�
	void OnGetObjRecCallbackList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//�޸Ķ�������
	void OnSetObjProp(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//�޸Ķ����
	void OnSetObjRec(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//�޸Ķ�����
	void OnSetObjCon(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);
	//ȡ�����GUID Name�б�
	void OnGetPlayerGNList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);

	std::string ExportOBJList(std::vector<std::string>& arg);
	std::string ExportOBJ(std::vector<std::string>& arg);

private:
	NetServerMaintainModule* m_pMaintainNetServer = nullptr;
	NFKernelModule* m_pKernelModule = nullptr;
	NFIHeartBeatModule* m_pHeartBeatModule = nullptr;
	CGameKernel* m_pGameKernel = nullptr;
};


#endif //__H_MaintainModule_H__