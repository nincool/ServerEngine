///--------------------------------------------------------------------
/// �ļ���:		NFContainer.h
/// ��  ��:		����
/// ˵  ��:		
/// ��������:	2019��11��12��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#include "NFContainer.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"

NFContainer::NFContainer(NFGUID self, NFIPluginManager* pPluginManager)
	: NFVisible(self, pPluginManager)
{
	// ����ڲ����� �������Ʋ������ⲿ�޸�
	ADD_RAW_STR(m_pPropertyManager, Name, EDIT_NO, true);
}

NFContainer::~NFContainer()
{
}