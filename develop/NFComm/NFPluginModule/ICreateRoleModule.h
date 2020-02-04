///--------------------------------------------------------------------
/// �ļ���:		ICreateRoleModule.h
/// ��  ��:		���ǽӿ�
/// ˵  ��:		
/// ��������:	2019��8��3��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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
	// �༭�������
	virtual bool AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp) = 0;
};

#endif