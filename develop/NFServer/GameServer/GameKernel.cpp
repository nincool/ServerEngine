///--------------------------------------------------------------------
/// 文件名:		GameKernel.cpp
/// 内  容:		GameServer核心模块
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "GameKernel.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"

//初始化
bool CGameKernel::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pGameServer = pPluginManager->FindModule<CGameServer>();
	m_pPlayerRenameModule = pPluginManager->FindModule<PlayerRenameModule>();
	m_pPubDataModule = pPluginManager->FindModule<CPubDataModule>();
	m_pLetterModule = pPluginManager->FindModule<NFILetterModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pCreateRoleModule = pPluginManager->FindModule<ICreateRoleModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();

	return true;
}

//模块初始化后
bool CGameKernel::AfterInit()
{
	// 获得玩家类型
	pPlayerClass = m_pClassModule->GetElement(LC::Player::ThisName());
	assert(pPlayerClass != nullptr);
	m_pNetClientModule->AddReceiveCallBack(NF_ST_DB, OuterMsg::SS_ACK_LOAD_PLAYER_GUID_NAME, this, (NetMsgFun)&CGameKernel::OnLoadPlayerGuidName);
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnAllNetComplete, METHOD_ARGS(CGameKernel::OnAllNetComplete));

	m_pClassModule->AddEventCallBack(LC::Player::ThisName(), EVENT_OnCreate, METHOD_ARGS(CGameKernel::OnPlayerCreate));
	m_pClassModule->AddEventCallBack(LC::Player::ThisName(), EVENT_OnDestroy, METHOD_ARGS(CGameKernel::OnPlayerDestroy));

	return true;
}

NFIKernelModule* CGameKernel::CKernel()
{
	return m_pKernelModule;
}

//发命令给对象
bool CGameKernel::Command(int nMsgId, const NFGUID& self, const NFGUID& target, const NFDataList& msg)
{
	if (self.IsNull() || target.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " para is null self:" << self
			<< " target:" << target << ",id:" << nMsgId;
		return false;
	}

	NF_SHARE_PTR<NFObject> pTarget = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(target));
	if (pTarget == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " target not exist target:" << target << ",id:" << nMsgId;
		return false;
	}

	pTarget->OnCommand(nMsgId, self, msg);

	return true;
}

//发COMMADND命令给所有玩家对象
bool CGameKernel::CommandByWorld(int nMsgId, const NFGUID& self, const NFDataList& msg)
{
	for (auto &it : m_hashPlayer)
	{
		it.second->OnCommand(nMsgId, self, msg);
	}

	return true;
}

bool CGameKernel::AddCustomCallBack(int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod)
{
	NF_SHARE_PTR<NFIClass> pClass = m_pClassModule->GetElement(LC::Player::ThisName());
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL name:" << LC::Player::ThisName();
		return false;
	}

	if (!pClass->AddCustomCallBack(nMsgId, pMethod))
	{
		return false;
	}
	
	m_pNetServerInsideModule->AddReceiveCallBack(nMsgId, this, (NetMsgFun)&CGameKernel::OnRecvMsgFunction);
	return true;
}

// 通用接收消息
void CGameKernel::OnRecvMsgFunction(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::MsgBase xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << "MsgBase ParseFromArray failed msg:" << nMsgID;
		return;
	}

	std::string strMsg;
	strMsg.assign(xMsg.msg_data().data(), xMsg.msg_data().length());

	NFGUID player;
	PBConvert::ToNF(xMsg.player_id(), player);

	if (!m_pKernelModule->ExistObject(player))
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << player;
		return;
	}

	// 执行客户端消息回调
	pPlayerClass->RunCustomCallBack(nMsgID, player, strMsg);
}

//发命令给对象
bool CGameKernel::Custom(int nMsgId, const NFGUID& self, const google::protobuf::Message& msg)
{
	m_pGameServer->SendMsgPBToGate(nMsgId, msg, self);
	return true;
}

//发命令给对象
bool CGameKernel::Custom(int nMsgId, const NFGUID& self, const std::string& msg)
{
	m_pGameServer->SendMsgToGate(nMsgId, msg, self);
	return true;
}
//发COMMADND命令给所有玩家对象
bool CGameKernel::CustomByWorld(int nMsgId, const google::protobuf::Message& msg)
{
	return m_pGameServer->SendMsgToClientAll(nMsgId, msg);
}

/// \breif   PlayerRename 玩家改名
bool CGameKernel::PlayerRename(const NFGUID& self, const std::string& strName)
{
	return m_pPlayerRenameModule->PlayerRename(self, strName);
}

/// \brief 发送信件
bool CGameKernel::SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
	const std::string& strContent, const NFDataList& params, const std::string& strAppend)
{
	m_pLetterModule->SendLetter(sender, sendName, receiver, strTitle, strContent, params, strAppend);
	return true;
}

/// \brief 查询信件数量
bool CGameKernel::QueryLetter(const NFGUID& self)
{
	m_pLetterModule->QueryLetter(self);
	return true;
}

/// \brief 查看信件
bool CGameKernel::LookLetter(const NFGUID& self, const int num)
{
	m_pLetterModule->LookLetter(self, num);
	return true;
}

/// \brief 删除信件
bool CGameKernel::DeleteLetter(const NFGUID& self, std::string strLetterID)
{
	m_pLetterModule->DeleteLetter(self, strLetterID);
	return true;
}

// 获得公共GUID
const NFGUID& CGameKernel::GetPubGuid(const std::string& name)
{
	return m_pPubDataModule->GetPubGuid(name);
}

// 获得公会GUID
const NFGUID& CGameKernel::GetGuildGuid(const std::string& name)
{
	return m_pPubDataModule->GetGuildGuid(name);
}

// Message事件给PUB
bool CGameKernel::CommandToPub(int nCommandId, const NFGUID& sendobj, const NFDataList& msg)
{
	if (sendobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " sendobj is null";
		return false;
	}

	return m_pPubDataModule->CommandToPub(nCommandId, sendobj, msg);
}

// Message事件给GUID
bool CGameKernel::CommandToGuid(int nCommandId, const NFGUID& sendobj, const NFGUID& targetobj, const NFDataList& msg)
{
	if (sendobj.IsNull() || targetobj.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " para is null send:" << sendobj
			<< " target:" << targetobj;
		return false;
	}

	NF_SHARE_PTR<NFObject> pTarget = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(targetobj));
	if (pTarget == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " target not exist target:" << targetobj;
		return false;
	}

	std::string strClassName = pTarget->ClassName();

	return m_pPubDataModule->CommandToGuid(nCommandId, sendobj, targetobj, strClassName, msg);
}

bool CGameKernel::AlterPlayer(const NFGUID& player, const NFDataList& args, const std::function<void(const NFGUID & self, const NFDataList & args)>& fp)
{
	return m_pCreateRoleModule->AlterPlayer(player, args, fp);
}

//获取名字
const std::string& CGameKernel::SeekRoleName(const NFGUID& self) const
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " self is null";
		return NULL_STR;
	}

	auto it = m_mapGuidName.find(self);
	if (it == m_mapGuidName.end())
	{
		return NULL_STR;
	}

	return it->second;
}

//获取Guid
const NFGUID& CGameKernel::SeekRoleGuid(const std::string& name) const
{
	if (name.empty())
	{
		QLOG_WARING << __FUNCTION__ << " name is empty";
		return NULL_OBJECT;
	}

	auto it = m_mapNameGuid.find(name);
	if (it == m_mapNameGuid.end())
	{
		return NULL_OBJECT;
	}

	return it->second;
}

const std::unordered_map<NFGUID, std::string>& CGameKernel::GetRoleGuids() const
{
	return m_mapGuidName;
}

const std::unordered_map<std::string, NFGUID>& CGameKernel::GetRoleNames() const
{
	return m_mapNameGuid;
}

NF_SHARE_PTR<NFPlayer> CGameKernel::GetPlayer(const NFGUID& player) const
{
	auto it = m_hashPlayer.find(player);
	if (it == m_hashPlayer.end())
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << player;
		return nullptr;
	}

	auto pPlayer = it->second;
	if (pPlayer == nullptr || pPlayer->GetState() == COE_DESTROY)
	{
		QLOG_WARING << __FUNCTION__ << " pPlayer == NULL" << player;
		return nullptr;
	}

	return pPlayer;
}

const CGameKernel::HashPlayer& CGameKernel::GetPlayerAll() const
{
	return m_hashPlayer;
}

bool CGameKernel::AddPlayer(NF_SHARE_PTR<NFPlayer> pPlayer)
{
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pPlayer == NULL";
		return false;
	}

	m_hashPlayer[pPlayer->Self()] = pPlayer;
	return true;
}

bool CGameKernel::RemovePlayer(const NFGUID& player)
{
	m_hashPlayer.erase(player);
	return true;
}

int CGameKernel::OnPlayerCreate(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find:" << self;
		return 0;
	}

	AddPlayer(pPlayer);
	return 0;
}

int CGameKernel::OnPlayerDestroy(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	RemovePlayer(self);
	return 0;
}

int CGameKernel::OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	// 请求加载玩家NFGUID与Name的映射表
	m_pNetClientModule->SendMsgByType(NF_ST_DB, OuterMsg::SS_REQ_LOAD_PLAYER_GUID_NAME, NULL_STR);

	return 0;
}

void CGameKernel::OnLoadPlayerGuidName(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::GuidNameList xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " GuidNameList ParseFromArray failed";
		return;
	}

	for (int i = 0; i < xMsg.list_size(); ++i)
	{
		NFGUID player;
		PBConvert::ToNF(xMsg.list(i).id(), player);
		auto& strName = xMsg.list(i).name();
		if (player.IsNull() || strName.empty())
		{
			continue;
		}

		m_mapGuidName[player] = strName;
		m_mapNameGuid[strName] = player;
	}

	if (xMsg.state() == OuterMsg::GuidNameList::INIT_FINISH)
	{
		// 进行状态控制
		m_pClassModule->RunCommandCallBack(
			pPluginManager->GetAppName(),
			OuterMsg::SS_ACK_LOAD_GUID_NAME_COMPLETE,
			NULL_OBJECT,
			NULL_OBJECT,
			NFDataList());
	}
}

//写入数据库日志
void CGameKernel::LogToDB(const std::string& recName, const NFDataList& col)
{
	OuterMsg::LogToDB xLog;
	xLog.set_rec_name(recName);

	int count = col.GetCount();
	for (int i = 0; i < count; ++i)
	{
		xLog.mutable_data_list()->add_v_data()->set_v_string(col.GetData(i).ToString());
	}

	m_pNetClientModule->SendMsgByType(NF_ST_DB, OuterMsg::SS_LOG_TO_DB, xLog);
}

bool CGameKernel::ClosurePlayer(const NFGUID& player, int64_t dnOpenTime)
{
	auto fp = [&, dnOpenTime](const NFGUID& self, const NFDataList& args)
	{
		NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
		if (pPlayer == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " player not exist" << self;
			return;
		}

		pPlayer->SetOpenTime(dnOpenTime);

		if (pPlayer->GetOnlineState() == OnlineState::ONLINE_TRUE)
		{
			m_pKernelModule->PlayerLeaveGame(self);
		}
	};

	return AlterPlayer(player, NULL_DATA_LIST, fp);
}