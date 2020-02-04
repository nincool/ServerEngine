///--------------------------------------------------------------------
/// 文件名:		NFContainer.h
/// 内  容:		容器
/// 说  明:		
/// 创建日期:	2019年11月12日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFContainer.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"

NFContainer::NFContainer(NFGUID self, NFIPluginManager* pPluginManager)
	: NFVisible(self, pPluginManager)
{
	// 添加内部属性 容器名称不允许外部修改
	ADD_RAW_STR(m_pPropertyManager, Name, EDIT_NO, true);
}

NFContainer::~NFContainer()
{
}