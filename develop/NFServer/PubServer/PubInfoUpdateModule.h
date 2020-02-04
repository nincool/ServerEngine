///--------------------------------------------------------------------
/// �ļ���:		PubInfoUpdateModule.h
/// ��  ��:		�������Ժͱ�ͬ����game
/// ˵  ��:		
/// ��������:	2019��8��8��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _PUBINFO_UPDATE_MODULE_H__
#define _PUBINFO_UPDATE_MODULE_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPubInfoUpdateModule : public NFIModule
{
public:
	CPubInfoUpdateModule(NFIPluginManager* p);
    ~CPubInfoUpdateModule();

    virtual bool Init();
    virtual bool AfterInit();

private:
	// �����¼����
	int OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// �������Ը���
	int OnVisPropUpdate(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData);
	// ���ӱ����
	int OnVisRecordUpdate(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldData, const NFData& newData);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif // _PUBINFO_UPDATE_MODULE_H__
