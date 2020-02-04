///--------------------------------------------------------------------
/// 文件名:		NFKernelContainer.cpp
/// 内  容:		容器相关操作
/// 说  明:		
/// 创建日期:	2019年8月9日
/// 修改人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFKernelModule.h"
#include "NFComm/NFCore/NFPlayer.h"

bool NFKernelModule::AddViewport(const NFGUID& player, int id, const NFGUID& container)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(player));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not player";
		return false;
	}

	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container";
		return false;
	}

	return pPlayer->AddViewport(id, pContainer.get());
}

bool NFKernelModule::RemoveViewport(const NFGUID& player, int id)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(player));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find player";
		return false;
	}

	return pPlayer->RemoveViewport(id);
}

bool NFKernelModule::FindViewport(const NFGUID& player, int id)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(player));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not player";
		return false;
	}

	return pPlayer->FindViewport(id);
}

const NFGUID& NFKernelModule::GetViewportContainer(const NFGUID& player, int id)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(player));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find player" << player;
		return NULL_OBJECT;
	}

	NFVisible* pContainer = pPlayer->GetViewContainer(id);
	if (pContainer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find container id: " << id;
		return NULL_OBJECT;
	}

	return pContainer->Self();
}

bool NFKernelModule::ClearViewport(const NFGUID& player)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(player));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not player";
		return false;
	}

	pPlayer->ClearViewport();

	return true;
}


int NFKernelModule::GetViewerCount(const NFGUID& container)
{
	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container guid:" << container;
		return 0;
	}

	return pContainer->GetViewCount();
}

bool NFKernelModule::CloseViewers(const NFGUID& container)
{
	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container guid:" << container;
		return false;
	}

	pContainer->CloseViewers();

	return true;
}

bool NFKernelModule::Place(const NFGUID& obj, const NFGUID& container)
{
	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container:" << container;
		return false;
	}

	return pContainer->AddChild(obj, -1, true);
}

bool NFKernelModule::PlaceIndex(const NFGUID& obj, const NFGUID& container, int index)
{
	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container guid:" << container;
		return false;
	}

	return pContainer->AddChild(obj, index, true);
}

bool NFKernelModule::Exchange(const NFGUID& container1, int index1, const NFGUID& container2, int index2)
{
	NF_SHARE_PTR<NFVisible> pContainer1 = dynamic_pointer_cast<NFVisible>(GetObject(container1));
	if (pContainer1 == nullptr || !pContainer1->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container guid:" << container1;
		return false;
	}

	NF_SHARE_PTR<NFVisible> pContainer2 = dynamic_pointer_cast<NFVisible>(GetObject(container2));
	if (pContainer2 == nullptr || !pContainer2->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container guid:" << container2;
		return false;
	}

	if (!pContainer1->IsVaildIndex(index1))
	{
		QLOG_WARING << __FUNCTION__ << " container:" << pContainer1->GetName()
			<< "index:" << index1 << " error";
		return false;
	}

	if (!pContainer2->IsVaildIndex(index2))
	{
		QLOG_WARING << __FUNCTION__ << " container:" << pContainer2->GetName()
			<< "index:" << index2 << " error";
		return false;
	}

	NF_SHARE_PTR<NFIObject> pObj1 = pContainer1->GetChildObjByIndex(index1);
	NF_SHARE_PTR<NFIObject> pObj2 = pContainer1->GetChildObjByIndex(index2);
	if (pObj1 == nullptr && pObj2 == nullptr)
	{
		std::ostringstream str;
		QLOG_WARING << __FUNCTION__ << " container1:" << pContainer1->GetName()
			<< " index1:" << index1 << " item not exist and "
			<< " container2:" << pContainer2->GetName() 
			<< " index2:" << index2 << " item not exist";
		return false;
	}

	// 是同一个容器
	if (pContainer1 == pContainer2)
	{
		if (pObj1 != nullptr)
		{
			return pContainer1->ChangeChild(pObj1->Self(), index2);
		}
		else if (pObj2 != nullptr)
		{
			return pContainer1->ChangeChild(pObj2->Self(), index1);
		}
	}

	if (pObj1 != nullptr && pObj2 == nullptr)
	{
		return pContainer1->MoveChild(pObj1->Self(), dynamic_cast<NFObject*>(pContainer2.get()), index2);
	}
	else if (pObj1 == nullptr && pObj2 != nullptr)
	{
		return pContainer2->MoveChild(pObj2->Self(), dynamic_cast<NFObject*>(pContainer1.get()), index1);
	}
	else
	{
		if (!pContainer1->RemoveChild(pObj1->Self()))
		{
			return false;
		}

		if (!pContainer2->MoveChild(pObj2->Self(), dynamic_cast<NFObject*>(pContainer1.get()), index1))
		{
			// 移动失败，将物品1添加回容器1
			pContainer1->AddChild(pObj1->Self(), index1);
		}
	}

	return true;
}

bool NFKernelModule::ChangeIndex(const NFGUID& obj, int newIndex)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return false;
	}

	NFObject* pParent = pObj->GetParent();
	if (pParent == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not parent";
		return false;
	}

	return pParent->ChangeChild(obj, newIndex);
}

int NFKernelModule::GetCapacity(const NFGUID& container)
{
	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container";
		return false;
	}

	return pContainer->GetCapacity();
}

int NFKernelModule::ExtendCapacity(const NFGUID& container, int nCap)
{
	NF_SHARE_PTR<NFVisible> pContainer = dynamic_pointer_cast<NFVisible>(GetObject(container));
	if (pContainer == nullptr || !pContainer->IsVisible())
	{
		QLOG_WARING << __FUNCTION__ << " not container";
		return false;
	}

	return pContainer->SetCapacity(nCap);
}

int NFKernelModule::GetIndex(const NFGUID& obj)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return false;
	}

	return pObj->GetIndexInParent();
}

const NFGUID& NFKernelModule::GetChild(const NFGUID& obj, int index)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return NULL_OBJECT;
	}

	return pObj->GetChildByIndex(index);
}

const NFGUID& NFKernelModule::GetChild(const NFGUID& obj, const std::string& name)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return NULL_OBJECT;
	}

	return pObj->GetChildByName(name);
}

int NFKernelModule::GetChildCount(const NFGUID& obj)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return 0;
	}

	return pObj->GetChildNum();
}

const std::vector<NFGUID>& NFKernelModule::GetChildList(const NFGUID& obj)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return NULL_VECTOR<NFGUID>;
	}

	return pObj->GetChildList();
}

bool NFKernelModule::ClearChild(const NFGUID& obj)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(obj));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " obj not find";
		return 0;
	}

	pObj->ClearChild();

	return true;
}

bool NFKernelModule::GetPropertyList(const NFGUID& obj, NFDataList& args)
{
	args.Clear();

	NFObject* pObj = dynamic_cast<NFObject*>(GetObject(obj).get());
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL";
		return false;
	}

	NFIPropertyManager* pMgr = pObj->GetPropertyManager().get();
	if (pMgr == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pMgr == NULL";
		return false;
	}

	NFIProperty* pProp = pMgr->First().get();
	while (pProp != nullptr)
	{
		args << pProp->GetName();
		pProp = pMgr->Next().get();
	}

	return true;
}

bool NFKernelModule::GetRecordList(const NFGUID& obj, NFDataList& args)
{
	args.Clear();

	NFObject* pObj = dynamic_cast<NFObject*>(GetObject(obj).get());
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL";
		return false;
	}

	NFIRecordManager* pMgr = pObj->GetRecordManager().get();
	if (pMgr == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pMgr == NULL";
		return false;
	}

	NFIRecord* pRec = pMgr->First().get();
	while (pRec != nullptr)
	{
		args << pRec->GetName();
		pRec = pMgr->Next().get();
	}

	return true;
}