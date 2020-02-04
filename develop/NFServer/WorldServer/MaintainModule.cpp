///--------------------------------------------------------------------
/// 文件名:		MaintainModule.cpp
/// 内  容:		维护模块
/// 说  明:		
/// 创建日期:	2019年12月11日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "MaintainModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/StringUtil.h"

MaintainModule::MaintainModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

MaintainModule::~MaintainModule()
{
}

bool MaintainModule::Init()
{
	m_pWorldServer = pPluginManager->FindModule<CWorldServer>();
	m_pMaintainNetServer = pPluginManager->FindModule<NetServerMaintainModule>();
	m_pWorldServerState = pPluginManager->FindModule<CWorldServerState>();
	m_pWorldPlayers = pPluginManager->FindModule<CWorldPlayers>();
	
	return true;
}

bool MaintainModule::AfterInit()
{
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_SERVER_LIST, this, (NetMsgFun)&MaintainModule::OnGetServerList);


	m_pMaintainNetServer->AddCommandCallBack("close_server", this, (MaintainCBFun)&MaintainModule::RunCloseServer);
	m_pMaintainNetServer->AddCommandCallBack("get_server_state", this, (MaintainCBFun)&MaintainModule::RunGetServerState);
	m_pMaintainNetServer->AddCommandCallBack("get_player_count", this, (MaintainCBFun)&MaintainModule::RunGetPlayerCount);
	m_pMaintainNetServer->AddCommandCallBack("set_client_version", this, (MaintainCBFun)&MaintainModule::RunSetClientVersion);
	m_pMaintainNetServer->AddCommandCallBack("set_players_max", this, (MaintainCBFun)&MaintainModule::RunSetPlayersMax);

	m_pMaintainNetServer->AddCommandCallBack("white_ip_switch", this, (MaintainCBFun)&MaintainModule::RunWhiteIPSwitch);
	m_pMaintainNetServer->AddCommandCallBack("add_white_ip", this, (MaintainCBFun)&MaintainModule::RunAddWhiteIP);
	m_pMaintainNetServer->AddCommandCallBack("clear_white_ip", this, (MaintainCBFun)&MaintainModule::RunClearWhiteIP);
	m_pMaintainNetServer->AddCommandCallBack("is_white_ip", this, (MaintainCBFun)&MaintainModule::RunIsWhiteIP);

	return true;
}

void MaintainModule::OnGetServerList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	REQGetServerInfoList req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetServerInfoList ack;
	ack.u = req.u;

	m_pWorldServer->GetServerList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);

	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

std::string MaintainModule::RunCloseServer(std::vector<std::string>& arg)
{
	m_pWorldServer->CommandShut();

	return "begin close server ing...";
}

std::string MaintainModule::RunGetServerState(std::vector<std::string>& arg)
{
	return m_pWorldServerState->GetState();
}

std::string MaintainModule::RunGetPlayerCount(std::vector<std::string>& arg)
{
	return std::to_string(m_pWorldPlayers->GetPlayerCount());
}

std::string MaintainModule::RunSetClientVersion(std::vector<std::string>& arg)
{
	if (arg.size() < 2)
	{
		return "Parameter error.";
	}

	m_pWorldServer->SetClientVersion(StringUtil::StringAsInt(arg[1].c_str()));
	return "ok.";
}

std::string MaintainModule::RunSetPlayersMax(std::vector<std::string>& arg)
{
	if (arg.size() < 2)
	{
		return "Parameter error.";
	}

	m_pWorldServer->SetPlayersMax(StringUtil::StringAsInt(arg[1].c_str()));
	return "ok.";
}

//白名单
std::string MaintainModule::RunWhiteIPSwitch(std::vector<std::string>& arg)
{
	if (arg.size() < 2)
	{
		return "Parameter error.";
	}

	m_pWorldServer->SetWhiteIP(arg[1] == "1" ? true : false);
	return "ok.";
}
std::string MaintainModule::RunAddWhiteIP(std::vector<std::string>& arg)
{
	if (arg.size() < 3)
	{
		return "Parameter error.";
	}

	std::string ipList = arg[1] + arg[2];
	m_pWorldServer->AddWhiteIPList(ipList.c_str());
	return "ok.";
}
std::string MaintainModule::RunClearWhiteIP(std::vector<std::string>& arg)
{
	m_pWorldServer->ClearWhiteIP();
	return "ok.";
}
std::string MaintainModule::RunIsWhiteIP(std::vector<std::string>& arg)
{
	if (arg.size() < 2)
	{
		return "Parameter error.";
	}
	unsigned int nIP = convert_ip(arg[1].c_str());
	if (m_pWorldServer->IsWhiteIP(nIP))
	{
		return "true.";
	}
	
	return "false.";
}