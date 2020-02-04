///--------------------------------------------------------------------
/// �ļ���:		CreateRoleModule.cpp
/// ��  ��:		��ɫ����
/// ˵  ��:		
/// ��������:	2019��8��20��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "CreateRoleModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFPlayer.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFUtils/StringUtil.h"

bool CCreateRoleModule::Init()
{
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pKernelModule = dynamic_cast<NFKernelModule*>(pPluginManager->FindModule<NFIKernelModule>());
	m_pGameKernel = dynamic_cast<IGameKernel*>(pPluginManager->FindModule<IGameKernel>());
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pHeartBeatModule = pPluginManager->FindModule<NFIHeartBeatModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pGameServer = pPluginManager->FindModule<CGameServer>();

	assert(m_pKernelModule != nullptr);

	return true;
}

bool CCreateRoleModule::AfterInit()
{
	m_pClassModule->AddEventCallBack(LC::Player::ThisName(), EVENT_OnCreate, METHOD_ARGS(CCreateRoleModule::OnPlayerCreate));

	//�����ɫ�б�
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ROLE_LIST, this, (NetMsgFun)&CCreateRoleModule::OnReqiureRoleListProcess);
	//DB ���͹��Ľ�ɫ�б�
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SC_ACK_ROLE_LIST, this, (NetMsgFun)&CCreateRoleModule::OnReposeRoleListProcess);

	// ���󴴽���ɫ
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_CREATE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnCreateRoleGameProcess);
	// Ԥ������ɫ�ɹ�
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_PRECREATE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnAckPreCreateRole);
	//DB ���͹��Ľ�ɫ�����ظ�
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SC_ACK_CREATE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnAckCreateRoleProcess);

	//ɾ����ɫ
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_DELETE_ROLE, this, (NetMsgFun)&CCreateRoleModule::OnDeleteRoleGameProcess);

	//���������Ϸ
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ENTER_GAME, this, (NetMsgFun)&CCreateRoleModule::OnClienEnterGameProcess);
	//DB ���ؽ�ɫ���ݷ���
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOAD_ROLE_DATA, this, (NetMsgFun)&CCreateRoleModule::OnDBLoadRoleDataProcess);

	//�ͻ��˷���������Ϸ����¼�
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::CS_REQ_ENTER_GAME_FINISH, this, (NetMsgFun)&CCreateRoleModule::OnClientEnterGameFinishProcess);

	//ɾ����ɫ
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_REQ_PLAYER_REMOVE, this, (NetMsgFun)&CCreateRoleModule::OnCommandPlayerRemove);

	// �༭�������
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_ALTER_PLAYER_DATA, this, (NetMsgFun)&CCreateRoleModule::OnAlterPlayer);

	//ɾ����ɫ
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_ACK_OFFLINE_NOTIFY, this, (NetMsgFun)&CCreateRoleModule::OnCommandPlayerOffline);
	return true;
}

bool CCreateRoleModule::Shut()
{
	return true;
}

bool CCreateRoleModule::Execute()
{
	int64_t dnCurTime = pPluginManager->GetNowTime();
	static int64_t dnLastTime = 0;
	if (dnCurTime < dnLastTime)
	{
		return true;
	}

	dnLastTime = dnCurTime + ONE_MIN_S;

	// ɾ����ʱ�༭����
	if (m_mapAlterPlayer.empty())
	{
		return true;
	}

	std::list<NFGUID> list_del;
	for (auto map_it : m_mapAlterPlayer)
	{
		bool bDel = true;
		for (auto it : map_it.second)
		{
			if (dnCurTime < it.dnDeleteTime)
			{
				bDel = false;
			}
		}

		if (bDel)
		{
			list_del.push_back(map_it.first);
		}
	}

	for (auto it : list_del)
	{
		QLOG_ERROR << __FUNCTION__ << " alterinfo over time player:" << it;
		m_mapAlterPlayer.erase(it);
	}

	return true;
}

//�����ɫ�б�
void CCreateRoleModule::OnReqiureRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, std::string(msg, nLen));
}

//DB ���͹��Ľ�ɫ�б�
void CCreateRoleModule::OnReposeRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_PROXY, nMsgID, std::string(msg, nLen));
}

//������ɫ�ص�
void CCreateRoleModule::OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::EGameEventCode eCode = OuterMsg::EGEC_UNKOWN_ERROR;
	OuterMsg::ReqCreateRole xData;
	do
	{
		if (!xData.ParseFromArray(msg, nLen))
		{
			QLOG_WARING << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
			break;
		}

		auto& strName = xData.noob_name();
		if (strName.empty())
		{
			QLOG_WARING << __FUNCTION__ << " name is empty";
			eCode = OuterMsg::EGEC_EXIST_SENSITIVE_WORD;
			break;
		}

		// ���дʼ��
		if (m_pKernelModule->ExistSensitiveWord(strName))
		{
			eCode = OuterMsg::EGEC_EXIST_SENSITIVE_WORD;
			break;
		}

		// ���ȼ��
		if (StringUtil::UTF8ToString(strName).length() > m_pGameServer->GetRoleNameMaxSize())
		{
			eCode = OuterMsg::EGEC_LENGTH_ERROR;
			break;
		}

		eCode = OuterMsg::EGEC_SUCCESS;
	} while (false);


	if (eCode == OuterMsg::EGEC_SUCCESS)
	{
		// Ԥ����
		m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_PRECREATE_ROLE, std::string(msg, nLen));
	}
	else
	{
		OuterMsg::AckEventResult xAck;
		xAck.set_event_code(eCode);
		std::string strData;
		OuterMsg::MsgBase xMsg;
		xMsg.set_msg_data(xAck.SerializeAsString());
		*xMsg.mutable_player_id() = xData.client_id();
		m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_CREATE_ROLE, xMsg, nSockIndex);
	}
}

// Ԥ�����ɹ� ��ʼ������ɫ
void CCreateRoleModule::OnAckPreCreateRole(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::EGameEventCode eCode = OuterMsg::EGEC_UNKOWN_ERROR;
	OuterMsg::ReqCreateRole xData;
	do
	{
		if (!xData.ParseFromArray(msg, nLen))
		{
			QLOG_WARING << __FUNCTION__ << " ReqCreateRole ParseFromArray failed";
			break;
		}

		m_pClassModule->CloseEvent();
		NF_SHARE_PTR<NFPlayer> pPlayer =
			dynamic_pointer_cast<NFPlayer>(m_pKernelModule->CreateObject(LC::Player::ThisName(), NULL_STR, NULL_DATA_LIST));
		m_pClassModule->OpenEvent();
		if (pPlayer == nullptr)
		{
			break;
		}

		pPlayer->SetCreateTime(pPluginManager->GetNowTime());
		pPlayer->SetName(xData.noob_name());
		pPlayer->SetAccount(xData.account());
		pPlayer->OnEvent(EVENT_OnCreateRole, NULL_OBJECT, NFDataList() << xData.msg_data());

		if (!GetData(pPlayer->Self(), *xData.mutable_roleData()))
		{
			QLOG_ERROR << __FUNCTION__ << " GetData failed";
			break;
		}

		m_pClassModule->CloseEvent();
		m_pKernelModule->DestroyPlayer(pPlayer->Self());
		m_pClassModule->OpenEvent();

		eCode = OuterMsg::EGEC_SUCCESS;

	} while (false);


	if (eCode == OuterMsg::EGEC_SUCCESS)
	{
		// ��ʽ����, �����Ѿ�����Ԥ����, ���Գ�����������, Ӧ�ñ�Ȼ�ɹ�
		m_pNetClientModule->SendMsgByType(
			NF_SERVER_TYPES::NF_ST_DB, OuterMsg::CS_REQ_CREATE_ROLE, xData);
	}
	else
	{
		OuterMsg::AckEventResult xAck;
		xAck.set_event_code(eCode);
		std::string strData;
		OuterMsg::MsgBase xMsg;
		*xMsg.mutable_player_id() = xData.client_id();
		m_pNetServerInsideModule->SendMsgToTypeServer(NF_ST_PROXY, OuterMsg::SC_ACK_CREATE_ROLE, xMsg);
	}
}

//DB ���͹��Ľ�ɫ�����ظ�
void CCreateRoleModule::OnAckCreateRoleProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID nClientID;
	OuterMsg::AckEventResult xData;
	if (!ReceivePB(nMsgID, msg, nLen, xData, nClientID))
	{
		QLOG_WARING << __FUNCTION__ << " AckEventResult ReceivePB failed";
		return;
	}

	m_pNetServerInsideModule->SendMsgToTypeServer(NF_SERVER_TYPES::NF_ST_PROXY, nMsgID, std::string(msg, nLen));
}

//ɾ����ɫ
void CCreateRoleModule::OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, std::string(msg, nLen));
}

//���������Ϸ
void CCreateRoleModule::OnClienEnterGameProcess(
	const NFSOCK nSockIndex, 
	const int nMsgID, 
	const char* msg, 
	const uint32_t nLen)
{
	NFGUID nClientID;
	OuterMsg::ReqEnterGameServer xMsg;
	if (!ReceivePB(nMsgID, msg, nLen, xMsg, nClientID))
	{
		QLOG_WARING << __FUNCTION__ << " ReqEnterGameServer ReceivePB failed";
		return;
	}

	NFGUID nRoleID = PBToNF(xMsg.id());
	if (nRoleID.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " role is null";
		return;
	}

	//ȡ�ý����������Ϣ
	auto* pServerData = m_pNetServerInsideModule->GetSockIDServer(nSockIndex);
	if (nullptr == pServerData)
	{
		QLOG_ERROR << __FUNCTION__ << " nSockIndex not exist";
		return;
	}

	int nGateID = pServerData->report.app_id();
	if (nGateID < 0)
	{
		QLOG_ERROR << __FUNCTION__ << " nGateID < 0";
		return;
	}

	//�Ѵ��� ����ȡ������� ֱ�Ӹ���������Ϣ
	auto it = m_mapConnInfo.find(nRoleID);
	bool bLoading = it != m_mapConnInfo.end();
	auto& info = m_mapConnInfo[nRoleID];
	info.nGateID = nGateID;
	info.xClientID = nClientID;
	info.strIP = xMsg.ip();
	info.strAccount = xMsg.account();
	info.strDeviceID = xMsg.device_id();
	info.strDeviceMac = xMsg.device_mac();
	info.nOsType = xMsg.os_type();

	// ��Ӧ�༭������ݻص�
	auto itAlert = m_mapAlterPlayer.find(nRoleID);
	if (itAlert != m_mapAlterPlayer.end())
	{
		// �Ѿ�����༭���ݣ��ȴ��༭���ݷ���ֱ���߼��ؽ�ɫ�߼�
		return;
	}

	// �༭��ҿ���
	EnableAlterPlayer(nRoleID);

	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(nRoleID));
	if (pPlayer == nullptr)
	{
		// �����ڣ����ؽ�ɫ
		if (!bLoading)
		{
			m_pNetClientModule->SendMsgByType(
				NF_SERVER_TYPES::NF_ST_DB,
				OuterMsg::SS_REQ_LOAD_ROLE_DATA,
				std::string(msg, nLen));
		}
		else
		{
			// ���ڼ��� �ȴ�
		}
	}
	else if (pPlayer->GetOnlineState() == OnlineState::ONLINE_ALTER)
	{
		// �༭״̬��ɫ
		// ���ڱ༭״̬�Ľ�ɫû�д����κ��¼���������Ҫ���¼�����������������������¼�
		
		// ��ȡ��ǰ�������
		OuterMsg::ObjectDataPack xRoleDataPack;
		if (!GetData(nRoleID, xRoleDataPack))
		{
			QLOG_ERROR << __FUNCTION__ << " GetData failed";
			return;
		}

		// �����༭
		CB_DestroyAlterPlayer(nRoleID, "", 0, 0);

		// �������
		LoadRole(xRoleDataPack);

	}
	else // ����
	{
		// ֪ͨ�ͻ��˽�����Ϸ
		OnPlayerCreate(nRoleID, NULL_OBJECT, NULL_DATA_LIST);

		// �·��������
		m_pClassModule->RunCommandCallBack(
			LC::Player::ThisName(), 
			OuterMsg::SS_REQ_UPDATE_PLAYER_DATA, 
			nRoleID, 
			NULL_OBJECT, 
			NULL_DATA_LIST);
	}
}

//DB ���ؽ�ɫ���ݷ���
void CCreateRoleModule::OnDBLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	// ���ؽ�ɫ
	LoadRole(xMsg);
}

bool CCreateRoleModule::LoadRole(const OuterMsg::ObjectDataPack& xMsg)
{
	NFGUID nRoleID;
	PBConvert::ToNF(xMsg.ident(), nRoleID);
	if (m_pKernelModule->ExistObject(nRoleID))
	{
		QLOG_ERROR << __FUNCTION__ << " already exist role self:" << nRoleID;
		return false;
	}

	NF_SHARE_PTR<NFObject> pObject =
		dynamic_pointer_cast<NFObject>(
			PBConvert::ToNFObject(
				m_pKernelModule,
				NFGUID(),
				&xMsg,
				NULL_DATA_LIST,
				true, false, true));
	if (pObject == nullptr)
	{
		return false;
	}

	const NFGUID& self = pObject->Self();

	//���Ӷ�ʱ������ݱ�������
	m_pHeartBeatModule->AddHeartBeat(self, 
		"SaveDataOnTime", 
		this, 
		&CCreateRoleModule::SaveDataOnTime, 
		m_pGameServer->GetSaveDataTime(),
		-1);

	// ������Ϸ�¼�
	pObject->OnEvent(EVENT_OnEntryGame, NFGUID(), NULL_DATA_LIST);

	// ��Ӧ��ұ༭
	auto itAlert = m_mapAlterPlayer.find(self);
	if (itAlert != m_mapAlterPlayer.end())
	{
		for (auto& it : itAlert->second)
		{
			it.fp(self, it.args);
		}
		m_mapAlterPlayer.erase(itAlert);
	}

	return true;
}

// ��Ҵ���
int CCreateRoleModule::OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	auto pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (nullptr == pPlayer)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player self:" << self;
		return 0;
	}

	OuterMsg::EGameEventCode eCode = OuterMsg::EGameEventCode::EGEC_SUCCESS;
	// �������
	if (!PlayerConnect(self))
	{
		eCode = OuterMsg::EGameEventCode::EGEC_UNKOWN_ERROR;
	}
	else if (pPlayer->GetOpenTime() > pPluginManager->GetNowTime())
	{
		eCode = OuterMsg::EGameEventCode::EGEC_PLAYER_CLOSURE;
	}

	//֪ͨ�ͻ��˽�����Ϸ
	OuterMsg::AckEventResult xAskMsg;
	xAskMsg.set_event_code(eCode);
	*xAskMsg.mutable_event_client() = NFToPB(pPlayer->GetClientID());
	*xAskMsg.mutable_event_object() = NFToPB(self);
	m_pGameServer->SendMsgPBToGate(OuterMsg::SC_ACK_ENTER_GAME, xAskMsg, self);

	if (eCode != OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		// ֪ͨworld�Ͽ�����
		OuterMsg::ClientConnectInfo xMsg;
		PBConvert::ToPB(self, *xMsg.mutable_player());
		PBConvert::ToPB(pPlayer->GetClientID(), *xMsg.mutable_client());
		m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_ACK_USER_BREAK, xMsg);
	}

	return 0;
}

void CCreateRoleModule::OnClientEnterGameFinishProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	NFGUID nPlayerID;
	OuterMsg::ReqAckEnterGameSuccess xMsg;
	if (!ReceivePB(nMsgID, msg, nLen, xMsg, nPlayerID))
	{
		QLOG_ERROR << __FUNCTION__ << " ReqAckEnterGameSuccess ReceivePB failed";
		return;
	}

	NF_SHARE_PTR<NFPlayer> pPlayerObj 
		= dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(nPlayerID));
	if (pPlayerObj == nullptr)
	{
		return;
	}

	// �������״̬
	pPlayerObj->SetState(COE_CREATE_CLIENT_FINISH);

	pPlayerObj->OnEvent(EVENT_OnReady, NFGUID(), NULL_DATA_LIST);

	//m_pNetServerInsideModule->SendMsg(OuterMsg::SC_ACK_ENTER_GAME_FINISH, std::string(msg, nLen), nSockIndex);
}

bool CCreateRoleModule::GetData(const NFGUID& self, OuterMsg::ObjectDataPack& xRoleDataPack)
{
	return PBConvert::ToPBObject(
		m_pKernelModule,
		self,
		&xRoleDataPack,
		[](NFIProperty& prop)
		{
			return !prop.IsNull() && prop.GetSave();
		},
		[](NFIRecord& record)
		{
			return !record.IsEmpty() && record.GetSave();
		});
}

void CCreateRoleModule::SaveData(const NFGUID& self, bool bExit/*= false*/)
{
	OuterMsg::ObjectDataPack xRoleDataPack;
	if (!GetData(self, xRoleDataPack))
	{
		QLOG_ERROR << __FUNCTION__ << " GetData failed";
		return;
	}
	
	int nMsgID = bExit ? (int)OuterMsg::SS_REQ_SAVE_ROLE_DATA_EXIT : (int)OuterMsg::SS_REQ_SAVE_ROLE_DATA;

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, nMsgID, xRoleDataPack);
}

int CCreateRoleModule::SaveDataOnTime(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// ���ݴ���¼�
	NF_SHARE_PTR<NFObject> pObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
	if (pObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find obj:" << self;
		return 0;
	}

	// ����¼�
	pObject->OnEvent(EVENT_OnStore, NFGUID(), NFDataList() << STORE_TIMING);

	SaveData(self);
	return 0;
}

void CCreateRoleModule::SendOnline(const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer>& pPlayer = 
		dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find obj:" << self;
		return;
	}

	OuterMsg::RoleOnlineNotify xMsg;
	*xMsg.mutable_self() = NFToPB(self);
	xMsg.set_game(pPluginManager->GetAppID());
	xMsg.set_proxy(pPlayer->GetGateID());
	*xMsg.mutable_client() = NFToPB(pPlayer->GetClientID());

	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_WORLD, OuterMsg::SS_ACK_ONLINE_NOTIFY, xMsg);
}

bool CCreateRoleModule::PlayerConnect(const NFGUID& self)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = 
		dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	auto it = m_mapConnInfo.find(pPlayer->Self());
	if (it == m_mapConnInfo.end())
	{
		QLOG_ERROR << __FUNCTION__ << " not find conninfo self:" << pPlayer->Self();
		return false;
	}
	auto& info = it->second;

	// ������������
	pPlayer->SetGameID(pPluginManager->GetAppID());
	pPlayer->SetGateID(info.nGateID);
	pPlayer->SetOnlineTime(pPluginManager->GetNowTime());
	pPlayer->SetOfflineTime(0);
	pPlayer->SetClientID(info.xClientID);
	pPlayer->SetAccount(info.strAccount);
	pPlayer->SetDeviceID(info.strDeviceID);
	pPlayer->SetDeviceMAC(info.strDeviceMac);
	pPlayer->SetIP(info.strIP);
	pPlayer->SetOsType(info.nOsType);
	pPlayer->SetOnlineState(OnlineState::ONLINE_TRUE);

	// ɾ����ʱ����
	m_mapConnInfo.erase(it);

	//�����������
	SendOnline(self);

	return true;
}

bool CCreateRoleModule::PlayerDisconnect(const NFGUID& self)
{
	m_mapConnInfo.erase(self);

	NF_SHARE_PTR<NFPlayer> pPlayer =
		dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	// �Ƿ�����
	if (pPlayer->GetOnlineState() != OnlineState::ONLINE_TRUE)
	{
		// ������
		return true;
	}

	// �����¼�
	pPlayer->OnEvent(EVENT_OnDisconnect, NULL_OBJECT, NULL_DATA_LIST);

	// ������������
	auto now = pPluginManager->GetNowTime();
	auto onlineTime = pPlayer->GetOnlineTime();
	pPlayer->SetGateID(0);
	pPlayer->SetOfflineTime(now);
	pPlayer->SetOnlineTime(0);
	pPlayer->SetClientID(NULL_OBJECT);
	if (onlineTime > 0 && now > onlineTime)
	{
		pPlayer->SetTotalSec(pPlayer->GetTotalSec() + now - onlineTime);
	}

	pPlayer->SetOnlineState(OnlineState::ONLINE_FALSE);

	return true;
}

const PlayerConnInfo* CCreateRoleModule::GetConnInfo(const NFGUID& player) const
{
	auto it = m_mapConnInfo.find(player);
	if (it == m_mapConnInfo.end())
	{
		return nullptr;
	}

	return &(it->second);
}

const std::map<NFGUID, PlayerConnInfo>& CCreateRoleModule::GetConnInfoMap() const
{
	return m_mapConnInfo;
}

bool CCreateRoleModule::RemoveConnInfo(const NFGUID& nRoleID)
{
	auto it = m_mapConnInfo.find(nRoleID);
	if (it == m_mapConnInfo.end())
	{
		return false;
	}

	m_mapConnInfo.erase(it);

	return true;
}

void CCreateRoleModule::OnCommandPlayerRemove(
	const NFSOCK nSockIndex, 
	const int nMsgID, 
	const char* msg, 
	const uint32_t nLen)
{
	NFGUID self;
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	PBConvert::ToNF(xMsg, self);

	// ���ݴ���¼�
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return;
	}

	if (pPlayer->GetOnlineState() == OnlineState::ONLINE_TRUE)
	{
		// ������� �ж��Ƴ��������
		QLOG_WARING << __FUNCTION__ << " player online cant remove :" << self;
		return;
	}

	// ����¼�
	pPlayer->OnEvent(EVENT_OnStore, NULL_OBJECT, NFDataList() << STORE_EXIT);

	// ����ִ������
	PlayerDisconnect(self);

	//�������-ʱ�������ݵ����ݿ�
	SaveData(self, true);

	// ɾ����ɫ
	m_pKernelModule->DestroyPlayer(self);

	// ɾ��������Ϣ
	RemoveConnInfo(self);
}

void CCreateRoleModule::OnCommandPlayerOffline(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	NFGUID self;
	OuterMsg::GUID xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GUID ParseFromArray failed";
		return;
	}

	PBConvert::ToNF(xMsg, self);

	PlayerDisconnect(self);
}

bool CCreateRoleModule::AlterPlayer(const NFGUID& player, 
	const NFDataList& args, 
	const std::function<void(const NFGUID & self, const NFDataList & args)>& fp)
{
	if (player.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player is null";
		return false;
	}

	if (m_pGameKernel->SeekRoleName(player).empty())
	{
		QLOG_ERROR << __FUNCTION__ << " not player:" << player;
		return false;
	}

	// ʹ�ñ༭״̬��ҿ���
	EnableAlterPlayer(player);

	// ��ɫ�Ѿ�����
	NF_SHARE_PTR<NFPlayer> pPlayerObj = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(player));
	if (pPlayerObj != nullptr)
	{
		// ֱ�ӵ��ûص���Ϣ
		fp(player, args);

		// �༭״̬������ɾ������
		if (pPlayerObj->GetOnlineState() == OnlineState::ONLINE_ALTER)
		{
			// ɾ������
			m_pHeartBeatModule->RemoveHeartBeat(player, "CB_DestroyAlterPlayer");
			// ��ʱɾ������
			m_pHeartBeatModule->AddHeartBeat(player,
				"CB_DestroyAlterPlayer",
				this,
				&CCreateRoleModule::CB_DestroyAlterPlayer,
				m_pGameServer->GetAlterPlayerLifeTime(),
				1);

			// ʹ�ñ༭״̬��Ҳ�����
			DisableAlterPlayer(player);
		}

		return true;
	}

	// ��������
	AlterPlayerInfo info;
	info.fp = fp;
	info.args = args;
	info.dnDeleteTime = pPluginManager->GetNowTime() + ONE_MIN_S;
	m_mapAlterPlayer[player].push_back(info);

	// ��ɫ�Ѿ���ʼ����
	auto* pConnInfo = GetConnInfo(player);
	if (pConnInfo != nullptr)
	{
		return true;
	}

	// �����������
	OuterMsg::GUID xMsg;
	PBConvert::ToPB(player, xMsg);

	return m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_ALTER_PLAYER_DATA, xMsg);
}

void CCreateRoleModule::OnAlterPlayer(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	NFGUID player;
	PBConvert::ToNF(xMsg.ident(), player);
	if (m_pKernelModule->ExistObject(player))
	{
		// ����Ѿ�����
		QLOG_ERROR << __FUNCTION__ << " role already exist:" << player;
		return;
	}

	auto itAlter = m_mapAlterPlayer.find(player);
	if (itAlter == m_mapAlterPlayer.end())
	{
		// û�д��������
		QLOG_ERROR << __FUNCTION__ << " not find player:" << player;
		return;
	}

	bool bRet = false;
	do
	{
		// ��ҵȴ���¼
		auto* pConnInfo = GetConnInfo(player);
		if (pConnInfo != nullptr)
		{
			// ��ҵ�¼
			if (LoadRole(xMsg))
			{
				return;
			}
			break;
		}

		// �ر��¼�
		m_pClassModule->CloseEvent();
		// ������ɫ
		NF_SHARE_PTR<NFPlayer> pPlayer =
			dynamic_pointer_cast<NFPlayer>(
				PBConvert::ToNFObject(
					m_pKernelModule,
					NFGUID(),
					&xMsg,
					NULL_DATA_LIST,
					true, false, true));
		// �����¼�
		m_pClassModule->OpenEvent();
		if (pPlayer == nullptr)
		{
			// ��Ҵ���ʧ��
			QLOG_ERROR << __FUNCTION__ << " create player failed";
			break;
		}

		// ��ʱ����
		m_pHeartBeatModule->AddHeartBeat(player,
			"CB_AlterPlayerSaveData",
			this,
			&CCreateRoleModule::CB_AlterPlayerSaveData,
			m_pGameServer->GetSaveDataTime(),
			-1);

		// ��ʱɾ������
		m_pHeartBeatModule->AddHeartBeat(player, 
			"CB_DestroyAlterPlayer", 
			this, 
			&CCreateRoleModule::CB_DestroyAlterPlayer, 
			m_pGameServer->GetAlterPlayerLifeTime(),
			1);

		// ��ұ༭״̬
		pPlayer->SetOnlineState(OnlineState::ONLINE_ALTER);
		
		bRet = true;

	} while (false);

	if (bRet)
	{
		// ��Ӧ�ص�
		for (auto& it : itAlter->second)
		{
			it.fp(player, it.args);
		}

		// �ر��¼�
		m_pClassModule->CloseEvent();
		// ��������
		SaveData(player);
		// �����¼�
		m_pClassModule->OpenEvent();
		// ʹ�ñ༭״̬��Ҳ�����
		DisableAlterPlayer(player);
	}
	else
	{
		// �ر��¼�
		m_pClassModule->CloseEvent();
		// ɾ������
		m_pKernelModule->DestroyObjectAny(player);
		// �����¼�
		m_pClassModule->OpenEvent();
	}
	
	// ����ص�
	m_mapAlterPlayer.erase(itAlter);
}

int CCreateRoleModule::CB_AlterPlayerSaveData(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// �༭��ҿ���
	EnableAlterPlayer(self);

	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return 0;
	}

	if (pPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		QLOG_ERROR << __FUNCTION__ << " OnlineState:" << pPlayer->GetOnlineState() << " not OnlineState::ONLINE_ALTER";
		return 0;
	}

	SaveData(self);

	// �༭��Ҳ�����
	DisableAlterPlayer(self);

	return 0;
}

int CCreateRoleModule::CB_DestroyAlterPlayer(const NFGUID& self, const char* name, const int nIntervalTime, const int nCount)
{
	// �༭��ҿ���
	EnableAlterPlayer(self);

	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find player:" << self;
		return 0;
	}

	if (pPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		return 0;
	}

	// �ر��¼�
	m_pClassModule->CloseEvent();
	// ��������
	SaveData(self);
	// ɾ������
	if (!m_pKernelModule->DestroyPlayer(self))
	{
		QLOG_ERROR << __FUNCTION__ << " destroy player failed";
	}
	// �����¼�
	m_pClassModule->OpenEvent();

	return 0;
}

bool CCreateRoleModule::EnableAlterPlayer(const NFGUID& player)
{
	// ʹ�ñ༭״̬��ҿ���
	NF_SHARE_PTR<NFPlayer> pAlterPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetDisableObject(player));
	if (pAlterPlayer == nullptr)
	{
		return false;
	}

	if (pAlterPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		QLOG_ERROR << __FUNCTION__ << " OnlineState:" << pAlterPlayer->GetOnlineState() << " not OnlineState::ONLINE_ALTER";
	}

	return m_pKernelModule->EnableObject(player);
}

bool CCreateRoleModule::DisableAlterPlayer(const NFGUID& player)
{
	// ʹ�ñ༭״̬��ҿ���
	NF_SHARE_PTR<NFPlayer> pAlterPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(player));
	if (pAlterPlayer == nullptr)
	{
		return false;
	}

	if (pAlterPlayer->GetOnlineState() != OnlineState::ONLINE_ALTER)
	{
		QLOG_ERROR << __FUNCTION__ << " OnlineState:" << pAlterPlayer->GetOnlineState() << " not OnlineState::ONLINE_ALTER";
		return false;
	}

	return m_pKernelModule->DisableObject(player);
}