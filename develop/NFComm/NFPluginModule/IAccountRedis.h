///--------------------------------------------------------------------
/// 文件名:		IAccountRedis.h
/// 内  容:		账号数据库
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
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