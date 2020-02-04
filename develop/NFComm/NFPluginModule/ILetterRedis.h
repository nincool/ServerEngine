///---------------------------------------------------------
/// �ļ���: ILetterRedis.h
/// �� ��:  
/// ˵ ��:       
/// ��������: 2019/08/27
/// ������: �ڵ���
/// �޸���:
/// ��Ȩ����: Ѫ��������
///---------------------------------------------------------
#ifndef I_LETTER_REDIS_MODULE_H__
#define I_LETTER_REDIS_MODULE_H__

#include "NFIModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"

class ILetterRedis : public NFIModule
{
public:
	virtual bool SendLetter(const NFGUID& self, const OuterMsg::LetterDataPack& xLetterDataPack, const int nExpireSecond = 0) = 0;
	virtual bool LookLetter(const NFGUID& self, const int num, OuterMsg::LetterDataPack& xLetterDataPack, bool bDelete = true) = 0;
	virtual bool DeleteLetter(const NFGUID& self, const std::string& strLetterID) = 0;
	virtual bool QueryLetter(const NFGUID& self, OuterMsg::AckQueryLetter& xQueryLetter) = 0;
};

#endif // I_LETTER_REDIS_MODULE_H__
