///--------------------------------------------------------------------
/// 文件名:		UpdateVisInfoModule.h
/// 内  容:		属性和表同步客户端
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __NF_UPDATE_VISINFO_MODULE_H__
#define __NF_UPDATE_VISINFO_MODULE_H__

#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFMessageDefine/OuterBase.pb.h"
#include "GameServer.h"

class CUpdateVisInfoModule : public NFIModule
{
public:
	CUpdateVisInfoModule(NFIPluginManager* p);
    ~CUpdateVisInfoModule();

    virtual bool Init();
    virtual bool AfterInit();

	// 同步玩家所有数据
	void UpdateAllPlayerData(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
private:
	// 对象事件添加
	int OnComplete(const NFGUID& self, const NFGUID& sender, const NFDataList& args);
	// 可视属性更新
	int OnVisPropUpdate(const NFGUID& self, const std::string& strName, const NFData& oldData, const NFData& newData);
	// 可视表更新
	int OnVisRecordUpdate(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldData, const NFData& newData);
	// 同步玩家所有可视属性
	int VisAllPropUpdate(const NFGUID& self);
	// 同步玩家所有可视表
	int VisAllRecordUpdate(const NFGUID& self);

	// 打包属性
	bool PackAllProperty(NFObject* pObj, OuterMsg::PropertyList* pPrivateList);
	// 打包所有表
	bool PackAllRecord(NFObject* pObj, OuterMsg::RecordList* pRecList);
	// 打包表
	bool PackRecord(NF_SHARE_PTR<NFIRecord> pRecord, OuterMsg::Record* pPrivateData);
private:
	NFIKernelModule* m_pKernelModule = nullptr;
	CGameServer* m_pGameServer = nullptr;
};

#endif // __NF_UPDATE_VISINFO_MODULE_H__
