///--------------------------------------------------------------------
/// 文件名:		ExtraServerModule.cpp
/// 内  容:		附加服务器
/// 说  明:		
/// 创建日期:	2019年8月20日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "ExtraServerModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFPluginModule/KConstDefine.h"

bool CExtraServerModule::Init()
{
	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pNetServerInside = pPluginManager->FindModule<NetServerInsideModule>();
	
	return true;
}

bool CExtraServerModule::AfterInit()
{
	for (int i = NF_ST_NONE + 1; i < NF_ST_MAX; ++i)
	{
		m_pNetClientModule->AddReceiveCallBack(NF_SERVER_TYPES(i), 
			OuterMsg::SS_ACK_EXTRA_SERVER, 
			this, 
			(NetMsgFun)&CExtraServerModule::OnExtraMsg);
	}

	m_pNetServerInside->AddReceiveCallBack(OuterMsg::SS_ACK_EXTRA_SERVER, this, (NetMsgFun)&CExtraServerModule::OnExtraMsg);
	
	return true;
}

//接收到世界服转发来
void CExtraServerModule::OnExtraMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	OuterMsg::ExtraMsg xMsg;
	if (!xMsg.ParseFromArray(msg, nLen))
	{
		QLOG_ERROR << __FUNCTION__ << " ExtraMsg ParseFromArray failed";
		return;
	}

	NFGUID self;
	if (!PBConvert::ToNF(xMsg.object_id(), self))
	{
		QLOG_WARING << __FUNCTION__ << " PB to NFGUID failed";
		return;
	}

	if (!self.IsNull())
	{
		NFObject* pObj = dynamic_cast<NFObject*>(m_pKernelModule->GetObject(self).get());
		if (pObj == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " not find object:" << self;
			return;
		}

		pObj->OnExtra(static_cast<NF_SERVER_TYPES>(xMsg.server_type()), xMsg.msg_id(), self, xMsg.data());
	}
	else
	{
		m_pClassModule->RunExtraCallBack(
			pPluginManager->GetAppName(),
			static_cast<NF_SERVER_TYPES>(xMsg.server_type()), 
			xMsg.msg_id(), 
			self, 
			xMsg.data());
	}
}