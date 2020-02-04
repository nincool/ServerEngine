///--------------------------------------------------------------------
/// 文件名:		MaintainModule.cpp
/// 内  容:		维护模块
/// 说  明:		
/// 创建日期:	2019年10月14日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "MaintainModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/StringUtil.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFUtils/pb_json/pb2json.hpp"
#include <fstream>


MaintainModule::MaintainModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

MaintainModule::~MaintainModule()
{

}

bool MaintainModule::Init()
{
	m_pMaintainNetServer = pPluginManager->FindModule<NetServerMaintainModule>();
	m_pKernelModule = (NFKernelModule*)pPluginManager->FindModule<NFIKernelModule>();
	m_pHeartBeatModule = pPluginManager->FindModule<NFIHeartBeatModule>();
	m_pGameKernel = (CGameKernel*)pPluginManager->FindModule<IGameKernel>();

	return true;
}

bool MaintainModule::AfterInit()
{
	//取得对象列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjList);
	//取得对象属性列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_PROP_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjPropList);
	//取得对象表格列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_REC_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjRecList);
	//取得对象容器列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_CON_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjConList);
	//取得对象表格内容
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_REC, this, (NetMsgFun)&MaintainModule::OnGetObjRec);
	//取得对象定时器列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_BEAT_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjBeatList);
	//取得对象定时器内容
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_BEAT, this, (NetMsgFun)&MaintainModule::OnGetObjBeat);
	//取得对象属性回调列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_PROP_CRITICAL_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjPropCriticalList);
	//取得对象表格回调列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_OBJ_REC_CALLBACK_LIST, this, (NetMsgFun)&MaintainModule::OnGetObjRecCallbackList);
	//取得玩家GUID Name列表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::GET_PLAYER_GN_LIST, this, (NetMsgFun)&MaintainModule::OnGetPlayerGNList);

	//修改对象属性
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::SET_OBJ_PROP, this, (NetMsgFun)&MaintainModule::OnSetObjProp);
	//修改对象表
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::SET_OBJ_REC, this, (NetMsgFun)&MaintainModule::OnSetObjRec);
	//修改对容器
	m_pMaintainNetServer->AddReceiveCallBack(OuterMsg::SET_OBJ_CON, this, (NetMsgFun)&MaintainModule::OnSetObjCon);

	//执行命令
	m_pMaintainNetServer->AddCommandCallBack("export_obj_list", this, (MaintainCBFun)&MaintainModule::ExportOBJList);
	m_pMaintainNetServer->AddCommandCallBack("export_obj", this, (MaintainCBFun)&MaintainModule::ExportOBJ);
	
	return true;
}

//取得对象列表
bool MaintainModule::GetObjList(const REQGetObjList& req, ACKGetOBJList& ack)
{
	if (req.u == "")
	{
		auto objAll = m_pKernelModule->GetObjectAll();
		for (auto& it_map : objAll)
		{
			auto& it = it_map.second;
			if (it == nullptr)
			{
				continue;
			}

			if (it->Type() == req.t)
			{
				ACKGetOBJ obj;
				obj.u = it->Self().ToString();
				obj.n = it->GetName();
				obj.c = it->ClassName();
				obj.t = it->Type();
				obj.cf = it->Config();
				ack.l.push_back(obj);
			}
		}
	}
	else
	{
		NFGUID uid(req.u);
		do
		{
			NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
			if (nullptr == pSObj)
			{
				break;
			}

			const std::vector<NFGUID>& childList = m_pKernelModule->GetChildList(uid);
			for (int i = 0; i < childList.size(); ++i)
			{
				NF_SHARE_PTR<NFIObject> pChild = m_pKernelModule->GetObject(childList[i]);
				if (nullptr == pChild)
				{
					continue;
				}

				ACKGetOBJ obj;
				obj.u = pChild->Self().ToString();
				obj.n = pChild->GetName();
				obj.c = pChild->ClassName();
				obj.t = pChild->Type();
				obj.cf = pChild->Config();
				ack.l.push_back(obj);
			}

		} while (false);

	}

	return true;
}

//取得对象属性列表
bool MaintainModule::GetObjPropList(const REQSelectObj& req, ACKGetObjPropList& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIPropertyManager> pPropM = pObj->GetPropertyManager();
	if (nullptr == pPropM)
	{
		QLOG_WARING << __FUNCTION__ << " pPropM == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIProperty> pProp = pPropM->First();
	while (pProp)
	{
		JProp jp;
		jp.n = pProp->GetName();
		jp.t = pProp->GetType();
		jp.v = pProp->ToString();
		jp.pr = pProp->GetPrivate();
		jp.pu = pProp->GetPublic();
		jp.s = pProp->GetSave();
		ack.p.push_back(jp);

		pProp = pPropM->Next();
	}

	return true;
}

//取得对象表格列表
bool MaintainModule::GetObjRecList(const REQSelectObj& req, ACKGetObjRecList& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIRecordManager> pRecM = pObj->GetRecordManager();
	if (nullptr == pRecM)
	{
		QLOG_WARING << __FUNCTION__ << " pRecM == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIRecord> pRec = pRecM->First();
	while (pRec)
	{
		ack.l.push_back(pRec->GetName());

		if (pRec->GetPrivate())
		{
			ack.pr_cout += 1;
		}

		if (pRec->GetSave())
		{
			ack.s_count += 1;
		}

		pRec = pRecM->Next();
	}

	return true;
}

//取得对象容器列表
bool MaintainModule::GetObjConList(const REQSelectObj& req, ACKGetOBJList& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	const std::vector<NFGUID>& childList = pObj->GetChildList();
	for (auto u : childList)
	{
		ACKGetOBJ obj;
		obj.u = u.ToString();
		NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(u);
		if (nullptr != pSObj)
		{
			obj.n = pSObj->GetName();
			obj.t = pSObj->Type();
		}
		ack.l.push_back(obj);
	}

	return true;
}

//取得对象表格内容
bool MaintainModule::GetObjRec(const J_SelectObjRec& req, J_ObjRecData& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIRecord> pRec = pObj->GetRecord(req.r);
	if (nullptr == pRec)
	{
		QLOG_WARING << __FUNCTION__ << " record not find:" << uid
			<< " name:" << req.r;
		return false;
	}

	ack.pr = pRec->GetPrivate();
	ack.pu = pRec->GetPublic();
	ack.s = pRec->GetSave();
	ack.col = pRec->GetCols();
	ack.row = pRec->GetRows();
	ack.rowMax = pRec->GetMaxRows();

	std::vector<std::string> rowData_vec;
	NFDataList rowData;
	int nRoes = pRec->GetRows();
	int nCols = pRec->GetCols();
	for (int i = 0; i < nRoes; ++i)
	{
		rowData.Clear();
		rowData_vec.clear();
		rowData = pRec->GetRowValue(i);
		rowData_vec.push_back(to_string(i));
		for (int c = 0; c < nCols; ++c)
		{
			auto& data = rowData.GetData(c);
			rowData_vec.push_back(data.ToString());
		}
		ack.data.push_back(rowData_vec);
	}

	return true;
}

//取得对象定时器列表
bool MaintainModule::GetObjBeatList(const REQSelectObj& req, ACKGetObjBeatList& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	list<beat_data_t*> listBeatInfo;
	m_pHeartBeatModule->GetHeartBeatList(uid, listBeatInfo);

	list<beat_data_t*>::iterator iterBeat = listBeatInfo.begin();
	while (iterBeat != listBeatInfo.end())
	{
		ack.l.push_back((*iterBeat)->strName);
		++iterBeat;
	}

	return true;
}

//取得对象定时器内容
bool MaintainModule::GetObjBeat(const J_SelectObjBeat& req, J_ObjBeatData& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	beat_data_t* pFind = m_pHeartBeatModule->GetHeartBeat(uid, req.h.c_str());
	if (nullptr == pFind)
	{
		QLOG_WARING << __FUNCTION__ << " heart beat not find:" << uid
			<< " name:" << req.h;
		return false;
	}

	ack.slot = pFind->nSlot;
	ack.del = pFind->bDelete;
	ack.time = pFind->nTime;
	ack.mc = pFind->nMax;
	ack.ct = pFind->nCount;

	return true;
}

//取得对象属性回调列表
bool MaintainModule::GetObjPropCriticalList(const REQSelectObj& req, ACKGetObjPropCriticalList& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIPropertyManager> pPropM = pObj->GetPropertyManager();
	if (nullptr == pPropM)
	{
		QLOG_WARING << __FUNCTION__ << " pPropM == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIProperty> pProp = pPropM->First();
	while (pProp)
	{
		if (pProp->GetTCallBackCount() > 0)
		{
			JPropCritical jp;
			jp.n = pProp->GetName();
			jp.c = pProp->GetTCallBackCount();
			ack.p.push_back(jp);
		}

		pProp = pPropM->Next();
	}

	return true;
}

//取得对象表格回调列表
bool MaintainModule::GetObjRecCallBackList(const REQSelectObj& req, ACKGetObjRecCallBackList& ack)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NFObject* pObj = (NFObject*)pSObj.get();
	if (nullptr == pObj)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIRecordManager> pRecM = pObj->GetRecordManager();
	if (nullptr == pRecM)
	{
		QLOG_WARING << __FUNCTION__ << " pRecM == NULL:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIRecord> pRec = pRecM->First();
	while (pRec)
	{
		if (pRec->GetTCallBackCount() > 0)
		{
			JRecCallBack jp;
			jp.n = pRec->GetName();
			jp.c = pRec->GetTCallBackCount();
			ack.p.push_back(jp);
		}

		pRec = pRecM->Next();
	}

	return true;
}

//修改对象属性
bool MaintainModule::SetObjProp(const J_ModifyObjProp& req)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	switch (pSObj->GetPropertyType(req.p))
	{
	case TDATA_INT:
		pSObj->SetPropertyInt(req.p, lexical_cast<int64_t>(req.v));
		break;
	case TDATA_FLOAT:
		pSObj->SetPropertyFloat(req.p, lexical_cast<double>(req.v));
		break;
	case TDATA_STRING:
		pSObj->SetPropertyString(req.p, req.v);
		break;
	case TDATA_OBJECT:
		pSObj->SetPropertyObject(req.p, NFGUID(req.v));
		break;
	}

	return true;
}

//修改对象表
bool MaintainModule::SetObjRec(J_ModifyObjRec& req)
{
	NFGUID uid(req.u);
	NF_SHARE_PTR<NFIObject> pSObj = m_pKernelModule->GetObject(uid);
	if (nullptr == pSObj)
	{
		QLOG_WARING << __FUNCTION__ << " Obj not exist:" << uid;
		return false;
	}

	NF_SHARE_PTR<NFIRecord> pRec = pSObj->GetRecord(req.r);
	if (nullptr == pRec)
	{
		QLOG_WARING << __FUNCTION__ << " record not find:" << uid
			<< " name:" << req.r;
		return false;
	}

	if (req.data.size() == pRec->GetCols())
	{
		if (-1 == req.row)
		{
			//增加
			req.row = pRec->AddRow(-1, NULL_DATA_LIST);
			if (-1 == req.row)
			{
				return false;
			}
		}

		for (int i = 0; i < pRec->GetCols(); ++i)
		{
			switch (pRec->GetColType(i))
			{
			case TDATA_INT:
				pRec->SetInt(req.row, i, lexical_cast<int64_t>(req.data[i]));
				break;
			case TDATA_FLOAT:
				pRec->SetFloat(req.row, i, lexical_cast<double>(req.data[i]));
				break;
			case TDATA_STRING:
				pRec->SetString(req.row, i, req.data[i]);
				break;
			case TDATA_OBJECT:
				pRec->SetObject(req.row, i, NFGUID(req.data[i]));
				break;
			}
		}
	}
	else if (req.data.size() == 0)	//删除
	{
		if (-1 == req.row)
		{
			pRec->Clear();
		}
		else
		{
			pRec->Remove(req.row);
		}
	}

	return true;
}

//取得对象列表
void MaintainModule::OnGetObjList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	REQGetObjList req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetOBJList ack;
	ack.p = req.u;

	GetObjList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象属性列表
void MaintainModule::OnGetObjPropList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	REQSelectObj req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetObjPropList ack;
	ack.u = req.u;

	GetObjPropList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象表格列表
void MaintainModule::OnGetObjRecList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	REQSelectObj req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetObjRecList ack;
	ack.u = req.u;

	GetObjRecList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象容器列表
void MaintainModule::OnGetObjConList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	REQSelectObj req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetOBJList ack;
	ack.p = req.u;

	GetObjConList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象表格内容
void MaintainModule::OnGetObjRec(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	J_SelectObjRec req;
	ajson::load_from_buff(req, msg, nLen);

	J_ObjRecData ack;
	ack.u = req.u;
	ack.r = req.r;

	GetObjRec(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象定时器列表
void MaintainModule::OnGetObjBeatList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	REQSelectObj req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetObjBeatList ack;
	ack.u = req.u;

	GetObjBeatList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象定时器内容
void MaintainModule::OnGetObjBeat(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	J_SelectObjBeat req;
	ajson::load_from_buff(req, msg, nLen);

	J_ObjBeatData ack;
	ack.u = req.u;
	ack.h = req.h;

	GetObjBeat(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象属性回调列表
void MaintainModule::OnGetObjPropCriticalList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	REQSelectObj req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetObjPropCriticalList ack;
	ack.u = req.u;

	GetObjPropCriticalList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//取得对象表格回调列表
void MaintainModule::OnGetObjRecCallbackList(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	REQSelectObj req;
	ajson::load_from_buff(req, msg, nLen);

	ACKGetObjRecCallBackList ack;
	ack.u = req.u;

	GetObjRecCallBackList(req, ack);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//修改对象属性
void MaintainModule::OnSetObjProp(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	J_ModifyObjProp req;
	ajson::load_from_buff(req, msg, nLen);

	SetObjProp(req);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, req);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//修改对象表
void MaintainModule::OnSetObjRec(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	J_ModifyObjRec req;
	ajson::load_from_buff(req, msg, nLen);

	SetObjRec(req);

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, req);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

//修改对容器
void MaintainModule::OnSetObjCon(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	J_ModifyChild req;
	ajson::load_from_buff(req, msg, nLen);

	if (!req.remove.empty())
	{
		m_pKernelModule->DestroySelf(NFGUID(req.remove));
	}
}

//取得玩家GUID Name列表
void MaintainModule::OnGetPlayerGNList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
	const uint32_t nLen)
{
	ACKPlayerGNList ack;
	auto objAll = m_pGameKernel->GetRoleGuids();
	for (auto& it_map : objAll)
	{
		ACKPlayerGN obj;
		obj.u = it_map.first.ToString();
		obj.n = it_map.second;
		ack.l.push_back(obj);
	}

	ajson::string_stream ssAck;
	ajson::save_to(ssAck, ack);
	m_pMaintainNetServer->SendMsg(nMsgID, ssAck.str(), nSockIndex);
}

std::string MaintainModule::ExportOBJList(std::vector<std::string>& arg)
{
	std::ofstream outFile;
	if (arg.size() < 2)
	{
		outFile.open("ExportOBJList.txt", std::ios::trunc);
	}
	else
	{
		outFile.open(arg[1], std::ios::trunc);
	}

	auto objAll = m_pKernelModule->GetObjectAll();
	for (auto& it_map : objAll)
	{
		auto& it = it_map.second;
		if (it == nullptr)
		{
			continue;
		}

		if (!it->Parent().IsNull())
		{
			continue;
		}

		outFile << "guid:" << it->Self().ToString() << ", "
			<< "name:" << it->GetName() << ", "
			<< "class:" << it->ClassName() << ", "
			<< "config:" << it->Config() << ", "
			<< "type:" << it->Type() << "\n";
	}
	outFile.close();

	return "out file ok.";
}

std::string MaintainModule::ExportOBJ(std::vector<std::string>& arg)
{
	if (arg.size() < 2)
	{
		return "Parameter error.";
	}

	NFGUID guid = m_pGameKernel->SeekRoleGuid(arg[1]);
	if (guid.IsNull())
	{
		guid = arg[1];
	}

	OuterMsg::ObjectDataPack xObjPack;
	if (PBConvert::ToPBObject(m_pKernelModule, guid, &xObjPack))
	{
		std::ofstream outFile;
		if (arg.size() == 3)
		{
			outFile.open(arg[2], std::ios::trunc);
		}
		else
		{
			outFile.open("ExportOBJ.txt", std::ios::trunc);
		}

		nlohmann::json jObj;
		Pb2Json::Message2Json(xObjPack, jObj);

		outFile << jObj.dump();
		outFile.close();
		return "out file ok.";
	}

	return "can't find.";
}