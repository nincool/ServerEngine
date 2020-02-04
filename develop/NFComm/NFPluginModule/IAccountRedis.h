///--------------------------------------------------------------------
/// �ļ���:		IAccountRedis.h
/// ��  ��:		�˺����ݿ�
/// ˵  ��:		
/// ��������:	2019��8��3��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _I_ACCOUNT_REDIS_H
#define _I_ACCOUNT_REDIS_H

#include "NFIModule.h"

class IAccountRedis : public NFIModule
{
public:
	virtual bool VerifyAccount(const std::string& strAccount, const std::string& strPwd) = 0;
};

#endif