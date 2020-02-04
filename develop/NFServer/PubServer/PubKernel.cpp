///--------------------------------------------------------------------
/// �ļ���:		PubKernelModule.cpp
/// ��  ��:		���������ģ��
/// ˵  ��:		
/// ��������:	2019��8��26��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "PubKernel.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"

const int SAVING_SEC_MAX = 300;

CPubKernel::CPubKernel(NFIPluginManager* p)
{
	pPluginManager = p;
}

CPubKernel::~CPubKernel()
{
}

bool CPubKernel::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pHeartBeatModule = pPluginManager->FindModule<NFIHeartBeatModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pPubServerState = pPluginManager->FindModule<CPubServerState>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

bool CPubKernel::Shut()
{
	return true;
}

bool CPubKernel::Execute()
{
	if (m_mapPubInfo.size() > 0 && nLastTime != pPluginManager->GetNowTime())
	{
		nLastTime = pPluginManager->GetNowTime();

		for (auto &it : m_mapPubInfo)
		{
			if (++it.second.nSavingSec >= SAVING_SEC_MAX)
			{
				QLOG_ERROR << __FUNCTION__ << " save object timeout:" << it.first;

				RemoveObject(it.first);
			}
		}

		for (auto &it : m_listRemovePub)
		{
			m_mapPubInfo.erase(it);
		}
		m_listRemovePub.clear();
	}

	return true;
}

bool CPubKernel::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_SYN_INFO, this, (NetMsgFun)&CPubKernel::OnReqSynServerInfo);
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_COMMAND_TOPUB, this, (NetMsgFun)&CPubKernel::OnCommandTransmit);

	m_pClassModule->AddEventCallBack(LC::Guild::ThisName(), EVENT_OnRecover, METHOD_ARGS(CPubKernel::OnGuildRecover));
	m_pClassModule->AddEventCallBack(LC::Pub::ThisName(), EVENT_OnRecover, METHOD_ARGS(CPubKernel::OnPubRecover));

	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_START_LOAD_GUILD, this, (NetMsgFun)&CPubKernel::OnAckStartLoadGuild);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_START_LOAD_PUB, this, (NetMsgFun)&CPubKernel::OnAckStartLoadPub);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_CREATE_PUB, this, (NetMsgFun)&CPubKernel::OnAckCreatePub);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_CREATE_GUILD, this, (NetMsgFun)&CPubKernel::OnAckCreateGuild);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_GUILD_DATA, this, (NetMsgFun)&CPubKernel::OnAckLoadGuildData);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_PUB_DATA, this, (NetMsgFun)&CPubKernel::OnAckLoadPubData);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_OBJECT_SAVED, this, (NetMsgFun)&CPubKernel::OnAckSaveObjectData);
	return true;
}

NFIKernelModule* CPubKernel::CKernel()
{
	return m_pKernelModule;
}

void CPubKernel::OnReqSynServerInfo(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	// �Ѿ�����������
	if (m_pPubServerState->GetPubState() == CPubServerState::PUBSTATE_OPENED)
	{
		// ͬ������Guid����
		SynAllGuidInfo(mxMapPubGuid, OuterMsg::SS_ACK_START_LOAD_PUB);
		SynAllGuidInfo(mxMapGuildGuid, OuterMsg::SS_ACK_START_LOAD_GUILD);

		// ͬ������data����
		SynAllMapData(mxMapPubGuid, OuterMsg::SS_ACK_LOAD_PUB_DATA);
		SynAllMapData(mxMapGuildGuid, OuterMsg::SS_ACK_LOAD_GUILD_DATA);

		// ȷ�ϼ������
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_ACK_LOAD_PUB_COMPLETE, std::string(msg, nLen));
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_ACK_LOAD_GUILD_COMPLETE, std::string(msg, nLen));
	}
}

void CPubKernel::OnAckStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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
		OnMapAddIndex(mxMapPubGuid, xData.name(), id);
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, nMsgID, xMsg);
}

void CPubKernel::OnAckStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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
		OnMapAddIndex(mxMapGuildGuid, xData.name(), id);
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, nMsgID, xMsg);
}

void CPubKernel::OnAckCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckResult xAckResult;
	if (!xAckResult.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckResult ParseFromArray failed";
		return;
	}

	if (xAckResult.event_code() == OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		const std::string& pub_name = xAckResult.name();
		NFGUID id;
		PBConvert::ToNF(xAckResult.obj_id(), id);

		SavePubData(pub_name, id);
	}
}

void CPubKernel::OnAckCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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

		SaveGuildData(id);
	}
}

void CPubKernel::OnAckLoadPubData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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
		QLOG_ERROR << __FUNCTION__ << " pObject == NULL";
		return;
	}

	const NFGUID& self = pObject->Self();

	//���Ӷ�ʱPub���ݱ�������
	m_pHeartBeatModule->AddHeartBeat(self, "SavePubDataOnTime", this, &CPubKernel::SavePubDataOnTime, 300000, -1);
}

void CPubKernel::OnAckLoadGuildData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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
		QLOG_ERROR << __FUNCTION__ << " pObject == NULL";
		return;
	}

	const NFGUID& self = pObject->Self();

	//���Ӷ�ʱGuild���ݱ�������
	m_pHeartBeatModule->AddHeartBeat(self, "SaveGuildDataOnTime", this, &CPubKernel::SaveGuildDataOnTime, 300000, -1);
}

int CPubKernel::SaveGuildDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// ���ݴ���¼�
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " obj not exist:" << self;
		return 0;
	}

	// ����¼�
	pObject->OnEvent(EVENT_OnStore, NFGUID(), NFDataList() << STORE_TIMING);

	SaveGuildData(self);
	return 0;
}

int CPubKernel::SavePubDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// ���ݴ���¼�
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " obj not exist:" << self;
		return 0;
	}

	// ����¼�
	pObject->OnEvent(EVENT_OnStore, NFGUID(), NFDataList() << STORE_TIMING);

	SavePubData(pObject->ClassName(), self);
	return 0;
}

// guild������� ͬ����GAME
int CPubKernel::OnGuildRecover(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " obj not exist:" << self;
		return 0;
	}

	OnMapAddIndex(mxMapGuildGuid, pObj->GetName(), pObj->Self());

	SynDataToGame(pObj->Self(), OuterMsg::SS_ACK_LOAD_GUILD_DATA);

	return 0;
}

// pub������� ͬ����GAME
int CPubKernel::OnPubRecover(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(self);
	if (pObj == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " obj not exist:" << self;
		return 0;
	}

	OnMapAddIndex(mxMapPubGuid, pObj->ClassName(), pObj->Self());

	SynDataToGame(pObj->Self(), OuterMsg::SS_ACK_LOAD_PUB_DATA);

	return 0;
}

// ��������
NF_SHARE_PTR<NFIObject> CPubKernel::CreatePub(const std::string& pub_name, bool bSave)
{
	map<std::string, NFGUID>::iterator iterFind = mxMapPubGuid.find(pub_name);
	if (iterFind != mxMapPubGuid.end())
	{
		return nullptr;
	}

	auto pf = [&](NF_SHARE_PTR<NFIObject> pObj)
	{
		if (pObj != nullptr)
		{
			OnMapAddIndex(mxMapPubGuid, pub_name, pObj->Self());

			//֪ͨ��Game 
			OuterMsg::AckResult xAckResult;
			xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_SUCCESS);
			PBConvert::ToPB(pObj->Self(), *xAckResult.mutable_obj_id());
			xAckResult.set_name(pub_name);

			dynamic_cast<NFObject*>(pObj.get())->SetName(pub_name);

			//֪ͨ��Game
			m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_ACK_CREATE_PUB, xAckResult);

			SynDataToGame(pObj->Self(), OuterMsg::SS_ACK_LOAD_PUB_DATA);
		}
	};

	NF_SHARE_PTR<NFIObject> pObject = dynamic_cast<NFKernelModule*>(m_pKernelModule)->CreateObject(
		m_pKernelModule->CreateGUID(), pub_name, "", NFDataList(), pf);
	if (pObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " create object failed";
		return nullptr;
	}
	
	if (bSave)
	{
		// �����ݿⴴ������
		OuterMsg::PubOpData xMsg;
		xMsg.set_PubName(pub_name);
		PBConvert::ToPB(pObject->Self(), *xMsg.mutable_pub_id());

		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_CREATE_PUB, xMsg);

		//���Ӷ�ʱPub���ݱ�������
		m_pHeartBeatModule->AddHeartBeat(pObject->Self(), "SavePubDataOnTime", this, &CPubKernel::SavePubDataOnTime, 300000, -1);
	}

	return pObject;
}

// ��������
NF_SHARE_PTR<NFIObject> CPubKernel::CreateGuild(const std::string& guild_name)
{
	map<std::string, NFGUID>::iterator iterFind = mxMapGuildGuid.find(guild_name);
	if (iterFind != mxMapGuildGuid.end())
	{
		return nullptr;
	}

	auto pf = [&](NF_SHARE_PTR<NFIObject> pObj)
	{
		if (pObj != nullptr)
		{
			OnMapAddIndex(mxMapGuildGuid, guild_name, pObj->Self());

			//֪ͨ��Game 
			OuterMsg::AckResult xAckResult;
			xAckResult.set_event_code(OuterMsg::EGameEventCode::EGEC_SUCCESS);
			PBConvert::ToPB(pObj->Self(), *xAckResult.mutable_obj_id());
			xAckResult.set_name(guild_name);

			dynamic_cast<NFObject*>(pObj.get())->SetName(guild_name);

			//֪ͨ��Game
			m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_ACK_CREATE_GUILD, xAckResult);

			SynDataToGame(pObj->Self(), OuterMsg::SS_ACK_LOAD_GUILD_DATA);
		}
	};

	NF_SHARE_PTR<NFIObject> pObject = dynamic_cast<NFKernelModule*>(m_pKernelModule)->CreateObject(
		m_pKernelModule->CreateGUID(), LC::Guild::ThisName(), "", NFDataList(), pf);
	if (pObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " create object failed";
		return nullptr;
	}

	// �����ݿⴴ������
	OuterMsg::PubOpData xMsg;
	xMsg.set_PubName(guild_name);
	PBConvert::ToPB(pObject->Self(), *xMsg.mutable_pub_id());

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_CREATE_GUILD, xMsg);

	//���Ӷ�ʱGuild���ݱ�������
	m_pHeartBeatModule->AddHeartBeat(pObject->Self(), "SaveGuildDataOnTime", this, &CPubKernel::SaveGuildDataOnTime, 300000.0f, -1);

	return pObject;
}

// ��ù�����ӦGUID
const NFGUID& CPubKernel::GetPubGuid(const std::string& name)
{
	map<std::string, NFGUID>::iterator iterFind = mxMapPubGuid.find(name);
	if (iterFind == mxMapPubGuid.end())
	{
		QLOG_ERROR << __FUNCTION__ << " pub not exist:" << name;
		return NULL_OBJECT;
	}

	return iterFind->second;
}

// ��ù����ӦGUID
const NFGUID& CPubKernel::GetGuildGuid(const std::string& name)
{
	map<std::string, NFGUID>::iterator iterFind = mxMapGuildGuid.find(name);
	if (iterFind == mxMapGuildGuid.end())
	{
		QLOG_ERROR << __FUNCTION__ << " guild not exist:" << name;
		return NULL_OBJECT;
	}

	return iterFind->second;
}

// �ֶ�����������
bool CPubKernel::SavePubData(const std::string& name, const NFGUID& object_id)
{
	InnerSavePubData(name, object_id);

	return true;
}

// �ֶ����湫������
bool CPubKernel::SaveGuildData(const NFGUID& object_id)
{
	InnerSaveGuildData(object_id);

	return true;
}

// command�¼������
bool CPubKernel::CommandToPlayer(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg)
{
	if (sendobj.IsNull() || targetobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sendobj/targetobj is null";
		return false;
	}

	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::ToPB(targetobj, *commandMsg.mutable_target());
	commandMsg.set_target_class_Name(LC::Player::ThisName());
	PBConvert::NFDataListToPBDataList(msg, *commandMsg.mutable_data());

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_COMMAND_TOGAME, commandMsg);

	return true;
}

// command���������
bool CPubKernel::CommandToGame(int nCommandId, const NFGUID& sendobj, const NFDataList& msg)
{
	if (sendobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sendobj is null";
		return false;
	}

	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandId);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::NFDataListToPBDataList(msg, *commandMsg.mutable_data());

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_COMMAND_TOGAME, commandMsg);

	return true;
}

// Command�¼����ڲ�����
bool CPubKernel::Command(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg)
{
	if (sendobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sendobj is null";
		return false;
	}

	if (targetobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " targetobj is null";
		return false;
	}

	NF_SHARE_PTR<NFObject> pTarget = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(targetobj));
	if (pTarget == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " target not exist target:" << targetobj;
		return false;
	}

	pTarget->OnCommand(nCommandId, sendobj, msg);

	return true;
}

// Command�¼�����������
bool CPubKernel::CommandToHelper(int nCommandId, const NFGUID& sendobj, const NFDataList& msg)
{
	if (sendobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sendobj is null";
		return false;
	}

	m_pClassModule->RunCommandCallBack(pPluginManager->GetAppName(), nCommandId, NULL_OBJECT, sendobj, msg);

	return true;
}

// Map�������
bool CPubKernel::OnMapAddIndex(map<std::string, NFGUID>& mapInfo, const std::string& key, const NFGUID& id)
{
	if (key.empty())
	{
		return true;
	}

	map<std::string, NFGUID>::iterator iterFind = mapInfo.find(key);
	if (iterFind != mapInfo.end())
	{
		return false;
	}

	mapInfo.insert(make_pair(key, id));
	return true;
}

void CPubKernel::SynDataToGame(const NFGUID& id, const int nMsgID)
{
	// ֪ͨ������Ϣ
	OuterMsg::ObjectDataPack xRoleDataPack;
	PBConvert::ToPBObject(
		m_pKernelModule,
		id,
		&xRoleDataPack,
		[](NFIProperty& prop)
		{
			return prop.GetPrivate();
		},
		[](NFIRecord& record)
		{
			return record.GetPrivate();
		});

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, nMsgID, xRoleDataPack);
}

//���յ�Game��Command
void CPubKernel::OnCommandTransmit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::CommandMsg commandMsg;
	if (!commandMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " CommandMsg ParseFromArray failed";
		return;
	}

	NFGUID self;
	PBConvert::ToNF(commandMsg.self(), self);
	NFGUID target;
	PBConvert::ToNF(commandMsg.target(), target);
	NFDataList dataList;
	PBConvert::PBDataListToNFDataList(commandMsg.data(), dataList);

	if (!target.IsNull())
	{
		//��ָ��Ŀ��
		m_pClassModule->RunCommandCallBack(commandMsg.target_class_Name(), commandMsg.Id(), target, self, dataList);
	}
	else
	{
		m_pClassModule->RunCommandCallBack(pPluginManager->GetAppName(), commandMsg.Id(), target, self, dataList);
	}
}

void CPubKernel::SynAllMapData(map<std::string, NFGUID>& mapValue, const int nMsgID)
{
	for (auto &it : mapValue)
	{
		SynDataToGame(it.second, nMsgID);
	}
}

void CPubKernel::SynAllGuidInfo(map<std::string, NFGUID>& mapInfo, const int nMsgID)
{
	if (mapInfo.empty())
	{
		return;
	}

	OuterMsg::PropertyList xMsg;

	for (auto &it : mapInfo)
	{
		OuterMsg::PropertyString* pPropertyData = xMsg.add_property_string();
		if (pPropertyData == nullptr)
		{
			return;
		}

		pPropertyData->set_name(it.first);
		pPropertyData->set_data(it.second.ToString());
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, nMsgID, xMsg);
}

bool CPubKernel::SaveAllPubData()
{
	PubData pubData;
	pubData.nSavingSec = 0;
	pubData.nState = PUBSTATE_SAVING;
	for (auto &it : mxMapPubGuid)
	{
		m_mapPubInfo.insert(make_pair(it.second, pubData));
		InnerSavePubData(it.first, it.second, true);
	}

	for (auto &it : mxMapGuildGuid)
	{
		m_mapPubInfo.insert(make_pair(it.second, pubData));
		InnerSaveGuildData(it.second, true);
	}

	return true;
}

// ɾ��������
bool CPubKernel::DeletePub(const std::string& pub_name)
{
	// ����������
	NFGUID id = GetPubGuid(pub_name);
	if (id != NULL_OBJECT)
	{
		map<std::string, NFGUID>::iterator iterFind = mxMapPubGuid.find(pub_name);
		if (iterFind != mxMapPubGuid.end())
		{
			mxMapPubGuid.erase(iterFind);
		}

		OuterMsg::PubOpData xPubOpData;
		PBConvert::ToPB(id, *xPubOpData.mutable_pub_id());
		xPubOpData.set_PubName(pub_name);
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_REQ_DELETE_PUB, xPubOpData);

		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_DELETE_PUB, xPubOpData);

		return m_pKernelModule->DestroyObject(id);
	}

	return false;
}

// ɾ������
bool CPubKernel::DeleteGuild(const std::string& guild_name)
{
	// ����������
	NFGUID id = GetGuildGuid(guild_name);
	if (id != NULL_OBJECT)
	{
		map<std::string, NFGUID>::iterator iterFind = mxMapGuildGuid.find(guild_name);
		if (iterFind != mxMapGuildGuid.end())
		{
			mxMapGuildGuid.erase(iterFind);
		}

		OuterMsg::PubOpData xPubOpData;
		PBConvert::ToPB(id, *xPubOpData.mutable_pub_id());
		xPubOpData.set_PubName(guild_name);
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_GAME, OuterMsg::SS_REQ_DELETE_GUILD, xPubOpData);

		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_DELETE_GUILD, xPubOpData);

		return m_pKernelModule->DestroyObject(id);
	}

	return false;
}

void CPubKernel::OnAckSaveObjectData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID id;
	id.FromString(std::string(msg, nLen));

	RemoveObject(id);
}

bool CPubKernel::RemoveObject(const NFGUID& self)
{
	m_listRemovePub.push_back(self);

	return true;
}

bool CPubKernel::InnerSavePubData(const std::string& name, const NFGUID& object_id, bool bExit)
{
	OuterMsg::PubNameData xPubNameData;
	xPubNameData.set_class_name(name);
	OuterMsg::ObjectDataPack* xRoleDataPack = xPubNameData.mutable_object_data();
	PBConvert::ToPBObject(
		m_pKernelModule,
		object_id,
		xRoleDataPack,
		[](NFIProperty& prop)
		{
			return !prop.IsNull() && prop.GetSave();
		},
		[](NFIRecord& record)
		{
			return !record.IsEmpty() && record.GetSave();
		});

	int nMsgID = bExit ? OuterMsg::SS_REQ_SAVE_PUB_DATA_EXIT : OuterMsg::SS_REQ_SAVE_PUB_DATA;

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, xPubNameData);

	return true;
}

bool CPubKernel::InnerSaveGuildData(const NFGUID& object_id, bool bExit)
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

	int nMsgID = bExit ? OuterMsg::SS_REQ_SAVE_GUILD_DATA_EXIT : OuterMsg::SS_REQ_SAVE_GUILD_DATA;

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, xRoleDataPack);

	return true;
}