///--------------------------------------------------------------------
/// 文件名:		NFContainer.h
/// 内  容:		容器
/// 说  明:		
/// 创建日期:	2019年11月12日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
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