///--------------------------------------------------------------------
/// 文件名:		NFPlayer.cpp
/// 内  容:		玩家
/// 说  明:		
/// 创建日期:	2019年7月30日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFPlayer.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"

NFPlayer::NFPlayer(NFGUID self, NFIPluginManager* pPluginManager) : NFVisible(self, pPluginManager)
{
	// 添加内部属性
	ADD_RAW_STR(m_pPropertyManager, Name, EDIT_NO, true);
	ADD_RAW_STR(m_pPropertyManager, Account, EDIT_NO, true);
	ADD_RAW_STR(m_pPropertyManager, IP, EDIT_NO, false);
	ADD_RAW_INT(m_pPropertyManager, GameID, EDIT_NO, false);
	ADD_RAW_INT(m_pPropertyManager, GateID, EDIT_NO, false);
	ADD_RAW_OBJECT(m_pPropertyManager, ClientID, EDIT_NO, false);
	ADD_RAW_INT(m_pPropertyManager, CreateTime, EDIT_NO, true);
	ADD_RAW_INT(m_pPropertyManager, OpenTime, EDIT_NO, true);
	ADD_RAW_INT(m_pPropertyManager, TotalSec, EDIT_NO, true);
	ADD_RAW_INT(m_pPropertyManager, OnlineTime, EDIT_NO, false);
	ADD_RAW_INT(m_pPropertyManager, OfflineTime, EDIT_NO, true);
	ADD_RAW_STR(m_pPropertyManager, LiteData, EDIT_OK, true);
	ADD_RAW_STR(m_pPropertyManager, DeviceID, EDIT_NO, false); 
	ADD_RAW_INT(m_pPropertyManager, OsType, EDIT_NO, false);
	ADD_RAW_INT(m_pPropertyManager, OnlineState, EDIT_NO, false);
}

NFPlayer::~NFPlayer()
{
	m_vecViewport.clear();
}

bool NFPlayer::AddViewport(int id, NFVisible* pContainer)
{
	if (id < 0 || id > VIEWPORT_MAX_NUM)
	{
		QLOG_ERROR << __FUNCTION__ << " out range id:" << id;
		return false;
	}

	if (id < static_cast<int>(m_vecViewport.size()))
	{
		if (m_vecViewport[id] != nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " already exist id:" << id;
			return false;
		}
	}
	else
	{
		m_vecViewport.resize(id + 1, nullptr);
	}

	m_vecViewport[id] = NF_SHARE_PTR<NFViewport>(NF_NEW NFViewport(this, id, pContainer, m_pPluginManager));

	return true;
}

bool NFPlayer::RemoveViewport(int id)
{
	if (id < 0 || id >= static_cast<int>(m_vecViewport.size()))
	{
		QLOG_ERROR << __FUNCTION__ << " out range id:" << id 
			<< " invaild size:" << m_vecViewport.size();
		return false;
	}

	m_vecViewport[id] = nullptr;

	return true;
}

bool NFPlayer::FindViewport(int id)
{
	if (id < 0 || id >= static_cast<int>(m_vecViewport.size()))
	{
		return false;
	}

	return m_vecViewport[id] != nullptr;
}

void NFPlayer::ClearViewport()
{
	m_vecViewport.clear();
}

NFVisible* NFPlayer::GetViewContainer(int id)
{
	if (id < 0 || id >= static_cast<int>(m_vecViewport.size()))
	{
		QLOG_WARING << __FUNCTION__ << " out range id:" << id
			<< " invaild size:" << m_vecViewport.size();
		return nullptr;
	}

	if (m_vecViewport[id] == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " m_vecViewport == NULL id:" << id;
		return nullptr;
	}

	return m_vecViewport[id]->GetContainer();
}

void NFPlayer::ResendViewports()
{
	for (auto& it : m_vecViewport)
	{
		it->NotifyReset();
	}
}

int NFPlayer::OnCustom(int nMsgId, const std::string& strMsg)
{
	if (m_pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " m_pClass == NULL";
		return 0;
	}

	return m_pClass->RunCustomCallBack(nMsgId, Self(), strMsg);
}
