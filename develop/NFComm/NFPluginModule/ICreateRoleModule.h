///--------------------------------------------------------------------
/// 文件名:		ICreateRoleModule.h
/// 内  容:		创角接口
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef I_CREATE_ROLE_MODULE_H
#define I_CREATE_ROLE_MODULE_H

#include <iostream>
#include "NFIModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "../NFMessageDefine/OuterMsg.pb.h"

class ICreateRoleModule : public NFIModule
{
public:
	// 编辑玩家数据
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp) = 0;
};

#endif