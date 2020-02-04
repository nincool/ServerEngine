///--------------------------------------------------------------------
/// 文件名:		UpdateViewportModule.h
/// 内  容:		视窗同步数据模块
/// 说  明:		
/// 创建日期:	2019年8月3日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NF_UPDATE_VIEWPORT_MODULE_H__
#define __NF_UPDATE_VIEWPORT_MODULE_H__

#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "NFComm/NFCore/NFObject.h"
#include "GameServer.h"

class CUpdateViewportModule : public NFIModule
{
public:
	CUpdateViewportModule(NFIPluginManager* p);
    ~CUpdateViewportModule();

    virtual bool Init();
    virtual bool AfterInit();
public:

	// 视窗创建
	int OnViewInit(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 视窗添加对象
	int OnViewAdd(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 视窗移除对象
	int OnViewRemove(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 视窗对象改变位置
	int OnViewChange(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 视窗关闭
	int OnViewClose(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	
private:
	// 打包属性
	bool PackAllProperty(NFObject* pObj, OuterMsg::PropertyList* pPrivateList);
	// 打包所有表
	bool PackAllRecord(NFObject* pObj, OuterMsg::RecordList* pRecList);
	// 打包表
	bool PackRecord(NF_SHARE_PTR<NFIRecord> pRecord, OuterMsg::Record* pPrivateData);
	
	// 对象创建事件
	int OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 视窗属性变化
	int OnViewPropChanged(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData);
	// 视窗表变化
	int OnViewRecordChanged(const NFGUID& self, const RECORD_EVENT_DATA& event, const NFData& oldData, const NFData& newData);

	// 打包一条属性
	bool PackOneProperty(OuterMsg::PropertyList* pList, const std::string& strName, const NFData& data);
	// 打包一条表属性
	bool PackRecordOneData(OuterMsg::RecordList* pList, const RECORD_EVENT_DATA& event, const NFIRecord* pRecordData, const NFData& data);

private:
	NFIKernelModule* m_pKernelModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
};

#endif // __NF_UPDATE_VIEWPORT_MODULE_H__
