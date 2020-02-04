///--------------------------------------------------------------------
/// 文件名:		RoomKernel.cpp
/// 内  容:		房间服核心模块
/// 说  明:		
/// 创建日期:	2019年10月9日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "RoomKernel.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"

CRoomKernel::CRoomKernel(NFIPluginManager* p)
{
	pPluginManager = p;
}

CRoomKernel::~CRoomKernel()
{
}

bool CRoomKernel::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();

	return true;
}

bool CRoomKernel::Shut()
{
	return true;
}

bool CRoomKernel::Execute()
{
	return true;
}

bool CRoomKernel::AfterInit()
{

	return true;
}

NFIKernelModule* CRoomKernel::CKernel()
{
	return m_pKernelModule;
}

// 发送消息给平台服
bool CRoomKernel::SendCommandToPlatform(int nCommandId, const NFGUID& sendobj, const NFDataList& args)
{
	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::NFDataListToPBDataList(args, *commandMsg.mutable_data());

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_PLATFORM, OuterMsg::SS_COMMAND_TOPLATFORM, commandMsg);

	return true;
}

// 发送消息给战场服GUID
bool CRoomKernel::SendCommandToPlatformBattle(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& args)
{
	if (sendobj.IsNull() || targetobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sendobj/targetobj is null";
		return false;
	}
	
	//获得对象
	NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->GetObject(sendobj);
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find:" << sendobj;
		return false;
	}

	std::string strClassName = pObject->GetName();

	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::ToPB(targetobj, *commandMsg.mutable_target());
	commandMsg.set_target_class_Name(strClassName);
	PBConvert::NFDataListToPBDataList(args, *commandMsg.mutable_data());

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_PLATFORM, OuterMsg::SS_COMMAND_TOPLATFORM, commandMsg);

	return true;
}