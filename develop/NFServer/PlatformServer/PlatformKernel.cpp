///--------------------------------------------------------------------
/// 文件名:		PlatformKernel.cpp
/// 内  容:		平台服核心模块
/// 说  明:		
/// 创建日期:	2019年10月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PlatformKernel.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"

CPlatformKernel::CPlatformKernel(NFIPluginManager* p)
{
	pPluginManager = p;
}

CPlatformKernel::~CPlatformKernel()
{
}

bool CPlatformKernel::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pPlatformServerState = pPluginManager->FindModule<CPlatformServerState>();
	m_pHeartBeatModule = pPluginManager->FindModule<NFIHeartBeatModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CPlatformKernel::Shut()
{
	return true;
}

bool CPlatformKernel::Execute()
{
	return true;
}

bool CPlatformKernel::AfterInit()
{
	m_pClassModule->AddEventCallBack(LC::Battle::ThisName(), EVENT_OnDestroy, METHOD_ARGS(CPlatformKernel::OnBattleDestroy));

	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_START_LOAD_PLATFORM, this, (NetMsgFun)&CPlatformKernel::OnAckStartLoadPlatform);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_CREATE_PLATFORM, this, (NetMsgFun)&CPlatformKernel::OnAckCreatePlatform);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_PLATFORM_DATA, this, (NetMsgFun)&CPlatformKernel::OnAckLoadPlatformData);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_PLATFORM_COMPLETE, this, (NetMsgFun)&CPlatformKernel::OnAckLoadPlatformComplete);
	return true;
}

void CPlatformKernel::OnAckStartLoadPlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PropertyList xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PropertyList ParseFromArray failed";
		return;
	}
	
	for (int i = 0; i < xMsg.property_string_size(); ++i)
	{
		const OuterMsg::PropertyString& xData = xMsg.property_string(i);
		NFGUID id;
		id.FromString(xData.data());

		OnMapAddIndex(mxMapPlatformGuid, id, xData.name());
	}
}

void CPlatformKernel::OnAckCreatePlatform(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckResult xAckResult;
	if (!xAckResult.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckResult ParseFromArray failed";
		return;
	}

	if (xAckResult.event_code() == OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		NFGUID id;
		PBConvert::ToNF(xAckResult.obj_id(), id);

		SavePlatformData(id);

		if (xAckResult.name() == LC::PlatformManager::ThisName())
		{
			// 进入配置加载
			m_pPlatformServerState->PlatformStateMachine(CPlatformServerState::PLATFORMINPUT_WAITINGOPEN, msg, nLen);
		}
	}
}

void CPlatformKernel::OnAckLoadPlatformData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xObjectDataPack;
	if (!xObjectDataPack.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	NFDataList var;
	NF_SHARE_PTR<NFObject> pObject =
		dynamic_pointer_cast<NFObject>(
			PBConvert::ToNFObject(
			(NFKernelModule*)m_pKernelModule,
				NFGUID(),
				&xObjectDataPack,
				var, false, false, true));

	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pObject == NULL";
		return;
	}

	if (pObject->ClassName() == LC::PlatformManager::ThisName())
	{
		m_pPlatformObject = pObject;
	}

	const NFGUID& self = pObject->Self();

	//增加定时Platform数据保存心跳
	m_pHeartBeatModule->AddHeartBeat(self, "SavePlatformDataOnTime", this, &CPlatformKernel::SavePlatformDataOnTime, 180000, -1);
}

// 加载是否完成
void CPlatformKernel::OnAckLoadPlatformComplete(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckEventResult xAckEventResult;
	if (!xAckEventResult.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckEventResult ParseFromArray failed";
		return;
	}

	// 加载数据完成
	if (xAckEventResult.event_code() == OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		// 进入配置加载
		m_pPlatformServerState->PlatformStateMachine(CPlatformServerState::PLATFORMINPUT_WAITINGOPEN, msg, nLen);
	}
	else if (xAckEventResult.event_code() == OuterMsg::EGameEventCode::EGEC_NOEXIST_PLATFORM_DB)
	{
		// 创建一个平台服管理
		m_pPlatformObject = m_pKernelModule->CreateObject(LC::PlatformManager::ThisName(), "", NFDataList());
		if (m_pPlatformObject == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " create object failed" << LC::PlatformManager::ThisName();
			return;
		}

		//通知数据库创建
		OuterMsg::PubOpData xMsg;
		xMsg.set_PubName(LC::PlatformManager::ThisName());
		PBConvert::ToPB(m_pPlatformObject->Self(), *xMsg.mutable_pub_id());

		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB,  OuterMsg::SS_REQ_CREATE_PLATFORM, xMsg);

		//增加定时BattleHelper数据保存心跳
		m_pHeartBeatModule->AddHeartBeat(m_pPlatformObject->Self(), "SavePlatformDataOnTime", this, &CPlatformKernel::SavePlatformDataOnTime, 180000, -1);
	}
}

int CPlatformKernel::SavePlatformDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// 数据存库事件
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not exist:" << self;
		return 0;
	}

	// 存库事件
	pObject->OnEvent(EVENT_OnStore, NFGUID(), NFDataList() << STORE_TIMING);

	SavePlatformData(self);

	return 0;
}

NFIKernelModule* CPlatformKernel::CKernel()
{
	return m_pKernelModule;
}

// 开启战场
NF_SHARE_PTR<NFIObject> CPlatformKernel::OnStartBattle(const std::string& sBattleName, const NFDataList& args)
{
	// 创建一个平台服辅助
	NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->CreateObject(sBattleName, "", NFDataList());
	if (pObject == nullptr)
	{
		QLOG_FATAL << __FUNCTION__ << "create object failed" << sBattleName;
		return nullptr;
	}

	//通知数据库创建
	OuterMsg::PubOpData xMsg;
	xMsg.set_PubName(sBattleName);
	PBConvert::ToPB(pObject->Self(), *xMsg.mutable_pub_id());

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_CREATE_PLATFORM, xMsg);

	//ROOM同步创建
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_ROOM, OuterMsg::SS_REQ_CREATE_PLATFORM, xMsg);

	//增加定时BattleHelper数据保存心跳
	m_pHeartBeatModule->AddHeartBeat(pObject->Self(), "SavePlatformDataOnTime", this, &CPlatformKernel::SavePlatformDataOnTime, 180000, -1);

	return pObject;
}

// 发送消息给游戏服GUID
bool CPlatformKernel::SendCommandToGameGuid(int nCommandId, const NFGUID& sendobj, 
	const NFGUID& targetobj, const NFDataList& args)
{
	// 获得玩家的worldid
	NF_SHARE_PTR<NFIObject> pObject = m_pKernelModule->GetObject(targetobj);
	if (pObject == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not find" << targetobj;
		return false;
	}

	int nServerID = pObject->GetPropertyInt("ServerID");

	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::ToPB(targetobj, *commandMsg.mutable_target());
	commandMsg.set_target_class_Name(pObject->ClassName());
	PBConvert::NFDataListToPBDataList(args, *commandMsg.mutable_data());

	std::string strCommandMsg;
	commandMsg.SerializeToString(&strCommandMsg);
	//m_pNetServerInsideModule->SendMsgToAppID(nServerID, OuterMsg::SS_NET_COMMAND_TRANSMIT, strCommandMsg);

	return true;
}

// 发送消息给Game
bool CPlatformKernel::SendCommandToGame(int nCommandId, int world_id, 
	const NFGUID& sendobj, const NFDataList& args)
{
	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::NFDataListToPBDataList(args, *commandMsg.mutable_data());

	std::string strCommandMsg;
	commandMsg.SerializeToString(&strCommandMsg);
	//m_pNetServerInsideModule->SendMsgToAppID(world_id, OuterMsg::SS_NET_COMMAND_TRANSMIT, strCommandMsg);

	return true;
}

// 发送消息给所有Game
bool CPlatformKernel::SendCommandToAllGame(int nCommandId, const NFGUID& sendobj, const NFDataList& args)
{
	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::NFDataListToPBDataList(args, *commandMsg.mutable_data());

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_WORLD, OuterMsg::SS_NET_COMMAND_TRANSMIT, commandMsg);

	return true;
}

// 发送消息给战场服
bool CPlatformKernel::SendCommandToBattle(int nCommandId, const NFGUID& sendobj, 
	const NFGUID& battle_guid, const NFDataList& args)
{
	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::ToPB(battle_guid, *commandMsg.mutable_target());
	commandMsg.set_target_class_Name(LC::Battle::ThisName());
	PBConvert::NFDataListToPBDataList(args, *commandMsg.mutable_data());

	std::string strCommandMsg;
	commandMsg.SerializeToString(&strCommandMsg);
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_ROOM, OuterMsg::SS_COMMAND_TOROOM, strCommandMsg);

	return true;
}

// 手动保存平台数据
bool CPlatformKernel::SavePlatformData(const NFGUID& object_id)
{
	OuterMsg::ObjectDataPack xRoleDataPack;
	PBConvert::ToPBObject(
		m_pKernelModule,
		object_id,
		&xRoleDataPack,
		[](NFIProperty& prop)
		{
			return !prop.IsNull() && prop.GetSave();
		},
		[](NFIRecord& record)
		{
			return !record.IsEmpty() && record.GetSave();
		});

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_SAVE_PLATFORM_DATA, xRoleDataPack);

	return true;
}

// 删除平台数据
bool CPlatformKernel::DeletePlatformData(const NFGUID& object_id)
{
	if (m_pPlatformObject != nullptr && object_id == m_pPlatformObject->Self())
	{
		QLOG_WARING << " m_pPlatformObject == NULL";
		return false;
	}

	m_pKernelModule->DestroyObject(object_id);
	return true;
}

int CPlatformKernel::OnBattleDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not find" << self;
		return 0;
	}

	map<NFGUID, std::string>::iterator iterFind = mxMapPlatformGuid.find(pObj->Self());
	if (iterFind != mxMapPlatformGuid.end())
	{
		mxMapPlatformGuid.erase(iterFind);
	}

	// 通知给ROOM
	OuterMsg::PubOpData xPubOpData;
	PBConvert::ToPB(self, *xPubOpData.mutable_pub_id());
	xPubOpData.set_PubName(pObj->GetName());
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_ROOM, OuterMsg::SS_REQ_DELETE_PLATFORM, xPubOpData);

	// 通知给DB
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_DELETE_PLATFORM, xPubOpData);

	return 0;
}

// Map添加数据
bool CPlatformKernel::OnMapAddIndex(map<NFGUID, std::string>& mapInfo, const NFGUID& key, const std::string& id)
{
	if (key.IsNull())
	{
		return false;
	}

	map<NFGUID, std::string>::iterator iterFind = mapInfo.find(key);
	if (iterFind != mapInfo.end())
	{
		return false;
	}

	mapInfo.insert(make_pair(key, id));
	return true;
}

bool CPlatformKernel::SaveAllPlatformData()
{
	return true;
}