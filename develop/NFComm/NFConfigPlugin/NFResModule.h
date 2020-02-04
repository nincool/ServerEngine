///--------------------------------------------------------------------
/// 文件名:		NFResModule.h
/// 内  容:		配置文件管理
/// 说  明:		加载proto二进制形式的配置文件
/// 创建日期:	2019年8月15日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NF_RES_MODULE_H__
#define __NF_RES_MODULE_H__

#include "NFComm/NFPluginModule/NFIResModule.h"
#include "../NFPluginModule/NFIElementModule.h"
#include "../NFPluginModule/KConstDefine.h"

class NFResModule : public NFIResModule
{
private:
	NFResModule() {};
public:
	NFResModule(NFIPluginManager* p);
	virtual ~NFResModule();

	virtual bool Init();
	virtual bool AfterInit();

	/// \breif   Load 加载配置文件
	virtual const google::protobuf::Message* Load(const char* szPath, ResReloadFunction pfReload = nullptr);

	/// \breif   Reload 重载已经加载过的配置
	virtual bool Reload(const char* szPath);

	/// \breif   ReloadAll 重载所有已经加载过的配置
	virtual bool ReloadAll();

	/// \breif   ConvertMapData 将 pb 数据转换为map
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapStrData& mapData, const std::string& strKeyName);
	virtual bool ConvertMapData(const google::protobuf::Message* pRes, ResMapIntData& mapData, const std::string& strKeyName);

protected:

	/// \breif   Get 获取配置文件
	virtual google::protobuf::Message* InnerGet(const char* szPath);

	/// \breif   Load 加载配置文件
	virtual bool InnerLoad(google::protobuf::Message* pRes, const char* szPath, bool bReload = false);
 
	/// \breif   InnerAddReloadHook 添加重载回调
	virtual bool InnerAddReloadHook(const char* szPath, const google::protobuf::Message* pRes, ResReloadFunction pfReload);

	/// \breif   GetMapRowData 获得一行数据
	virtual bool GetMapRowData(const google::protobuf::Message& msg, const std::string& strKeyName, NFData& xKeyData, MapRowData& mapRow);
private:
	NFIElementModule* m_pElementModule = nullptr;

	typedef std::map <std::string, google::protobuf::Message*> MapRes;
	MapRes m_mapRes;

	typedef std::map <std::string, std::vector<ResReloadFunction>> MapHook;
	MapHook m_mapHook;
};

#endif // __NF_RES_MODULE_H__
