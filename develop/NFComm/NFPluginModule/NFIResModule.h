///--------------------------------------------------------------------
/// 文件名:		NFIResModule.h
/// 内  容:		配置文件管理
/// 说  明:		加载proto二进制形式的配置文件
/// 创建日期:	2019年8月15日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NFI_RES_MODULE_H__
#define __NFI_RES_MODULE_H__

#include "NFIModule.h"
#include "Dependencies/protobuf/src/google/protobuf/message.h"
#include "KConstDefine.h"

class NFIResModule : public NFIModule
{
public:
	virtual ~NFIResModule() {};

	/// \breif   Load 加载配置文件
	virtual const google::protobuf::Message* Load(const char* szPath, ResReloadFunction pfReload = nullptr) = 0;

	/// \breif   Reload 重载已经加载过的配置
	virtual bool Reload(const char* szPath) = 0;

	/// \breif   ReloadAll 重载所有已经加载过的配置
	virtual bool ReloadAll() = 0;

	/// \breif   ConvertMapData 将 pb 数据转换为map
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName) = 0;
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName) = 0;
};
#endif // __NFI_RES_MODULE_H__