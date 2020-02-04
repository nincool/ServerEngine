///--------------------------------------------------------------------
/// �ļ���:		NFObjectContainer.cpp
/// ��  ��:		����
/// ˵  ��:		
/// ��������:	2019��7��26��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
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

				// ɾ�����õĸ���
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

	// ����Ϊ0 ֧��ĩβ���
	if (m_nCapacity <= 0)
	{
		return nSize;
	}

	// ���� ĩβ���
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

	// �����Ƿ����ڱ�ɾ��
	if (pObj->GetState() == COE_BEFOREDESTROY
		|| pObj->GetState() == COE_DESTROY)
	{
		return false;
	}

	// ������������������
	if (pObj->GetParent() != nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName
			<< " obj in other container ident:" << ident.ToString();
		return false;
	}

	// ��������
	if (m_nCapacity > 0 && m_nChildNum >= m_nCapacity)
	{
		QLOG_WARING << __FUNCTION__ << " container:" << m_strName << " full";
		return false;
	}

	// �ҵ���ȷ������
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
			// ����
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
		// �ҵ�δʹ��λ��
		index = GetUnusedIndex();
		if (index < 0)
		{
			QLOG_WARING << __FUNCTION__ << " container:" << m_strName 
				<< " full size:" << m_nCapacity;
			return false;
		}
	}

	// ѯ���Ƿ�����������Ӷ���
	if (bTest)
	{
		int nRet = OnEvent(EVENT_OnNoAdd, ident, NFDataList() << index);
		if (nRet > 0)
		{
			return false;
		}
	}

	// ������Ӷ���ǰ�¼�
	OnEvent(EVENT_OnAdd, ident, NFDataList() << index);

	// �������
	if (index == static_cast<int>(m_vecChildren.size()))
	{
		m_vecChildren.push_back(ident);
	}
	else
	{
		m_vecChildren[index] = ident;
	}

	// �����Ӷ�������
	++m_nChildNum;

	// ��¼��������������λ��
	pObj->SetParent(this);
	pObj->SetIndexInParent(index);

	// ������������¼�
	pObj->OnEvent(EVENT_OnEntry, Self(), NFDataList() << index);

	// ������Ӷ�����¼�
	OnEvent(EVENT_OnAfterAdd, pObj->Self(), NFDataList() << index);

	// ֪ͨ�Ӵ�����
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

	// ѯ���Ƿ����������Ƴ�����
	if (bTest)
	{
		int nRet = OnEvent(EVENT_OnNoRemove, pObj->Self(), NFDataList() << index);
		if (nRet > 0)
		{
			return false;
		}
	}

	// �����Ƴ�����ǰ�¼�
	OnEvent(EVENT_OnBeforeRemove, pObj->Self(), NFDataList() << index);

	m_vecChildren[index] = NFGUID();
	--m_nChildNum;
	pObj->SetParent(nullptr);
	pObj->SetIndexInParent(-1);

	// �����뿪�����¼�
	pObj->OnEvent(EVENT_OnLeave, Self(), NFDataList() << index);

	// �����Ƴ�������¼�
	OnEvent(EVENT_OnRemove, pObj->Self(), NFDataList() << index);

	// ֪ͨ�Ӵ�����
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
		// ���ʧ�� �����󷵻�ԭ��
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
		// ����Ҫ�ƶ�
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
		// ����
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

		// ��λ�ò�Ϊ�գ��򽻻�λ��
		m_vecChildren[oldIndex] = otherIdent;
		m_vecChildren[index] = ident;
		pObj->SetIndexInParent(index);
		pOtherObj->SetIndexInParent(oldIndex);

		// ��������ı�λ��
		OnEvent(EVENT_OnChange, pOtherObj->Self(), NFDataList() << index << oldIndex);
	}
	else
	{
		m_vecChildren[oldIndex] = NFGUID();
		m_vecChildren[index] = ident;
		pObj->SetIndexInParent(index);
	}

	// ��������ı�λ��
	OnEvent(EVENT_OnChange, pObj->Self(), NFDataList() << oldIndex << index);

	// �����Ӵ�
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

// Ч�ʱȽϵͣ����Ż�
const NFGUID& NFObject::GetChildByName(const std::string& strName)
{
	NF_SHARE_PTR<NFIObject> pObj = GetChildObjByName(strName);
	if (pObj != nullptr)
	{
		return pObj->Self();
	}

	return NULL_OBJECT;
}

// Ч�ʱȽϵͣ����Ż�
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