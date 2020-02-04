///---------------------------------------------------------
/// �ļ���: NFILetterModule.h
/// �� ��:  �ż��ӿ�
/// ˵ ��:       
/// ��������: 2019/08/30
/// ������: �ڵ���
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------

#ifndef NFI_LETTER_MODULE_H
#define NFI_LETTER_MODULE_H

#include <string>
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFList.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "KConstDefine.h"

class NFILetterModule : public  NFIModule
{
public:
	virtual ~NFILetterModule() {}
	/// \brief �����ż�
	/// \param sender ������(�����Ч)
	/// \param sendName ������
	/// \param receiver ������
	/// \param strTitle ����
	/// \param strContent ����
	/// \param params ���ݲ���
	/// \param strAppend ����
	virtual bool SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
		const std::string& strContent, const NFDataList& params, const std::string& strAppend) = 0;

	/// \brief �鿴�ż�����
	/// \param self ����
	virtual bool QueryLetter(const NFGUID& self) = 0;

	/// \brief �鿴�ż���Ϣ
	/// \param self ����
	/// \param num ����
	virtual bool LookLetter(const NFGUID& self, const int num) = 0;

	/// \brief ɾ���ż�
	/// \param self ����
	/// \param strLetterID �ż�id(Ϊ��ɾ��ȫ��)
	virtual bool DeleteLetter(const NFGUID& self, std::string strLetterID) = 0;
};

#endif //NFI_LETTER_MODULE_H
