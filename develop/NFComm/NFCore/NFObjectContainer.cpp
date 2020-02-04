///--------------------------------------------------------------------
/// 文件名:		NFObjectContainer.cpp
/// 内  容:		容器
/// 说  明:		
/// 创建日期:	2019年7月26日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFObject.h"
#include "NFViewport.h"

bool NFObject::SetCapacity(int nCap)
{
	if (nCap < 0)
	{
		QLOG_ERROR << __FUNCTION__ << " capital < 0 container:" << m_strName;
		return false;
	}
	else if (nCap < m_nCapacity)
	{
		int nSize = static_cast<int>(m_vecChildren.size());
		if (nCap < nSize)
		{
			for (int i = nCap; i < nSize; ++i)
			{
				if (!m_vecChildren[i].IsNull())
				{
					QLOG_WARING << __FUNCTION__ << " container:" << m_strName
						<< " cap:" << nCap 
						<< " capacity:" << m_nCapacity 
						<< " less failed index:" << i << " used ";
					return false;
				}

				// 删除不用的格子
				m_vecChildren.erase(m_vecChildren.begin() + nCap, m_vecChildren.end());
			}
		}
	}

	m_nCapacity = nCap;

	return true;
}

int NFObject::GetCapacity() const
{
	return m_nCapacity;
}

int NFObject::GetUnusedIndex() const
{
	int nSize = static_cast<int>(m_vecChildren.size());
	for (int i = 0; i < nSize; ++i)
	{
		if (m_vecChildren[i].IsNull())
		{
			return i;
		}
	}

	// 容量为0 支持末尾添加
	if (m_nCapacity <= 0)
	{
		return nSize;
	}

	// 满了 末尾添加
	if (nSize < m_nCapacity)
	{
		return nSize;
	}

	return -1;
}

bool NFObject::AddChild(const NFGUID& ident, int index, bool bTest/* = false*/)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(ident));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " child not exist ident:" << ident.ToString();
		return false;
	}

	// 对象是否正在被删除
	if (pObj->GetState() == COE_BEFOREDESTROY
		|| pObj->GetState() == COE_DESTROY)
	{
		return false;
	}

	// 对象已在其他容器中
	if (pObj->GetParent() != nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " obj in other container ident:" << ident.ToString();
		return false;
	}

	// 容器已满
	if (m_nCapacity > 0 && m_nChildNum >= m_nCapacity)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName << " full";
		return false;
	}

	// 找到正确的索引
	if (index >= 0 && m_nCapacity > 0)
	{
		int nSize = static_cast<int>(m_vecChildren.size());
		if (index < nSize)
		{
			if (!m_vecChildren[index].IsNull())
			{
				QLOG_WARING << __FUNCTION__ << " container:" << m_strName 
					<< " index:" << index << " already exist ";
				return false;
			}
		}
		else if (index < m_nCapacity)
		{
			// 扩充
			m_vecChildren.resize(index, NFGUID());
		}
		else
		{
			QLOG_WARING << __FUNCTION__ << " container:" << m_strName
				<< " index:" << index 
				<< " over capacity:" << m_nCapacity;
			return false;
		}
	}
	else
	{	
		// 找到未使用位置
		index = GetUnusedIndex();
		if (index < 0)
		{
			QLOG_WARING << __FUNCTION__ << " container:" << m_strName 
				<< " full size:" << m_nCapacity;
			return false;
		}
	}

	// 询问是否允许容器添加对象
	if (bTest)
	{
		int nRet = OnEvent(EVENT_OnNoAdd, ident, NFDataList() << index);
		if (nRet > 0)
		{
			return false;
		}
	}

	// 容器添加对象前事件
	OnEvent(EVENT_OnAdd, ident, NFDataList() << index);

	// 保存对象
	if (index == static_cast<int>(m_vecChildren.size()))
	{
		m_vecChildren.push_back(ident);
	}
	else
	{
		m_vecChildren[index] = ident;
	}

	// 增加子对象数量
	++m_nChildNum;

	// 记录对象所在容器和位置
	pObj->SetParent(this);
	pObj->SetIndexInParent(index);

	// 对象进入容器事件
	pObj->OnEvent(EVENT_OnEntry, Self(), NFDataList() << index);

	// 容器添加对象后事件
	OnEvent(EVENT_OnAfterAdd, pObj->Self(), NFDataList() << index);

	// 通知视窗更新
	NotifyViewerAdd(ident, index);

	return true;
}

bool NFObject::RemoveChild(const NFGUID& ident, bool bTest/* = false*/)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(ident));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " child not exist ident:" << ident.ToString();
		return false;
	}

	if (pObj->GetParent() != this)
	{
		std::ostringstream str;
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " obj not in this ident:" << ident.ToString();
		return false;
	}

	int index = pObj->GetIndexInParent();
	if (index < 0 || index >= static_cast<int>(m_vecChildren.size()))
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " obj index:" << index << 
			" invaild ident:" << ident.ToString();
		return false;
	}

	if (m_vecChildren[index] != ident)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " obj not find ident:" << ident.ToString();
		return false;
	}

	return RemoveChildByIndex(index, bTest);
}

bool NFObject::RemoveChildByIndex(int index, bool bTest/* = false*/)
{
	if (index < 0 || index >= static_cast<int>(m_vecChildren.size()))
	{
		std::ostringstream str;
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " index:" << index 
			<< " invaild size:" << static_cast<int>(m_vecChildren.size());
		return false;
	}

	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(m_vecChildren[index]));
	if (pObj == nullptr)
	{
		std::ostringstream str;
		QLOG_ERROR << __FUNCTION__ << " container:" << m_strName
			<< " child not exist ident:" << m_vecChildren[index].ToString();
		return false;
	}

	// 询问是否允许容器移除对象
	if (bTest)
	{
		int nRet = OnEvent(EVENT_OnNoRemove, pObj->Self(), NFDataList() << index);
		if (nRet > 0)
		{
			return false;
		}
	}

	// 容器移除对象前事件
	OnEvent(EVENT_OnBeforeRemove, pObj->Self(), NFDataList() << index);

	m_vecChildren[index] = NFGUID();
	--m_nChildNum;
	pObj->SetParent(nullptr);
	pObj->SetIndexInParent(-1);

	// 对象离开容器事件
	pObj->OnEvent(EVENT_OnLeave, Self(), NFDataList() << index);

	// 容器移除对象后事件
	OnEvent(EVENT_OnRemove, pObj->Self(), NFDataList() << index);

	// 通知视窗更新
	NotifyViewerRemove(index);

	return true;
}

bool NFObject::MoveChild(const NFGUID& ident, NFObject* pOtherContainer, int index)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(ident));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " child not exist ident:" << ident.ToString();
		return false;
	}

	int oldIndex = pObj->GetIndexInParent();
	if (!RemoveChild(ident))
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " remove failed ident:" << ident.ToString();
		return false;
	}

	if (!pOtherContainer->AddChild(ident, index))
	{
		// 添加失败 将对象返回原处
		if (!AddChild(ident, oldIndex))
		{
			QLOG_ERROR << __FUNCTION__ << " container:" << m_strName
				<< " child return old container failed ident:" << ident.ToString() 
				<< " oldIndex:" << oldIndex;
		}

		QLOG_ERROR << __FUNCTION__ << " container:" << pOtherContainer->GetName()
			<< " addchild failed ident:" << ident.ToString()
			<< " index:" << index;
		return false;
	}
	
	return true;
}

bool NFObject::ChangeChild(const NFGUID& ident, int index)
{
	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(ident));
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " child not exist ident:" << ident.ToString();
		return false;
	}

	if (pObj->GetParent() != this)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " obj not in this ident:" << ident.ToString();
		return false;
	}

	int oldIndex = pObj->GetIndexInParent();
	if (oldIndex < 0 || oldIndex >= static_cast<int>(m_vecChildren.size()))
	{
		QLOG_ERROR << __FUNCTION__ << " container:" << m_strName
			<< " obj index:" << oldIndex 
			<< " invaild ident:" << ident.ToString();
		return false;
	}

	if (m_vecChildren[oldIndex] != ident)
	{
		QLOG_ERROR << __FUNCTION__ << " container:" << m_strName
			<< " obj not find ident:" << ident.ToString();
		return false;
	}
	
	if (index == oldIndex)
	{
		// 不需要移动
		return true;
	}

	if (index < 0 || index >= m_nCapacity)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " new index:" << index 
			<< "less zero or over capacity:" << m_nCapacity;
		return false;
	}

	if (index >= static_cast<int>(m_vecChildren.size()))
	{
		// 扩容
		m_vecChildren.resize((index + 1), NFGUID());
	}

	const NFGUID& otherIdent = m_vecChildren[index];
	if (!otherIdent.IsNull())
	{
		NF_SHARE_PTR<NFObject> pOtherObj = dynamic_pointer_cast<NFObject>(m_pKernelModule->GetObject(otherIdent));
		if (pOtherObj == nullptr)
		{
			std::ostringstream str;
			QLOG_ERROR << __FUNCTION__ << " container:" << m_strName
				<< " child not exist ident:" << otherIdent.ToString();
			return false;
		}

		if (pOtherObj->GetParent() != this)
		{
			QLOG_ERROR << __FUNCTION__ << " container:" << m_strName
				<< " obj not in this ident:" << otherIdent.ToString();
			return false;
		}

		// 新位置不为空，则交换位置
		m_vecChildren[oldIndex] = otherIdent;
		m_vecChildren[index] = ident;
		pObj->SetIndexInParent(index);
		pOtherObj->SetIndexInParent(oldIndex);

		// 容器对象改变位置
		OnEvent(EVENT_OnChange, pOtherObj->Self(), NFDataList() << index << oldIndex);
	}
	else
	{
		m_vecChildren[oldIndex] = NFGUID();
		m_vecChildren[index] = ident;
		pObj->SetIndexInParent(index);
	}

	// 容器对象改变位置
	OnEvent(EVENT_OnChange, pObj->Self(), NFDataList() << oldIndex << index);

	// 更新视窗
	NotifyViewerChange(oldIndex, index);

	return 0;
}

void NFObject::ClearChild()
{
	if (m_nChildNum <= 0)
	{
		return;
	}

	int nSize = static_cast<int>(m_vecChildren.size());
	for (int i = 0; i < nSize; ++i)
	{
		m_pKernelModule->DestroyObject(m_vecChildren[i]);
	}

	m_vecChildren.clear();
}

const NFGUID& NFObject::GetChildByIndex(int index)
{
	if (index < 0 || index >= static_cast<int>(m_vecChildren.size()))
	{
		QLOG_WARING << __FUNCTION__ << " out range index:" << index;
		return NULL_OBJECT;
	}

	return m_vecChildren[index];
}

NF_SHARE_PTR<NFIObject> NFObject::GetChildObjByIndex(int index)
{
	return m_pKernelModule->GetObject(GetChildByIndex(index));
}

// 效率比较低，待优化
const NFGUID& NFObject::GetChildByName(const std::string& strName)
{
	NF_SHARE_PTR<NFIObject> pObj = GetChildObjByName(strName);
	if (pObj != nullptr)
	{
		return pObj->Self();
	}

	return NULL_OBJECT;
}

// 效率比较低，待优化
NF_SHARE_PTR<NFIObject> NFObject::GetChildObjByName(const std::string& strName)
{
	for (auto& it : m_vecChildren)
	{
		NF_SHARE_PTR<NFIObject> pObj = m_pKernelModule->GetObject(it);
		if (pObj != nullptr && pObj->GetName() == strName)
		{
			return pObj;
		}
	}

	return nullptr;
}

int NFObject::GetChildNum() const
{
	return m_nChildNum;
}

int NFObject::GetChildSize() const
{
	return static_cast<int>(m_vecChildren.size());
}

bool NFObject::IsVaildIndex(int index)
{
	return index >= 0 && index < m_nCapacity;
}