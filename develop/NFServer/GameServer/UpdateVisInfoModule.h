///--------------------------------------------------------------------
/// �ļ���:		UpdateVisInfoModule.h
/// ��  ��:		���Ժͱ�ͬ���ͻ���
/// ˵  ��:		
/// ��������:	2019��8��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_UPDATE_VISINFO_MODULE_H__
#define __NF_UPDATE_VISINFO_MODULE_H__

#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "GameServer.h"

class CUpdateVisInfoModule : public NFIModule
{
public:
	CUpdateVisInfoModule(NFIPluginManager* p);
    ~CUpdateVisInfoModule();

    virtual bool Init();
    virtual bool AfterInit();

	// ͬ�������������
	void UpdateAllPlayerData(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
private:
	// �����¼����
	int OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �������Ը���
	int OnVisPropUpdate(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData);
	// ���ӱ����
	int OnVisRecordUpdate(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldData, const NFData& newData);
	// ͬ��������п�������
	int VisAllPropUpdate(const NFGUID& self);
	// ͬ��������п��ӱ�
	int VisAllRecordUpdate(const NFGUID& self);

	// �������
	bool PackAllProperty(NFObject* pObj, OuterMsg::PropertyList* pPrivateList);
	// ������б�
	bool PackAllRecord(NFObject* pObj, OuterMsg::RecordList* pRecList);
	// �����
	bool PackRecord(NF_SHARE_PTR<NFIRecord> pRecord, OuterMsg::Record* pPrivateData);
private:
	NFIKernelModule* m_pKernelModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
};

#endif // __NF_UPDATE_VISINFO_MODULE_H__
