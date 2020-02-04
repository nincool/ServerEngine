///--------------------------------------------------------------------
/// �ļ���:		NFContainer.h
/// ��  ��:		����
/// ˵  ��:		
/// ��������:	2019��11��12��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __NF_CONTAINER_H__
#define __NF_CONTAINER_H__

#include "NFVisible.h"

 class _NFExport NFContainer
     : public NFVisible
 {
 private:
	 NFContainer() : NFVisible(NFGUID(), nullptr) {};

 public:
	 NFContainer(NFGUID self, NFIPluginManager* pPluginManager);
	 virtual ~NFContainer();
 };

#endif // __NF_CONTAINER_H__