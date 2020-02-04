///--------------------------------------------------------------------
/// 文件名:		PubInfoUpdateModule.h
/// 内  容:		可视属性和表同步给game
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef _PUBINFO_UPDATE_MODULE_H__
#define _PUBINFO_UPDATE_MODULE_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CPubInfoUpdateModule : public NFIModule
{
public:
	CPubInfoUpdateModule(NFIPluginManager* p);
    ~CPubInfoUpdateModule();

    virtual bool Init();
    virtual bool AfterInit();

private:
	// 对象事件添加
	int OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 可视属性更新
	int OnVisPropUpdate(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData);
	// 可视表更新
	int OnVisRecordUpdate(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldData, const NFData& newData);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif // _PUBINFO_UPDATE_MODULE_H__
