///---------------------------------------------------------
/// �ļ���: NFLetterModule.h
/// �� ��:  
/// ˵ ��:       
/// ��������: 2019/08/26
/// ������: �ڵ���
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------
#ifndef NF_LETTER_MODULE_H__
#define NF_LETTER_MODULE_H__

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFILetterModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class NFLetterModule : public NFILetterModule
{
public:
	NFLetterModule(NFIPluginManager* p)
	{
		pPluginManager = p;
	}

	virtual bool Init();

	virtual bool AfterInit();

	/// \brief �鿴�ż�����
	void OnAckQueryLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	/// \brief �鿴�ż��ظ�
	void OnAckLookLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	/// \brief �����ż�
	bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend);

	/// \brief �鿴�ż�����
	bool QueryLetter(const NFGUID& self);

	/// \brief �鿴�ż�(num=0ȫ��ȡ����)
	bool LookLetter(const NFGUID& self, const int num);

	/// \brief ɾ���ż�
	bool DeleteLetter(const NFGUID& self, std::string strLetterID);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetClientModule* m_pNetClientModule = nullptr;
	NFIClassModule* m_pClassModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;

};

#endif // NF_LETTER_MODULE_H__
