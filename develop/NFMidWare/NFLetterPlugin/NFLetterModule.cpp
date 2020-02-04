///---------------------------------------------------------
/// 文件名: NFLetterModule.cpp
/// 内 容:  
/// 说 明:       
/// 创建日期: 2019/08/26
/// 创建人: 于登雷
/// 修改人:
/// 版权所有: 血帆海盗团
///---------------------------------------------------------
#include "NFLetterModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFCore/NFDateTime.hpp"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"

bool NFLetterModule::Init()
{
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	return true;
}

bool NFLetterModule::AfterInit()
{
	//DB 发送的信件数量。
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_QUERY_LETTER, this, (NetMsgFun)&NFLetterModule::OnAckQueryLetterProcess);
	//DB 发送的信件信息。
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_ACK_LOOK_LETTER, this, (NetMsgFun)&NFLetterModule::OnAckLookLetterProcess);

	return true;
}

/// \brief 发送信件
bool NFLetterModule::SendLetter(const NFGUID& sender, const std::string& sendName, const NFGUID& receiver, const std::string& strTitle,
	const std::string& strContent, const NFDataList& params, const std::string& strAppend)
{
	if (receiver.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " receiver not exist:" << receiver;
		return false;
	}

	OuterMsg::LetterDataPack xdataPack;
	PBConvert::ToPB(receiver, *xdataPack.mutable_self());
	auto xdata = xdataPack.add_letter();
	char id[128];
	Port_CreateGuid(id, sizeof(id));
	xdata->set_ID(id);
	*xdata->mutable_sender() = NFToPB(sender);
	PBConvert::ToPB(sender, *xdata->mutable_sender());
	xdata->set_sendname(sendName);
	xdata->set_time(::time(nullptr));
	PBConvert::ToPB(receiver, *xdata->mutable_receiver());
	xdata->set_title(strTitle);
	xdata->set_content(strContent);
	PBConvert::NFDataListToPBDataList(params, *xdata->mutable_args());
	xdata->set_append(strAppend);
	std::string strMsg;
	if (!xdataPack.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " LetterDataPack SerializeToString failed";
		return false;
	}

	if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_SEND_LETTER, strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " SendMsgByType:" << NF_SERVER_TYPES::NF_ST_DB 
			<< " MsgID:" << OuterMsg::SS_REQ_SEND_LETTER << " failed";
	}

	return true;
}

/// \brief 查看信件数量
bool NFLetterModule::QueryLetter(const NFGUID& self)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player not exist:" << self;
		return false;
	}

	std::string strMsg;
	OuterMsg::GUID selfGuid;
	PBConvert::ToPB(self, selfGuid);
	if (!selfGuid.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " GUID SerializeToString failed";
		return false;
	}

	if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_QUERY_LETTER, strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " SendMsgByType:" << NF_SERVER_TYPES::NF_ST_DB
			<< " MsgID:" << OuterMsg::SS_REQ_QUERY_LETTER << " failed";
	}

	return true;
}

/// \brief 查看信件
bool NFLetterModule::LookLetter(const NFGUID& self, const int num)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player not exist:" << self;
		return false;
	}

	OuterMsg::ReqLookLetter xdata;
	PBConvert::ToPB(self, *xdata.mutable_self());
	xdata.set_num(num);
	std::string strMsg;
	if (!xdata.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " ReqLookLetter SerializeToString failed";
		return false;
	}

	if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_LOOK_LETTER, strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " SendMsgByType:" << NF_SERVER_TYPES::NF_ST_DB 
			<< " MsgID:" << OuterMsg::SS_REQ_LOOK_LETTER << " failed";
	}

	return true;
}

/// \brief 删除信件
bool NFLetterModule::DeleteLetter(const NFGUID& self, std::string strLetterID)
{
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player not exist:" << self;
		return false;
	}

	OuterMsg::ReqDeleteLetter xdata;
	PBConvert::ToPB(self, *xdata.mutable_self());
	xdata.set_letterID(strLetterID);
	std::string strMsg;
	if (!xdata.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << " ReqDeleteLetter SerializeToString failed";
		return false;
	}

	if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_DB, OuterMsg::SS_REQ_DELETE_LETTER, strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " SendMsgByType:" << NF_SERVER_TYPES::NF_ST_DB
			<< " MsgID:" << OuterMsg::SS_REQ_DELETE_LETTER << " failed";
	}

	return true;
}

/// \brief 查看信件数量
void NFLetterModule::OnAckQueryLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::AckQueryLetter xData;
	if (!xData.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " AckQueryLetter ParseFromArray failed";
		return;
	}

	// 没有信件
	if (xData.num() < 1)
	{
		return;
	}

	// 接收者
	NFGUID self;
	PBConvert::ToNF(xData.self(), self);
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player not exist:" << self;
		return;
	}

	// 信件信息
	NFDataList xLetterData;
	xLetterData << xData.num();

	// 查看信件数量事件
	m_pClassModule->RunEventCallBack(LC::Player::ThisName(), EVENT_OnQueryLetter, self, NFGUID(), xLetterData);
}

/// \brief 查看信件回复
void NFLetterModule::OnAckLookLetterProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::LetterDataPack xData;
	if (!xData.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " LetterDataPack ParseFromArray failed";
		return;
	}

	// 没有信件
	if (xData.letter_size() < 1)
	{
		return;
	}

	// 接收者
	NFGUID self;
	PBConvert::ToNF(xData.self(), self);
	if (self.IsNull())
	{
		QLOG_WARING << __FUNCTION__ << " player not exist:" << self;
		return;
	}

	// 信件信息
	NFDataList xLetterData;
	std::string strLetterInfo;
	xData.SerializeToString(&strLetterInfo); 
	xLetterData << strLetterInfo;

	// 查看信件回复事件
	m_pClassModule->RunEventCallBack(LC::Player::ThisName(), EVENT_OnLookLetter, self, NFGUID(), xLetterData);
}
