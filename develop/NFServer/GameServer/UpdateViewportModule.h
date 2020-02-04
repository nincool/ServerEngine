///--------------------------------------------------------------------
/// �ļ���:		UpdateViewportModule.h
/// ��  ��:		�Ӵ�ͬ������ģ��
/// ˵  ��:		
/// ��������:	2019��8��3��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_UPDATE_VIEWPORT_MODULE_H__
#define __NF_UPDATE_VIEWPORT_MODULE_H__

#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFObject.h"
#include "GameServer.h"

class CUpdateViewportModule : public NFIModule
{
public:
	CUpdateViewportModule(NFIPluginManager* p);
    ~CUpdateViewportModule();

    virtual bool Init();
    virtual bool AfterInit();
public:

	// �Ӵ�����
	int OnViewInit(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �Ӵ���Ӷ���
	int OnViewAdd(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �Ӵ��Ƴ�����
	int OnViewRemove(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �Ӵ�����ı�λ��
	int OnViewChange(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �Ӵ��ر�
	int OnViewClose(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	
private:
	// �������
	bool PackAllProperty(NFObject* pObj, OuterMsg::PropertyList* pPrivateList);
	// ������б�
	bool PackAllRecord(NFObject* pObj, OuterMsg::RecordList* pRecList);
	// �����
	bool PackRecord(NF_SHARE_PTR<NFIRecord> pRecord, OuterMsg::Record* pPrivateData);
	
	// ���󴴽��¼�
	int OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �Ӵ����Ա仯
	int OnViewPropChanged(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData);
	// �Ӵ���仯
	int OnViewRecordChanged(const NFGUID& self, const RECORD_EVENT_DATA& event, const NFData& oldData, const NFData& newData);

	// ���һ������
	bool PackOneProperty(OuterMsg::PropertyList* pList, const std::string& strName, const NFData& data);
	// ���һ��������
	bool PackRecordOneData(OuterMsg::RecordList* pList, const RECORD_EVENT_DATA& event, const NFIRecord* pRecordData, const NFData& data);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
};

#endif // __NF_UPDATE_VIEWPORT_MODULE_H__
