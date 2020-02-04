///--------------------------------------------------------------------
/// 文件名:		PubDataModule.cpp
/// 内  容:		game映射的pub数据模块
/// 说  明:		
/// 创建日期:	2019年9月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "PubDataModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include <assert.h>
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFComm/NFPluginModule/KConstDefine.h"

CPubDataModule::CPubDataModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

CPubDataModule::~CPubDataModule()
{
}

bool CPubDataModule::Init()
{
	mLastCheckTime = pPluginManager->GetNowTime();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

	return true;
}

bool CPubDataModule::Shut()
{
	return true;
}

bool CPubDataModule::AfterInit()
{
	m_pClassModule->AddEventCallBack(pPluginManager->GetAppName(), EVENT_OnAllNetComplete, METHOD_ARGS(CPubDataModule::OnAllNetComplete));
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_START_LOAD_PUB, this, (NetMsgFun)&CPubDataModule::AckStartLoadPub);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_START_LOAD_GUILD, this, (NetMsgFun)&CPubDataModule::AckStartLoadGuild);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_CREATE_PUB, this, (NetMsgFun)&CPubDataModule::AckCreatePub);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_CREATE_GUILD, this, (NetMsgFun)&CPubDataModule::AckCreateGuild);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_LOAD_PUB_DATA, this, (NetMsgFun)&CPubDataModule::AckLoadData);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_ACK_LOAD_GUILD_DATA, this, (NetMsgFun)&CPubDataModule::AckLoadData);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_REQ_DELETE_PUB, this, (NetMsgFun)&CPubDataModule::OnDeletePub);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_REQ_DELETE_GUILD, this, (NetMsgFun)&CPubDataModule::OnDeleteGuild);

	return true;
}

bool CPubDataModule::Execute()
{
	return true;
}

int CPubDataModule::OnAllNetComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args)
{
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_REQ_SYN_INFO, std::string());

	return 0;
}

void CPubDataModule::AckStartLoadPub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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
}

void CPubDataModule::AckStartLoadGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
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
}

void CPubDataModule::AckCreatePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckResult xAckResult;
	if (!xAckResult.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckResult ParseFromArray failed";
		return;
	}

	if (xAckResult.event_code() == OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		const std::string& name = xAckResult.name();
		NFGUID id;
		PBConvert::ToNF(xAckResult.obj_id(), id);
		OnMapAddIndex(mxMapPubGuid, name, id);
	}
}

void CPubDataModule::AckCreateGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckResult xAckResult;
	if (!xAckResult.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " AckResult ParseFromArray failed";
		return;
	}

	if (xAckResult.event_code() == OuterMsg::EGameEventCode::EGEC_SUCCESS)
	{
		const std::string& name = xAckResult.name();
		NFGUID id;
		PBConvert::ToNF(xAckResult.obj_id(), id);
		OnMapAddIndex(mxMapGuildGuid, name, id);
	}
}

void CPubDataModule::AckLoadData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ObjectDataPack xObjectDataPack;
	if (!xObjectDataPack.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ObjectDataPack ParseFromArray failed";
		return;
	}

	LoadData(&xObjectDataPack);
}

NF_SHARE_PTR<NFObject> CPubDataModule::LoadData(OuterMsg::ObjectDataPack* xObjectDataPack)
{
	NFGUID nObjectID;
	PBConvert::ToNF(xObjectDataPack->ident(), nObjectID);
	NF_SHARE_PTR<NFObject> pTempObject = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(nObjectID));
	if (pTempObject != nullptr)
	{
		m_pKernelModule->DestroyObject(nObjectID);
	}

	//创建玩家对象基础属性设置
	NFDataList var;
	NF_SHARE_PTR<NFObject> pObject =
		dynamic_pointer_cast<NFObject>(
			PBConvert::ToNFObject(
			(NFKernelModule*)m_pKernelModule,
				NFGUID(),
				xObjectDataPack,
				var));

	return pObject;
}

// Map添加数据
bool CPubDataModule::OnMapAddIndex(map<std::string, NFGUID>& mapInfo, const std::string& key, NFGUID& id)
{
	if (key.empty())
	{
		return true;
	}

	map<std::string, NFGUID>::iterator iterFind = mapInfo.find(key);
	if (iterFind != mapInfo.end())
	{
		mapInfo.erase(iterFind);
	}

	mapInfo.insert(make_pair(key, id));
	return true;
}

// 获得公共GUID
const NFGUID& CPubDataModule::GetPubGuid(const std::string& name)
{
	map<std::string, NFGUID>::iterator iterFind = mxMapPubGuid.find(name);
	if (iterFind == mxMapPubGuid.end())
	{
		return NULL_OBJECT;
	}

	return iterFind->second;
}

// 获得公会GUID
const NFGUID& CPubDataModule::GetGuildGuid(const std::string& name)
{
	map<std::string, NFGUID>::iterator iterFind = mxMapGuildGuid.find(name);
	if (iterFind == mxMapGuildGuid.end())
	{
		return NULL_OBJECT;
	}

	return iterFind->second;
}

bool CPubDataModule::CommandToPub(int nCommandID, const NFGUID& sendobj, const NFDataList& msg)
{
	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandID);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::NFDataListToPBDataList(msg, *commandMsg.mutable_data());

	std::string strCommandMsg;
	commandMsg.SerializeToString(&strCommandMsg);
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_COMMAND_TOPUB, strCommandMsg);

	return true;
}

bool CPubDataModule::CommandToGuid(int nCommandID, const NFGUID& sendobj, const NFGUID& targetobj, const std::string& strTargetClassName, const NFDataList& msg)
{
	OuterMsg::CommandMsg commandMsg;
	commandMsg.set_Id(nCommandID);
	PBConvert::ToPB(sendobj, *commandMsg.mutable_self());
	PBConvert::ToPB(targetobj, *commandMsg.mutable_target());
	commandMsg.set_target_class_Name(strTargetClassName);
	PBConvert::NFDataListToPBDataList(msg, *commandMsg.mutable_data());

	std::string strCommandMsg;
	commandMsg.SerializeToString(&strCommandMsg);
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_PUB, OuterMsg::SS_COMMAND_TOPUB, strCommandMsg);

	return true;
}

void CPubDataModule::OnDeletePub(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xPubOpData;
	if (!xPubOpData.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	map<std::string, NFGUID>::iterator iterFind = mxMapPubGuid.find(xPubOpData.PubName());
	if (iterFind != mxMapPubGuid.end())
	{
		mxMapPubGuid.erase(iterFind);
	}

	NFGUID id;
	PBConvert::ToNF(xPubOpData.pub_id(), id);
	m_pKernelModule->DestroyObject(id);
}

void CPubDataModule::OnDeleteGuild(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::PubOpData xPubOpData;
	if (!xPubOpData.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " PubOpData ParseFromArray failed";
		return;
	}

	map<std::string, NFGUID>::iterator iterFind = mxMapGuildGuid.find(xPubOpData.PubName());
	if (iterFind != mxMapGuildGuid.end())
	{
		mxMapGuildGuid.erase(iterFind);
	}

	NFGUID id;
	PBConvert::ToNF(xPubOpData.pub_id(), id);
	m_pKernelModule->DestroyObject(id);
}