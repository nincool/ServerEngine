///--------------------------------------------------------------------
/// 文件名:		PlayerRenameModule.cpp
/// 内  容:		改名
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "PlayerRenameModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFCore/NFPlayer.h"

PlayerRenameModule::PlayerRenameModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

bool PlayerRenameModule::Init()
{
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pCreateRoleModule = pPluginManager->FindModule<ICreateRoleModule>();
	m_pNFIClassModule = pPluginManager->FindModule<NFIClassModule>();

	return true;
}

bool PlayerRenameModule::AfterInit()
{
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_PLAYER_RENAME, this, (NetMsgFun)&PlayerRenameModule::OnPlayerRename);

	return true;
}

bool PlayerRenameModule::PlayerRename(const NFGUID& self, const std::string& strName)
{
	NF_SHARE_PTR<NFPlayer> pSelf = dynamic_pointer_cast<NFPlayer>(m_pKernelModule->GetObject(self));
	if (nullptr == pSelf)
	{
		QLOG_WARING << __FUNCTION__ << " player not find:" << self;
		return false;
	}

	OuterMsg::PlayerRename rename;
	rename.set_account(pSelf->GetAccount());
	*rename.mutable_player_id() = NFToPB(self);
	rename.set_player_name_old(pSelf->GetName());
	rename.set_player_name_new(strName);

	std::string strRenamePB;
	rename.SerializeToString(&strRenamePB);
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_PLAYER_RENAME, strRenamePB);

	return true;
}

void PlayerRenameModule::OnPlayerRename(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PlayerRename rename;
	rename.ParseFromArray(msg, nLen);

	NFGUID self = PBToNF(rename.player_id());

	NFDataList arg;
	if (rename.player_name_new() == rename.player_name_old())
	{
		//改名失败重名了
		arg.AddInt(0);
	}
	else
	{
		NF_SHARE_PTR<NFObject> pSelf = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(self));
		if (nullptr != pSelf)
		{
			pSelf->SetName(rename.player_name_new());
			pSelf->SetPropertyString(LC::Player::Name(), rename.player_name_new());
		}

		arg.AddInt(1);
		arg.AddString(rename.player_name_old());
		arg.AddString(rename.player_name_new());
	}
	
	m_pNFIClassModule->RunEventCallBack(LC::Player::ThisName(), EVENT_OnChangeName, self, self, arg);
}