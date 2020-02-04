///--------------------------------------------------------------------
/// 文件名:		RankModule.cpp
/// 内  容:		
/// 说  明:		
/// 创建日期:	2019年9月20日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "RankModule.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFPluginModule/NFIEventModule.h"
#include "NFComm/NFUtils/NFTool.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/LuaExt.hpp"
#include "NFComm/NFUtils/pb_json/pb2json.hpp"

const int REFRESH_TIME = 2;
enum MsgType
{
	MSG_TCP_SERVER,			// 服务器
	MSG_HTTP_CLIENT,		// 客户端
	MSG_MAINTAIN,			// 维护
};

bool RankModule::Init()
{
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pNetServerInsideModule = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pHttpNetModule = pPluginManager->FindModule<NFIHttpServerModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();

	m_pMaintainNetServer = pPluginManager->FindModule<NetServerMaintainModule>();

	return true;
}

bool RankModule::AfterInit()
{
	m_pNetServerInsideModule->AddReceiveCallBack(OuterMsg::SS_REQ_EXTRA_SERVER, this, (NetMsgFun)&RankModule::OnExtraMsg);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RETURN_RANK_LIST, this, (NetMsgFun)&RankModule::OnDBRunturnMsg);
	m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_INIT, this, (NetMsgFun)&RankModule::OnDBRankListInit);

	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::MSG_RANK_GET_DATA), NFHttpType::NF_HTTP_REQ_POST, this, &RankModule::OnHttpGetData);
	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::MSG_RANK_GET_DATA_ARRAY), NFHttpType::NF_HTTP_REQ_POST, this, &RankModule::OnHttpGetDataArray);
	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::MSG_RANK_GET_RANK), NFHttpType::NF_HTTP_REQ_POST, this, &RankModule::OnHttpGetRank);
	m_pHttpNetModule->AddRequestHandler("/" + to_string(OuterMsg::MSG_RANK_GET_RANGE), NFHttpType::NF_HTTP_REQ_POST, this, &RankModule::OnHttpGetRange);

	// 维护命令 
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::RUN_NET_COMMAND, this, (NetMsgFun)&RankModule::OnMaintaninMsg);

	return true;
}

bool RankModule::InitLua()
{
	LuaIntf::LuaBinding(mLuaContext).beginClass<RankModule>("CRankServer")
		.addFunction("connect_server", &RankModule::ConnectServer)
		.addFunction("create_server", &RankModule::CreateServer)
		.endClass();

	OnLuaExt(pPluginManager, m_pKernelModule, m_pLogModule, mLuaContext);

	TRY_LOAD_SCRIPT_FLE(pPluginManager->GetLuaName().c_str());
	TRY_RUN_GLOBAL_SCRIPT_FUN1("main", this);

	return true;
}

bool RankModule::InitNetServer()
{
	// 创建服务器网络
	for (int i = 0; i < mxNetList.size(); ++i)
	{
		switch (mxNetList[i].GetNetType())
		{
		case NET_TYPE_INNER:
			m_pNetServerInsideModule->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_OUTER:
			break;
		case NET_TYPE_MAINTAIN:
			m_pMaintainNetServer->InitNet(mxNetList[i].GetBufferSize(), mxNetList[i].GetMaxConnect(),
				mxNetList[i].GetIP(), mxNetList[i].GetPort());
			break;
		case NET_TYPE_HTTP:
			m_pHttpNetModule->InitServer(mxNetList[i].GetIP().c_str(), mxNetList[i].GetPort());
			break;
		default:
			break;
		}
	}

	return true;
}

bool RankModule::InitNetClient()
{
	for (auto& it : mxConnectList)
	{
		m_pNetClientModule->AddServerConnect(it);
	}

	SetNetClientReport(false);

	return true;
}

void RankModule::SetNetClientReport(bool bInitComplete)
{
	// 更新report信息
	OuterMsg::ServerInfoReport xServerInfoReport;
	xServerInfoReport.set_district_id(pPluginManager->GetDistrictID());
	xServerInfoReport.set_app_id(pPluginManager->GetAppID());
	xServerInfoReport.set_server_id(pPluginManager->GetServerID());
	*(xServerInfoReport.mutable_server_name()) = pPluginManager->GetAppName();
	xServerInfoReport.set_server_state(bInitComplete ? OuterMsg::EST_NARMAL : OuterMsg::EST_CRASH);
	xServerInfoReport.set_state_info(bInitComplete ? "Opened" : "Closed");
	xServerInfoReport.set_server_type(pPluginManager->GetServerType());
	xServerInfoReport.set_server_ip(GetTypeIP(NET_TYPE_INNER));
	xServerInfoReport.set_server_port(GetTypePort(NET_TYPE_INNER));
	xServerInfoReport.set_maintain_ip(GetTypeIP(NET_TYPE_MAINTAIN));
	xServerInfoReport.set_maintain_port(GetTypePort(NET_TYPE_MAINTAIN));

	std::string strMsg;
	xServerInfoReport.SerializeToString(&strMsg);
	m_pNetClientModule->SetReport(strMsg);
	m_pNetServerInsideModule->SetReport(strMsg);
}

bool RankModule::Shut()
{
	return true;
}

bool RankModule::Execute()
{
	if (m_state != RANK_STATE_INIT_REQ)
	{
		return true;
	}

	static int64_t time = 0;
	int64_t cur_time = pPluginManager->GetNowTime();
	if ((cur_time - time) < REFRESH_TIME)
	{
		return true;
	}
	time = cur_time;

	m_mapRankList.clear();
	m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_INIT, NULL_STR);

	return true;
}

void RankModule::OnExtraMsg(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << "ExtraMsg ParseFromArray failed";
		return;
	}

	xMsgBase.set_socket(nSockIndex);
	xMsgBase.set_msg_type(MsgType::MSG_TCP_SERVER);

	if (m_state != RANK_STATE_INIT_FINISH)
	{
		QLOG_WARING << __FUNCTION__ << " rank list initing";
		return;
	}

	switch (xMsgBase.msg_id())
	{
	case OuterMsg::RankListMsgID::MSG_RANK_CREATE:
		CreateRankList(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_DELETE:
		DeleteRankList(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_SET_SCORE:
		SetScore(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_SET_DATA:
		SetData(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_GET_DATA:
		GetData(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_GET_DATA_ARRAY:
		GetDataArray(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_GET_RANK:
		GetRank(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_GET_RANGE:
		GetRange(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_SWAP_RANK:
		SwapRank(xMsgBase);
		break;
	case OuterMsg::RankListMsgID::MSG_RANK_GET_RANK_ARRAY:
		GetRankArray(xMsgBase);
		break;
	default:
		QLOG_WARING << __FUNCTION__ << " msg:" << xMsgBase.msg_id() << " not define";
		return;
	}

	// 不需要在此回应消息(上面是异步处理会在其他地方回应消息)
	if (xMsgBase.data().empty())
	{
		return;
	}

	// 回应消息
	std::string strMsg;
	if (!xMsgBase.SerializeToString(&strMsg))
	{
		QLOG_WARING << __FUNCTION__ << "SerializeToString failed";
		return;
	}
	SendAckMessage(strMsg, xMsgBase);
}

void RankModule::OnDBRunturnMsg(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsgBase;
	if (!xMsgBase.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " ExtraMsg ParseFromArray failed";
		return;
	}

	SendAckMessage(std::string(msg, nLen), xMsgBase);
}

// 应答消息
void RankModule::SendAckMessage(const std::string& strMsgBase, OuterMsg::ExtraMsg& xMsgBase)
{
	switch (xMsgBase.msg_type())
	{
	case MsgType::MSG_TCP_SERVER:
		m_pNetServerInsideModule->SendMsg(OuterMsg::SS_ACK_EXTRA_SERVER, strMsgBase, xMsgBase.socket());
		break;
	case MsgType::MSG_HTTP_CLIENT:
	{
		NFHttpRequest* pReq = m_pHttpNetModule->GetNFHttpRequest(xMsgBase.socket());
		if (pReq != nullptr)
		{
			m_pHttpNetModule->ResponseMsgByHead(*pReq, xMsgBase.data(), xMsgBase.msg_id());
		}
		break;
	}
	case MsgType::MSG_MAINTAIN:
	{
		Pb2Json::Json json;
		switch (xMsgBase.msg_id())
		{
		case OuterMsg::MSG_RANK_GET_RANGE:
		{
			OuterMsg::RankListRangeAck xRange;
			xRange.ParseFromString(xMsgBase.data());
			Pb2Json::Message2Json(xRange, json);
			break;
		}
		default:
			json["ret_code"] = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

		m_pMaintainNetServer->SendMsg(OuterMsg::RUN_NET_COMMAND, json.dump(), xMsgBase.socket());
		break;
	}
	default:
		break;
	}

	QLOG_INFO << "Ack Server MsgID:" << xMsgBase.msg_id() << " MsgType:" << xMsgBase.msg_type();
}

void RankModule::OnDBRankListInit(
	const NFSOCK nSockIndex,
	const int nMsgID,
	const char* msg,
	const uint32_t nLen)
{
	if (m_state == RANK_STATE_INIT_FINISH)
	{
		return;
	}

	OuterMsg::RankListInit xInit;
	if (!xInit.ParseFromArray(msg, nLen))
	{
		QLOG_WARING << __FUNCTION__ << " RankListInit ParseFromArray failed";
		return;
	}

	if (xInit.finish() != 0)
	{
		m_state = RANK_STATE_INIT_FINISH;

		QuickLog::GetRef().OpenNextSystemEchoGreen();
		QLOG_SYSTEM_S << "===================ready====================";

		CATCH_BEGIN
		auto& gauge_family = pPluginManager->GetMetricsGauge(1);
		auto& second_gauge = gauge_family.Add({ {"State", "Opened"} });
		second_gauge.Increment();
		CATCH_END

		// 发送消息通知服务器初始化完毕
		SetNetClientReport(true);
		return;
	}
	m_state = RANK_STATE_INIT_ING;

	std::string strRankName = xInit.create().name();
	if (strRankName.empty())
	{
		QLOG_ERROR << __FUNCTION__ << " init rank list name empty";
		return;
	}

	QLOG_INFO_S << "init rank list begin : " << strRankName;

	OuterMsg::ExtraMsg xMsgBase;

	// 创建排行榜
	std::string strCreate;
	auto& xCreate = xInit.create();
	if (!xCreate.SerializeToString(&strCreate))
	{
		QLOG_WARING << " SerializeToString failed";
		return;
	}

	xMsgBase.set_data(strCreate);
	if (!CreateRankList(xMsgBase, true))
	{
		QLOG_ERROR << __FUNCTION__ << " init rank list failed. create failed:" << strRankName;
		return;
	}

	// 添加分数
	for (int i = 0; i < xInit.data_rank_size(); ++i)
	{
		auto* pScore = xInit.mutable_data_rank(i);
		if (pScore == nullptr)
		{
			continue;
		}
		pScore->set_name(xCreate.name());
		SetScore(*pScore, false);
	}

	int nLength = 0;
	auto it = m_mapRankList.find(strRankName);
	if (it != m_mapRankList.end() && it->second != nullptr)
	{
		nLength = it->second->GetLength();
	}

	QLOG_INFO_S << "init rank list end : " << strRankName
		<< " rank_count:" << xInit.data_rank_size()
		<< " list_length:" << nLength;
}

bool RankModule::CreateRankList(OuterMsg::ExtraMsg& xMsgBase, bool bInit/* = false*/)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;
	OuterMsg::RankListCreate xCreate;

	do
	{
		if (!xCreate.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		const std::string& strListName = xCreate.name();
		if (strListName.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
			QLOG_WARING << __FUNCTION__ << " rank list name empty";
			break;
		}

		if (m_mapRankList.find(strListName) != m_mapRankList.end())
		{
			// 已经存在，不能重新创建
			nRetCode = OuterMsg::RANK_CODE_EXIST;
			break;
		}

		std::vector<std::string> vec_score_name;
		std::vector<RankOrderType> vec_order;
		for (int i = 0; i < xCreate.order_size(); ++i)
		{
			vec_score_name.push_back(xCreate.order(i).name());
			vec_order.push_back(static_cast<RankOrderType>(xCreate.order(i).data()));
		}

		if (vec_order.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_ORDER_EMPTY;

			QLOG_WARING << __FUNCTION__ << " rank list order empty: " << strListName;
			break;
		}

		std::vector<std::string> vec_data;
		for (int i = 0; i < xCreate.data_name_size(); ++i)
		{
			vec_data.push_back(xCreate.data_name(i));
		}

		if (vec_data.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_DATA_EMPTY;

			QLOG_WARING << __FUNCTION__ << " rank list data name empty:" << strListName;
			break;
		}

		// 创建
		auto list = NF_MAKE_SPTR<SkipList>(strListName, xCreate.limit(), vec_order, vec_data);
		if (list == nullptr)
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;

			QLOG_WARING << __FUNCTION__ << "rank list failed:" << strListName;
			break;
		}
		list->SetScoreNames(vec_score_name);
		m_mapRankList[strListName] = list;

		if (!bInit)
		{
			// 操作数据库
			if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_CREATE, xMsgBase.data()))
			{
				nRetCode = OuterMsg::RANK_CODE_UNKNOW;
				break;
			}
		}

	} while (false);

	// 设定回应数据
	OuterMsg::RankListCreateAck reply;
	reply.set_ret_code(nRetCode);
	reply.set_name(xCreate.name());
	*reply.mutable_attach_data() = xCreate.attach_data();
	std::string strReply;
	reply.SerializeToString(&strReply);
	xMsgBase.set_data(std::move(strReply));

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xCreate.name()
		<< " RetCode:" << nRetCode;
	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::DeleteRankList(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;
	OuterMsg::RankListDelete xDelete;

	do
	{
		if (!xDelete.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		const std::string& strListName = xDelete.name();
		if (strListName.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
			QLOG_WARING << __FUNCTION__ << " rank list name empty";
			break;
		}

		auto it = m_mapRankList.find(strListName);
		if (it == m_mapRankList.end())
		{
			// 排行榜不存在
			nRetCode = OuterMsg::RANK_CODE_NOT_EXIST;
			break;
		}

		m_mapRankList.erase(it);

		// 操作数据库
		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_DELETE, xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

	} while (false);

	// 设定回应数据
	OuterMsg::RankListDeleteAck reply;
	reply.set_ret_code(nRetCode);
	reply.set_name(xDelete.name());
	*reply.mutable_attach_data() = xDelete.attach_data();
	std::string strReply;
	reply.SerializeToString(&strReply);
	xMsgBase.set_data(strReply);

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xDelete.name()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::SetScore(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;
	OuterMsg::RankListScore xScore;

	do
	{
		if (!xScore.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		nRetCode = SetScore(xScore);
		
	} while (false);

	// 设定回应数据
	OuterMsg::RankListScoreAck reply;
	reply.set_name(xScore.name());
	*reply.mutable_id() = xScore.id();
	reply.set_ret_code(nRetCode);
	*reply.mutable_attach_data() = xScore.attach_data();
	std::string strReply;
	reply.SerializeToString(&strReply);
	xMsgBase.set_data(strReply);

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xScore.name()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

OuterMsg::RankListCode RankModule::SetScore(OuterMsg::RankListScore& xScore, bool bInit/* = false*/)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;

	const std::string& strRankName = xScore.name();
	if (strRankName.empty())
	{
		nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
		QLOG_WARING << __FUNCTION__ << " rank list name empty";
		return nRetCode;
	}

	auto it = m_mapRankList.find(strRankName);
	if (it == m_mapRankList.end())
	{
		// 排行榜不存在
		nRetCode = OuterMsg::RANK_CODE_NOT_EXIST;
		return nRetCode;
	}

	auto& list = *it->second;
	NFGUID self;
	PBConvert::ToNF(xScore.id(), self);

	// 添加分数
	OuterMsg::RankListUpdate xUpdate;
	if (xScore.arr_data_size() > 0)
	{
		// 添加时间戳
		if (xScore.arr_data_size() != list.GetOrders().size())
		{
			nRetCode = OuterMsg::RANK_CODE_SCORE_NOT_MATCH;
			return nRetCode;
		}

		std::vector<int64_t> vec_score;
		for (int i = 0; i < xScore.arr_data_size(); ++i)
		{
			auto& data = xScore.arr_data(i);
			vec_score.push_back(data.data());
		}

		NFGUID add;
		NFGUID del;
		if (!list.Insert(self, vec_score, xScore.insert_time(), add, del))
		{
			// 未上榜(未从榜单外进入榜单，原来就在榜则仍然会在榜单中，只是名次可能掉落到最大名次之外了)
			nRetCode = OuterMsg::RANK_CODE_NO_ON_LIST;
			return nRetCode;
		}

		// 更新排行榜榜单数据
		xUpdate.set_name(strRankName);
		PBConvert::ToPB(del, *xUpdate.mutable_del());
		auto* pUpdateScore = xUpdate.mutable_update();
		if (pUpdateScore != nullptr)
		{
			*pUpdateScore->mutable_arr_data() = xScore.arr_data();
			*pUpdateScore->mutable_id() = xScore.id();
			pUpdateScore->set_insert_time(xScore.insert_time());
		}
	}
	else
	{
		if (!list.Remove(self))
		{
			nRetCode = OuterMsg::RANK_CODE_NO_ON_LIST;
			return nRetCode;
		}

		// 删除数据
		xUpdate.set_name(strRankName);
		PBConvert::ToPB(self, *xUpdate.mutable_del());
	}

	if (!bInit && !xUpdate.name().empty())
	{
		std::string strUdpate;
		if (!xUpdate.SerializeToString(&strUdpate))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			return nRetCode;
		}

		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_UPDATE, strUdpate))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			return nRetCode;
		}
	}

	return nRetCode;
}

bool RankModule::SetData(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;

	OuterMsg::RankListData xData;
	NFGUID self;

	do
	{
		if (!xData.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		PBConvert::ToNF(xData.id(), self);

		// 数据库 设置数据
		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_SET_DATA, xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

	} while (false);

	// 设定回应数据
	OuterMsg::RankListDataAck reply;
	*reply.mutable_id() = xData.id();
	reply.set_ret_code(nRetCode);
	*reply.mutable_attach_data() = xData.attach_data();
	std::string strReply;
	reply.SerializeToString(&strReply);
	xMsgBase.set_data(strReply);

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::GetData(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;
	OuterMsg::RankListData xData;

	// 是否立刻回应消息
	bool bReply = false;
	do
	{
		if (!xData.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		// 获取数据库数据
		std::string strMsg;
		if (!xMsgBase.SerializeToString(&strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			break;
		}

		// 数据库取数据
		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_GET_DATA, strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

	} while (false);

	NFGUID self;
	PBConvert::ToNF(xData.id(), self);

	bReply = bReply || nRetCode != OuterMsg::RANK_CODE_SUCCEED;
	if (bReply)
	{
		// 设定回应数据
		OuterMsg::RankListDataAck reply;
		*reply.mutable_id() = xData.id();
		reply.set_ret_code(nRetCode);
		*reply.mutable_attach_data() = xData.attach_data();
		std::string strReply;
		reply.SerializeToString(&strReply);
		xMsgBase.set_data(strReply);
	}
	else
	{
		xMsgBase.set_data("");
	}

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::GetDataArray(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;

	// 是否立刻回应消息
	bool bReply = false;
	do
	{
		// 获取数据库数据
		std::string strMsg;
		if (!xMsgBase.SerializeToString(&strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			break;
		}

		// 数据库取数据
		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_GET_DATA_ARRAY, strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

	} while (false);

	xMsgBase.set_data("");

	bReply = bReply || nRetCode != OuterMsg::RANK_CODE_SUCCEED;
	if (bReply)
	{
		OuterMsg::RankListDataArray xDataArray;
		if (xDataArray.ParseFromString(xMsgBase.data()))
		{
			// 设定回应数据
			OuterMsg::RankListDataArrayAck reply;
			reply.set_ret_code(nRetCode);
			*reply.mutable_attach_data() = xDataArray.attach_data();
			std::string strReply;
			reply.SerializeToString(&strReply);
			xMsgBase.set_data(strReply);
		}
	}

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::GetRank(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;
	OuterMsg::RankListRank xRank;
	NFGUID self;

	// 是否立刻回应
	bool bReply = false;
	int nRank = 0;
	do
	{
		if (!xRank.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		PBConvert::ToNF(xRank.id(), self);

		const std::string& strListName = xRank.name();
		if (strListName.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
			QLOG_WARING << __FUNCTION__ << " rank list name empty";
			break;
		}

		auto it = m_mapRankList.find(strListName);
		if (it == m_mapRankList.end())
		{
			// 排行榜不存在
			nRetCode = OuterMsg::RANK_CODE_NOT_EXIST;
			break;
		}

		auto& list = *it->second;
		nRank = list.GetRank(self);

		// 不需要取属性数据
		if (xRank.no_data() > 0)
		{
			// 立刻回应消息
			bReply = true;
			break;
		}

		OuterMsg::RankListRankAck xRankAck;
		xRankAck.set_name(xRank.name());
		xRankAck.set_rank(nRank);
		*xRankAck.mutable_id() = xRank.id();
		*xRankAck.mutable_attach_data() = xRank.attach_data();
		xRankAck.set_ret_code(nRetCode);

		auto* pMapData = xRankAck.mutable_map_data();
		if (pMapData == nullptr)
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

		// 没有设定需要携带的数据，则携带排行榜上的设定的属性
		if (xRank.data_name_size() <= 0)
		{
			auto& vecDataName = list.GetDataNames();
			for (auto& it : vecDataName)
			{
				(*pMapData)[it] = "";
			}
		}
		else
		{
			for (int i = 0; i < xRank.data_name_size(); ++i)
			{
				(*pMapData)[xRank.data_name(i)] = "";
			}
		}

		std::string strRankAck;
		if (!xRankAck.SerializeToString(&strRankAck))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			break;
		}
		xMsgBase.set_data(std::move(strRankAck));

		// 获取数据库数据
		std::string strMsg;
		if (!xMsgBase.SerializeToString(&strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			break;
		}

		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_GET_RANK_DATA, strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

	} while (false);

	bReply = bReply || nRetCode != OuterMsg::RANK_CODE_SUCCEED;
	if (bReply)
	{
		// 设定回应数据
		OuterMsg::RankListRankAck reply;
		reply.set_name(xRank.name());
		reply.set_rank(nRank);
		*reply.mutable_id() = xRank.id();
		reply.set_ret_code(nRetCode);
		*reply.mutable_attach_data() = xRank.attach_data();
		std::string strReply;
		reply.SerializeToString(&strReply);
		xMsgBase.set_data(strReply);
	}
	else
	{
		xMsgBase.set_data("");
	}

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xRank.name()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}


bool RankModule::GetRankArray(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;
	OuterMsg::RankListRankArrayAck xRankArrayAck;
	NFGUID self;

	do
	{
		if (!xRankArrayAck.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		const std::string& strListName = xRankArrayAck.name();
		if (strListName.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
			QLOG_WARING << __FUNCTION__ << " rank list name empty";
			break;
		}

		auto it = m_mapRankList.find(strListName);
		if (it == m_mapRankList.end())
		{
			// 排行榜不存在
			nRetCode = OuterMsg::RANK_CODE_NOT_EXIST;
			break;
		}
		auto& list = *it->second;
		
		for (int i = 0; i < xRankArrayAck.ids_size(); ++i)
		{
			xRankArrayAck.add_ranks(list.GetRank(PBToNF(xRankArrayAck.ids(i))));
		}

	} while (false);


	// 设定回应数据
	xRankArrayAck.set_ret_code(nRetCode);
	std::string strReply;
	xRankArrayAck.SerializeToString(&strReply);
	xMsgBase.set_data(strReply);

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xRankArrayAck.name()
		<< " RetCode:" << nRetCode;

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::GetRange(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;

	// 是否立刻回应
	bool bReply = false;
	OuterMsg::RankListRange xRange;
	OuterMsg::RankListRangeAck xRangeAck;
	do
	{
		if (!xRange.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		xRangeAck.set_name(xRange.name());
		xRangeAck.set_start(xRange.start());
		xRangeAck.set_count(xRange.count());
		*xRangeAck.mutable_attach_data() = xRange.attach_data();

		const std::string& strListName = xRange.name();
		if (strListName.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
			QLOG_WARING << __FUNCTION__ << " rank list name empty";
			break;
		}

		auto it = m_mapRankList.find(strListName);
		if (it == m_mapRankList.end())
		{
			// 排行榜不存在
			nRetCode = OuterMsg::RANK_CODE_NOT_EXIST;
			break;
		}

		auto& list = *it->second;
		std::vector<NFGUID> vec_range;
		int nStart = xRange.start();
		int nEnd = nStart + xRange.count() - 1;

		// 取连续排名数据
		if (nStart > 0)
		{
			vec_range.clear();
			list.GetRange(xRange.start(), xRange.count(), vec_range);

			// 整理范围数据
			for (int i = 0; i < vec_range.size(); ++i)
			{
				auto* pData = xRangeAck.add_data();
				if (pData == nullptr)
				{
					continue;
				}

				PBConvert::ToPB(vec_range[i], *pData->mutable_id());
				pData->set_rank(xRange.start() + i);
			}
		}

		// 取不连续名次数据
		if (xRange.arr_rank_size() > 0)
		{
			for (int i = 0; i < xRange.arr_rank_size(); ++i)
			{
				// 范围限制
				if (xRangeAck.data_size() > MAX_EVERY_GET_RANGE)
				{
					break;
				}

				int nRank = xRange.arr_rank(i);
				if (nRank >= nStart && nRank <= nEnd)
				{
					// 已经包含在连续排名里面，不需要重复读取
					continue;
				}

				vec_range.clear();
				if (!list.GetRange(nRank, 1, vec_range))
				{
					continue;
				}

				auto* pData = xRangeAck.add_data();
				if (pData == nullptr)
				{
					continue;
				}

				PBConvert::ToPB(vec_range[0], *pData->mutable_id());
				pData->set_rank(nRank);
			}
		}

		xRangeAck.set_max_rank(list.GetMaxRank());

		// 没有获取到数据
		if (xRangeAck.data_size() <= 0)
		{
			bReply = true;
			break;
		}

		// 不需要携带数据
		if (xRange.no_data() > 0)
		{
			bReply = true;
			break;
		}

		auto* pData = xRangeAck.mutable_data(0);
		if (pData == nullptr)
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

		auto* pMapData = pData->mutable_map_data();
		if (pMapData == nullptr)
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

		// 没有设定需要携带的数据，则携带排行榜上的设定的属性
		if (xRange.data_name_size() <= 0)
		{
			auto& vecDataName = list.GetDataNames();
			for (auto& it : vecDataName)
			{
				(*pMapData)[it] = "";
			}
		}
		else
		{
			for (int i = 0; i < xRange.data_name_size(); ++i)
			{
				(*pMapData)[xRange.data_name(i)] = "";
			}
		}

		std::string strRangeAck;
		if (!xRangeAck.SerializeToString(&strRangeAck))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			break;
		}
		xMsgBase.set_data(std::move(strRangeAck));

		// 获取数据库数据
		std::string strMsg;
		if (!xMsgBase.SerializeToString(&strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_SERIALIZE;
			break;
		}

		if (!m_pNetClientModule->SendMsgByType(NF_SERVER_TYPES::NF_ST_RANK_DB, OuterMsg::SS_DB_RANK_LIST_GET_RANGE_DATA, strMsg))
		{
			nRetCode = OuterMsg::RANK_CODE_UNKNOW;
			break;
		}

	} while (false);

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xRangeAck.name()
		<< " start:" << xRange.start()
		<< " count:" << xRange.count()
		<< " get_count:" << xRangeAck.data_size()
		<< " RetCode:" << nRetCode;

	bReply = bReply || nRetCode != OuterMsg::RANK_CODE_SUCCEED;
	if (bReply)
	{
		xRangeAck.set_ret_code(nRetCode);
	}
	else
	{
		xMsgBase.set_data("");
	}

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::SwapRank(OuterMsg::ExtraMsg& xMsgBase)
{
	OuterMsg::RankListCode nRetCode = OuterMsg::RANK_CODE_SUCCEED;

	// 是否立刻回应
	bool bReply = false;
	OuterMsg::RankListSwapRankAck xSwapRankAck;
	do
	{
		if (!xSwapRankAck.ParseFromString(xMsgBase.data()))
		{
			nRetCode = OuterMsg::RANK_CODE_ERROR_PARSE;
			break;
		}

		const std::string& strListName = xSwapRankAck.name();
		if (strListName.empty())
		{
			nRetCode = OuterMsg::RANK_CODE_NAME_EMPTY;
			QLOG_WARING << __FUNCTION__ << " rank list name empty";
			break;
		}

		auto it = m_mapRankList.find(strListName);
		if (it == m_mapRankList.end())
		{
			// 排行榜不存在
			nRetCode = OuterMsg::RANK_CODE_NOT_EXIST;
			break;
		}
		auto& list = *it->second;

		// 获取分数
		NFGUID key1 = ::PBToNF(xSwapRankAck.key1());
		OuterMsg::RankListScore xScore1;
		if (!key1.IsNull())
		{
			if (!GetRankListScore(strListName, key1, xScore1))
			{
				if (!xScore1.name().empty())
				{
					SetScore(xScore1);
				}
			}
		}

		// 获取分数
		NFGUID key2 = ::PBToNF(xSwapRankAck.key2());
		OuterMsg::RankListScore xScore2;
		if (!key2.IsNull())
		{
			if (!GetRankListScore(strListName, key2, xScore2))
			{
				if (!xScore2.name().empty())
				{
					SetScore(xScore2);
				}
			}
		}

		// 交换分数
		if (!xScore1.name().empty() && !xScore2.name().empty())
		{
			*xScore1.mutable_id() = xSwapRankAck.key2();
			SetScore(xScore1);

			*xScore2.mutable_id() = xSwapRankAck.key1();
			SetScore(xScore2);
		}

		// 获取最新名次
		if (!key1.IsNull())
		{
			xSwapRankAck.set_rank1(list.GetRank(key1));
		}
		if (!key2.IsNull())
		{
			xSwapRankAck.set_rank2(list.GetRank(key2));
		}
		
	} while (false);

	// 记录
	QLOG_INFO_S << "MsgID:" << xMsgBase.msg_id()
		<< " RankList: " << xSwapRankAck.name()
		<< " RetCode:" << nRetCode;

	xSwapRankAck.set_ret_code(nRetCode);
	std::string strReply;
	xSwapRankAck.SerializeToString(&strReply);
	xMsgBase.set_data(strReply);

	return nRetCode == OuterMsg::RANK_CODE_SUCCEED;
}

bool RankModule::GetRankListScore(const std::string& strListName, const NFGUID& key, OuterMsg::RankListScore& xScore)
{
	auto it = m_mapRankList.find(strListName);
	if (it == m_mapRankList.end())
	{
		return false;
	}
	auto& list = *it->second;

	xScore.set_name(strListName);
	*xScore.mutable_id() = NFToPB(key);
	auto& vec_score_name = list.GetScoreNames();

	std::vector<int64_t> vec;
	int64_t insert_time = 0;
	if (!list.GetScores(key, vec, insert_time))
	{
		for (int i = 0; i < vec_score_name.size(); ++i)
		{
			auto* pData = xScore.add_arr_data();
			if (pData == nullptr)
			{
				QLOG_ERROR << __FUNCTION__ << " add_arr_data failed";
				continue;
			}

			pData->set_name(vec_score_name[i]);
			pData->set_data(0);
		}

		// 时间戳在最后一名基础上加1，保证时间戳的排序在最后
		vec.clear();
		insert_time = 0;
		list.GetTailScores(vec, insert_time);
		xScore.set_insert_time(++insert_time);

		return false;
	}

	xScore.set_insert_time(insert_time);
	for (int i = 0; i < vec.size(); ++i)
	{
		auto* pData = xScore.add_arr_data();
		if (pData == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " add_arr_data failed";
			continue;
		}

		if (i < vec_score_name.size())
		{
			pData->set_name(vec_score_name[i]);
		}
		pData->set_data(vec[i]);
	}

	return true;
}

bool RankModule::OnHttpGetData(const NFHttpRequest& req)
{
	if (m_state != RANK_STATE_INIT_FINISH)
	{
		QLOG_WARING << __FUNCTION__ << " rank list initing";
		return false;
	}

	OuterMsg::ExtraMsg xMsgBase;
	xMsgBase.set_msg_id(OuterMsg::MSG_RANK_GET_DATA);
	xMsgBase.set_data(req.body);
	xMsgBase.set_socket(req.id);
	xMsgBase.set_msg_type(MsgType::MSG_HTTP_CLIENT);

	GetData(xMsgBase);
	if (!xMsgBase.data().empty())
	{
		SendAckMessage(NULL_STR, xMsgBase);
	}

	return true;
}

bool RankModule::OnHttpGetDataArray(const NFHttpRequest& req)
{
	if (m_state != RANK_STATE_INIT_FINISH)
	{
		QLOG_WARING << __FUNCTION__ << " rank list initing";
		return false;
	}

	OuterMsg::ExtraMsg xMsgBase;
	xMsgBase.set_msg_id(OuterMsg::MSG_RANK_GET_DATA_ARRAY);
	xMsgBase.set_data(req.body);
	xMsgBase.set_socket(req.id);
	xMsgBase.set_msg_type(MsgType::MSG_HTTP_CLIENT);

	GetDataArray(xMsgBase);
	if (!xMsgBase.data().empty())
	{
		SendAckMessage(NULL_STR, xMsgBase);
	}

	return true;
}

bool RankModule::OnHttpGetRank(const NFHttpRequest& req)
{
	if (m_state != RANK_STATE_INIT_FINISH)
	{
		QLOG_WARING << __FUNCTION__ << " rank list initing";
		return false;
	}

	OuterMsg::ExtraMsg xMsgBase;
	xMsgBase.set_msg_id(OuterMsg::MSG_RANK_GET_RANK);
	xMsgBase.set_data(req.body);
	xMsgBase.set_socket(req.id);
	xMsgBase.set_msg_type(MsgType::MSG_HTTP_CLIENT);

	GetRank(xMsgBase);
	if (!xMsgBase.data().empty())
	{
		SendAckMessage(NULL_STR, xMsgBase);
	}

	return true;
}

bool RankModule::OnHttpGetRange(const NFHttpRequest& req)
{
	if (m_state != RANK_STATE_INIT_FINISH)
	{
		QLOG_WARING << __FUNCTION__ << " rank list initing";
		return false;
	}

	OuterMsg::ExtraMsg xMsgBase;
	xMsgBase.set_msg_id(OuterMsg::MSG_RANK_GET_RANGE);
	xMsgBase.set_data(req.body);
	xMsgBase.set_socket(req.id);
	xMsgBase.set_msg_type(MsgType::MSG_HTTP_CLIENT);

	GetRange(xMsgBase);
	if (!xMsgBase.data().empty())
	{
		SendAckMessage(NULL_STR, xMsgBase);
	}

	return true;
}

void RankModule::ConnectServer(const ConnectCfg& connect_data)
{
	mxConnectList.push_back(connect_data);
}

void RankModule::CreateServer(const NetData& net_data)
{
	mxNetList.push_back(net_data);
}

const std::string& RankModule::GetTypeIP(ENUM_NET_TYPE type)
{
	for (auto& it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetIP();
		}
	}

	return NULL_STR;
}

int RankModule::GetTypePort(ENUM_NET_TYPE type)
{
	for (auto& it : mxNetList)
	{
		if (type == it.GetNetType())
		{
			return it.GetPort();
		}
	}

	return 0;
}

void RankModule::OnMaintaninMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	std::vector<std::string> vec = StringUtil::SplitString(std::string(msg, nLen), " ");
	if (vec.empty())
	{
		return;
	}

	Pb2Json::Json json;

	const std::string& strCmd = vec[0];
	if (strCmd == "ranklist_get_all_list")
	{
		// 获取所有排行榜信息
		// 参数 0:"ranklist_get_all_list"
		// 结果 结构{"ret_code":错误码(0成功), "data":[{"name":"排行榜名", "data_names":["绑定的数据名", ...], "score_names":["分数名", ...]}, ... ]}
		json["ret_code"] = OuterMsg::RANK_CODE_SUCCEED;
		for (auto& it : m_mapRankList)
		{
			auto pList = it.second;
			if (pList == nullptr)
			{
				continue;
			}

			Pb2Json::Json json_list;
			json_list["name"] = pList->GetName();
			json_list["limit_rank"] = pList->GetLimitRank();
			json_list["max_rank"] = pList->GetMaxRank();

			Pb2Json::Json json_data_name;
			auto datanames = pList->GetDataNames();
			for (auto& it : datanames)
			{
				json_data_name.push_back(it);
			}
			json_list["data_names"] = json_data_name;

			Pb2Json::Json json_score_name;
			auto scorenames = pList->GetScoreNames();
			for (auto& it : scorenames)
			{
				json_score_name.push_back(it);
			}
			json_list["score_names"] = json_score_name;

			json["data"].push_back(json_list);
		}
	}
	else if (strCmd == "ranklist_get_range")
	{
		if (vec.size() > 3)
		{
			// 获取排行榜范围排名
			// 参数 0:"ranklist_get_range" 1:排行榜名 2:起始名次 3:数量(最大100)
			// 结果 结构{"ret_code":错误码(0成功), "data":{"name":"排行榜名", "start":起始名次, "count":数量, "data":[{"id":{"data1":键值, "data2":键值}, "rank":名次, "map_data":[{"key":属性名, "value":属性值}, ...]}, ...]}
			OuterMsg::RankListRange xRange;
			xRange.set_name(vec[1]);
			xRange.set_start(::lexical_cast<int>(vec[2]));
			xRange.set_count(::lexical_cast<int>(vec[3]));

			OuterMsg::ExtraMsg xMsgBase;
			xMsgBase.set_msg_id(OuterMsg::MSG_RANK_GET_RANGE);
			xMsgBase.set_data(xRange.SerializeAsString());
			xMsgBase.set_socket(nSockIndex);
			xMsgBase.set_msg_type(MsgType::MSG_MAINTAIN);

			GetRange(xMsgBase);
			if (!xMsgBase.data().empty())
			{
				OuterMsg::RankListRangeAck xRange;
				xRange.ParseFromString(xMsgBase.data());
				Pb2Json::Message2Json(xRange, json);
			}
		}
		else
		{
			json["ret_code"] = OuterMsg::RANK_CODE_PARAM_ERROR;
		}
	}
	else if (strCmd == "ranklist_get_score_range")
	{
		if (vec.size() > 3)
		{
			// 获取排行榜范围排名
			// 参数 0:"ranklist_get_score_range" 1:排行榜名 2:起始名次 3:数量(最大100)
			auto it = m_mapRankList.find(vec[1]);
			if (it != m_mapRankList.end())
			{
				auto list = it->second;
				int nStart = ::lexical_cast<int>(vec[2]);
				int nCount = ::lexical_cast<int>(vec[3]);

				std::vector<NFGUID> vec_range;
				std::vector<int64_t> vec_score;
				list->GetRange(nStart, nCount, vec_range);
				Pb2Json::Json data;
				for (int i = 0; i < vec_range.size(); ++i)
				{
					vec_score.clear();
					int64_t insert_time = 0;
					if (!list->GetScores(vec_range[i], vec_score, insert_time))
					{
						continue;
					}

					Pb2Json::Json score_data;
					Pb2Json::Json scores;
					score_data["rank"] = nStart + i;
					score_data["key"] = vec_range[i].ToString();
					for (auto it : vec_score)
					{
						scores.push_back(it);
					}
					score_data["scores"] = scores;
					score_data["insert_time"] = insert_time;
					data.push_back(score_data);	
				}
				json["data"] = data;
			}
			else
			{
				json["ret_code"] = OuterMsg::RANK_CODE_NOT_EXIST;
			}
		}
		else
		{
			json["ret_code"] = OuterMsg::RANK_CODE_PARAM_ERROR;
		}
	}
	else
	{
		json["ret_code"] = OuterMsg::RANK_CODE_UNKNOW;
	}

	if (!json.empty())
	{
		m_pMaintainNetServer->SendMsg(OuterMsg::RUN_NET_COMMAND, json.dump(), nSockIndex);
	}
}