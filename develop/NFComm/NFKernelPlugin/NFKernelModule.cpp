///--------------------------------------------------------------------
/// 文件名:		NFKernelModule.cpp
/// 内  容:		核心模块
/// 说  明:		
/// 创建日期:	2019年8月8日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "NFKernelModule.h"
#include "NFComm/NFCore/NFObject.h"
#include "NFComm/NFCore/NFRecord.h"
#include "NFComm/NFCore/NFPerformance.hpp"
#include "NFComm/NFCore/NFPropertyManager.h"
#include "NFComm/NFCore/NFRecordManager.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/NFPlayer.h"
#include "NFComm/NFCore/PBConvert.hpp"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFCore/NFContainer.h"
#include "NFComm/NFUtils/NFTool.hpp"

NFKernelModule::NFKernelModule(NFIPluginManager* p)
{
	pPluginManager = p;
}

NFKernelModule::~NFKernelModule()
{
	m_hashObject.clear();
}

bool NFKernelModule::Init()
{
	mtDeleteSelfList.clear();

	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pHeartBeatModule = pPluginManager->FindModule<NFIHeartBeatModule>();
	m_pEventModule = pPluginManager->FindModule<NFIEventModule>();
	m_pResModule = pPluginManager->FindModule<NFIResModule>();
	m_pWordsFilterModule = pPluginManager->FindModule<IWordsFilterModule>();
	m_pNetClientModule = pPluginManager->FindModule<NetClientModule>();
	m_pNetServerInside = pPluginManager->FindModule<NetServerInsideModule>();
	m_pNetServerMaintainModule = pPluginManager->FindModule<NetServerMaintainModule>();

	return true;
}

bool NFKernelModule::AfterInit()
{
	return true;
}

bool NFKernelModule::Shut()
{
	return true;
}

bool NFKernelModule::Execute()
{
	mnCurExeObject.nHead64 = 0;
	mnCurExeObject.nData64 = 0;

	if (!mtDeleteSelfList.empty())
	{
		for (auto& it: mtDeleteSelfList)
		{
			DestroyObjectAny(it);
		}
		mtDeleteSelfList.clear();
	}

	return true;
}

NF_SHARE_PTR<NFIObject> NFKernelModule::CreateObject(
	const std::string& strClassName, 
	const std::string& strConfigID, 
	const NFDataList& args)
{
	return CreateObject(CreateGUID(), strClassName, strConfigID, args);
}

NF_SHARE_PTR<NFIObject> NFKernelModule::CreateObject(const NFGUID& self,
	const std::string& strClassName, 
	const std::string& strConfigID, 
	const NFDataList& args,
	std::function<void(NF_SHARE_PTR<NFIObject>)> pf/* = nullptr*/)
{
	NF_SHARE_PTR<NFIObject> pObject = nullptr;
	NFGUID ident = self;
	if (ident.IsNull())
	{
		ident = CreateGUID();
	}

	// 已经存在
	if (m_hashObject.find(ident) != m_hashObject.end())
	{
		QLOG_ERROR << __FUNCTION__ << " object already Exists id:" << ident;
		return nullptr;
	}

	// 逻辑类名，优先取配置中的逻辑类名
	std::string strObjClassName = strClassName;
	NF_SHARE_PTR<NFIPropertyManager> pConfigPropertyManager = nullptr;
	if (!strConfigID.empty())
	{
		pConfigPropertyManager = m_pElementModule->GetPropertyManager(strConfigID);
		if (pConfigPropertyManager != nullptr)
		{
			strObjClassName = pConfigPropertyManager->GetPropertyString(LC::IObject::ClassName());
		}
		else
		{
			QLOG_ERROR << __FUNCTION__ << " ConfigID:" << strConfigID << " not find";
			return nullptr;
		}
	}
	
	// 逻辑类
	NF_SHARE_PTR<NFIClass> pClass = m_pClassModule->GetElement(strObjClassName);
	if (pClass == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find ClassName:" << strClassName;
		return nullptr;
	}

	NF_SHARE_PTR<NFIPropertyManager> pStaticClassPropertyManager = pClass->GetPropertyManager();
	NF_SHARE_PTR<NFIRecordManager> pStaticClassRecordManager = pClass->GetRecordManager();
	if (pStaticClassPropertyManager == nullptr
		|| pStaticClassRecordManager == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " not find PropertyManager/RecordManager ClassName:" << strClassName;
		return nullptr;
	}

	// 创建对象
	OuterMsg::ObjectType eObjectType = pClass->GetType();
	switch (eObjectType)
	{
	case OuterMsg::OBJECT_TYPE_PLAYER:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFPlayer(ident, pPluginManager));
		break;
	case OuterMsg::OBJECT_TYPE_NPC:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFVisible(ident, pPluginManager));
		break;
	case OuterMsg::OBJECT_TYPE_ITEM:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFVisible(ident, pPluginManager));
		break;
	case OuterMsg::OBJECT_TYPE_CONTAINER:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFContainer(ident, pPluginManager));
		break;
	case OuterMsg::OBJECT_TYPE_HELPER:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFObject(ident, pPluginManager));
		break;
	case OuterMsg::OBJECT_TYPE_PUB:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFObject(ident, pPluginManager));
		break;
	case OuterMsg::OBJECT_TYPE_GUILD:
		pObject = NF_SHARE_PTR<NFIObject>(NF_NEW NFObject(ident, pPluginManager));
		break;
	}

	NF_SHARE_PTR<NFObject> pNFObject = dynamic_pointer_cast<NFObject>(pObject);
	if (pNFObject == nullptr)
	{
		return false;
	}

	if (pObject == nullptr || pNFObject == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " error create obj ClassName:" << strClassName
			<< " Type:" << pClass->GetType();
		return nullptr;
	}

	// 获取属性和表管理
	NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = pNFObject->GetPropertyManager();
	NF_SHARE_PTR<NFIRecordManager> pRecordManager = pNFObject->GetRecordManager();
	if (pPropertyManager == nullptr
		|| pRecordManager == nullptr)
	{
		QLOG_ERROR << __FUNCTION__ << " error not find PropertyManager/RecordManager ClassName:" << strClassName;
		return nullptr;
	}

	// 根据逻辑类设定对象的属性
	NF_SHARE_PTR<NFIProperty> pStaticProperty = pStaticClassPropertyManager->First();
	while (pStaticProperty != nullptr)
	{
		NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->AddProperty(ident, pStaticProperty);
		if (pProperty == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " error property ClassName:" << strClassName;
			continue;
		}

		pObject->AddPropertyCallBack(pProperty->GetName(), this, &NFKernelModule::OnPropertyCommonEvent);

		pStaticProperty = pStaticClassPropertyManager->Next();
	}

	// 根据逻辑类设定对象的表
	NF_SHARE_PTR<NFIRecord> pStaticRecord = pStaticClassRecordManager->First();
	while (pStaticRecord != nullptr)
	{
		NF_SHARE_PTR<NFRecord> pRecord = dynamic_pointer_cast<NFRecord>(pRecordManager->AddRecord(ident,
			pStaticRecord->GetName(),
			pStaticRecord->GetColData(),
			pStaticRecord->GetMaxRows()));
		if (pRecord == nullptr)
		{
			QLOG_ERROR << __FUNCTION__ << " error record ClassName:" << strClassName;
			continue;
		}

		pRecord->SetPublic(pStaticRecord->GetPublic());
		pRecord->SetPrivate(pStaticRecord->GetPrivate());
		pRecord->SetSave(pStaticRecord->GetSave());

		pObject->AddRecordCallBack(pStaticRecord->GetName(), this, &NFKernelModule::OnRecordCommonEvent);

		pStaticRecord = pStaticClassRecordManager->Next();
	}

	// 加入对象管理
	m_hashObject[ident] = pObject;

	// 对象创建事件
	pClass->RunEventCallBack(EVENT_OnCreate, ident, NFGUID(), NFDataList());

	// 通过配置初始化对象
	if (pConfigPropertyManager != nullptr)
	{
		pPropertyManager->SetRawPropertyEdit(EDIT_OK);
		NF_SHARE_PTR<NFIProperty> pConfigProperty = pConfigPropertyManager->First();
		while (nullptr != pConfigProperty)
		{
			if (!pConfigProperty->IsNull())
			{
				auto pProp = pPropertyManager->GetElement(pConfigProperty->GetName());
				if (pProp != nullptr)
				{
					pProp->SetValue(pConfigProperty.get());
				}
			}

			pConfigProperty = pConfigPropertyManager->Next();
		}

		pPropertyManager->RestoreRawPropertyEdit();
	}

	// 修改设定的属性
	for (int i = 0; i < args.GetCount(); i += 2)
	{
		const std::string& strPropertyName = args.String(i);
	
		NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->GetElement(strPropertyName);
		if (pProperty == nullptr)
		{
			continue;
		}

		switch (pProperty->GetType())
		{
		case TDATA_INT:
			pProperty->SetInt(args.Int(i + 1));
			break;
		case TDATA_FLOAT:
			pProperty->SetFloat(args.Float(i + 1));
			break;
		case TDATA_STRING:
			pProperty->SetString(args.String(i + 1));
			break;
		case TDATA_OBJECT:
			pProperty->SetObject(args.Object(i + 1));
			break;
		default:
			break;
		}
	}

	// 设定固定属性
	pNFObject->SetType(eObjectType);
	pNFObject->SetClassName(strObjClassName, pClass);
	pNFObject->SetConfig(strConfigID);

	// 对象加载事件
	pClass->RunEventCallBack(EVENT_OnLoad, ident, NFGUID(), NFDataList());

	// 特殊操作
	if (pf != nullptr)
	{
		pf(pObject);
	}

	QLOG_DEBUG << "create object: " << ident
		<< " config_name:" << strConfigID;

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(9);
	auto& second_gauge = gauge_family.Add(
		{ {"Type", lexical_cast<std::string>(eObjectType)}, {"ClassName", strObjClassName} });
	second_gauge.Increment();
	CATCH_END

	pNFObject->SetState(COE_CREATE_FINISH);

	// 对象创建完成
	pClass->RunEventCallBack(EVENT_OnComplete, ident, NFGUID(), NFDataList());

	return pObject;
}

NF_SHARE_PTR<NFIObject> NFKernelModule::CreateContainer(const NFGUID& self, 
	std::string& strName, 
	int nCap, 
	std::string& strClassName)
{
	if (strName.empty())
	{
		QLOG_WARING << __FUNCTION__ << " name empty strName:" << strName;
		return false;
	}

	NF_SHARE_PTR<NFObject> pSelfObj = dynamic_pointer_cast<NFObject>(GetObject(self));
	if (pSelfObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " self not find strName:" << strName;
		return nullptr;
	}

	auto pf = [&](NF_SHARE_PTR<NFIObject> pObject)
	{
		NF_SHARE_PTR<NFObject> pContainer = dynamic_pointer_cast<NFObject>(pObject);
		if (pContainer == nullptr)
		{
			QLOG_WARING << __FUNCTION__ << " error create container strName:" << strName;
			return;
		}

		pContainer->SetName(strName);
		pContainer->SetCapacity(nCap);

		// 添加父类
		if (!pSelfObj->AddChild(pContainer->Self(), -1))
		{
			// 添加失败，删除对象
			DestroyObject(pContainer->Self());
		}
	};

	return CreateObject(NFGUID(), strClassName, NULL_STR, NFDataList(), pf);
}

bool NFKernelModule::DestroyObject(const NFGUID self)
{
	if (self == mnCurExeObject
		&& !self.IsNull())
	{
		return DestroySelf(self);
	}

	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(self));
	if (pObj == nullptr)
	{
		return false;
	}

	// 不允许在此接口中删除玩家
	if (pObj->IsPlayer())
	{
		QLOG_ERROR << __FUNCTION__ << " can not del player";
		return false;
	}
	
	return DestroyObject(pObj.get());
}

bool NFKernelModule::DestroyObjectAny(const NFGUID self)
{
	if (self == mnCurExeObject
		&& !self.IsNull())
	{
		return DestroySelf(self);
	}

	NF_SHARE_PTR<NFObject> pObj = dynamic_pointer_cast<NFObject>(GetObject(self));
	if (pObj == nullptr)
	{
		return false;
	}

	return DestroyObject(pObj.get());
}

bool NFKernelModule::DestroyObject(NFObject* pObj)
{
	if (pObj == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pObj == NULL";
		return false;
	}

	const NFGUID& self = pObj->Self();

	// 对象销毁事件
	pObj->OnEvent(EVENT_OnDestroy, NFGUID(), NFDataList());

	// 设置正在被销毁状态
	pObj->SetState(COE_BEFOREDESTROY);

	NFObject* pContainer = pObj->GetParent();
	if (pContainer != nullptr)
	{
		pContainer->RemoveChild(self);
	}

	// 设置已经被销毁状态
	pObj->SetState(COE_DESTROY);

	m_pEventModule->RemoveEventCallBack(self);
	m_pHeartBeatModule->ClearHeartBeat(self);

	QLOG_DEBUG << "destroy object: " << self
		<< " ClassName:" << pObj->ClassName()
		<< " Config:" << pObj->Config();

	CATCH_BEGIN
	auto& gauge_family = pPluginManager->GetMetricsGauge(9);
	auto& second_gauge = gauge_family.Add(
		{ {"Type", lexical_cast<std::string>(pObj->Type())}, {"ClassName", pObj->ClassName()} });
	second_gauge.Decrement();
	CATCH_END

	// 从对象管理器中删除
	m_hashObject.erase(self);

	return true;
}

bool NFKernelModule::DestroyPlayer(const NFGUID self)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(self));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	return DestroyObject(pPlayer.get());
}

bool NFKernelModule::DisableObject(const NFGUID& self)
{
	auto it = m_hashObject.find(self);
	if (it == m_hashObject.end())
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	m_mapDisableObject[self] = it->second;
	m_hashObject.erase(it);

	return true;
}

bool NFKernelModule::EnableObject(const NFGUID& self)
{
	auto it = m_mapDisableObject.find(self);
	if (it == m_mapDisableObject.end())
	{
		QLOG_WARING << __FUNCTION__ << " not find player:" << self;
		return false;
	}

	m_hashObject[self] = it->second;
	m_mapDisableObject.erase(it);

	return true;
}

NF_SHARE_PTR<NFIObject> NFKernelModule::GetDisableObject(const NFGUID& self)
{
	auto it = m_mapDisableObject.find(self);
	if (it == m_mapDisableObject.end())
	{
		return nullptr;
	}

	return it->second;
}

NFGUID NFKernelModule::CreateGUID()
{
	NFGUID xID;
	if (!Port_CreateGuid(xID))
	{
		return NULL_OBJECT;
	}

	return xID;
}

bool NFKernelModule::IsBaseOf(const std::string& strBaseClassName, const std::string& strClassName)
{
	return m_pClassModule->IsBaseOf(strBaseClassName, strClassName);
}

int NFKernelModule::OnPropertyCommonEvent(const NFGUID& self, const std::string& strPropertyName, const NFData& oldVar, const NFData& newVar)
{
	NFPerformance performance;
	auto it = m_hashObject.find(self);
	if (it != m_hashObject.end())
	{
		NF_SHARE_PTR<NFIObject> xObject = it->second;
		if (xObject)
		{
			if (xObject->GetState() >= CLASS_OBJECT_EVENT::COE_CREATE_HASDATA)
			{
				std::list<PROPERTY_EVENT_FUNCTOR_PTR>::iterator it = mtCommonPropertyCallBackList.begin();
				for (; it != mtCommonPropertyCallBackList.end(); it++)
				{
					PROPERTY_EVENT_FUNCTOR_PTR& pFunPtr = *it;
					PROPERTY_EVENT_FUNCTOR* pFun = pFunPtr.get();
					pFun->operator()(self, strPropertyName, oldVar, newVar);
				}

				const std::string& strClassName = xObject->GetPropertyString(LC::IObject::ClassName());
				std::map<std::string, std::list<PROPERTY_EVENT_FUNCTOR_PTR>>::iterator itClass = mtClassPropertyCallBackList.find(strClassName);
				if (itClass != mtClassPropertyCallBackList.end())
				{
					std::list<PROPERTY_EVENT_FUNCTOR_PTR>::iterator itList = itClass->second.begin();
					for (; itList != itClass->second.end(); itList++)
					{
						PROPERTY_EVENT_FUNCTOR_PTR& pFunPtr = *itList;
						PROPERTY_EVENT_FUNCTOR* pFun = pFunPtr.get();
						pFun->operator()(self, strPropertyName, oldVar, newVar);
					}
				}
			}
		}
	}

	if (performance.CheckTimePoint(10))
	{
		QLOG_WARING_S << "[OnPropertyCommonEvent performance]" 
			<< " consume:" << performance.TimeScope() << "ms"
			<< " name:" << strPropertyName;
	}

	return 0;
}

NF_SHARE_PTR<NFIObject> NFKernelModule::GetObject(const NFGUID& ident) const
{
	auto it = m_hashObject.find(ident);
	if (it == m_hashObject.end())
	{
		return nullptr;
	}

	return it->second;
}

bool NFKernelModule::ExistObject(const NFGUID & ident) const
{
	return m_hashObject.find(ident) != m_hashObject.end();
}

const NFKernelModule::HashObject& NFKernelModule::GetObjectAll() const
{
	return m_hashObject;
}

bool NFKernelModule::DestroySelf(const NFGUID& self)
{
	mtDeleteSelfList.push_back(self);
	return true;
}

int NFKernelModule::OnRecordCommonEvent(const NFGUID& self, const RECORD_EVENT_DATA& xEventData, const NFData& oldVar, const NFData& newVar)
{
	NFPerformance performance;

	auto it = m_hashObject.find(self);
	if (it != m_hashObject.end())
	{
		NF_SHARE_PTR<NFIObject> xObject = it->second;
		if (xObject)
		{
			if (xObject->GetState() >= CLASS_OBJECT_EVENT::COE_CREATE_HASDATA)
			{
				std::list<RECORD_EVENT_FUNCTOR_PTR>::iterator it = mtCommonRecordCallBackList.begin();
				for (; it != mtCommonRecordCallBackList.end(); it++)
				{
					RECORD_EVENT_FUNCTOR_PTR& pFunPtr = *it;
					RECORD_EVENT_FUNCTOR* pFun = pFunPtr.get();
					pFun->operator()(self, xEventData, oldVar, newVar);
				}
			}

			const std::string& strClassName = xObject->GetPropertyString(LC::IObject::ClassName());
			std::map<std::string, std::list<RECORD_EVENT_FUNCTOR_PTR>>::iterator itClass = mtClassRecordCallBackList.find(strClassName);
			if (itClass != mtClassRecordCallBackList.end())
			{
				std::list<RECORD_EVENT_FUNCTOR_PTR>::iterator itList = itClass->second.begin();
				for (; itList != itClass->second.end(); itList++)
				{
					RECORD_EVENT_FUNCTOR_PTR& pFunPtr = *itList;
					RECORD_EVENT_FUNCTOR* pFun = pFunPtr.get();
					pFun->operator()(self, xEventData, oldVar, newVar);
				}
			}
		}
	}

	if (performance.CheckTimePoint(10))
	{
		QLOG_WARING_S << "[OnRecordCommonEvent performance]"
			<< " consume:" << performance.TimeScope() << "ms"
			<< " record:" << xEventData.strRecordName
			<< " event type:" << xEventData.nOpType;
	}

	return 0;
}

bool NFKernelModule::RegisterCommonPropertyEvent(const PROPERTY_EVENT_FUNCTOR_PTR& cb)
{
	mtCommonPropertyCallBackList.push_back(cb);
	return true;
}

bool NFKernelModule::RegisterCommonRecordEvent(const RECORD_EVENT_FUNCTOR_PTR& cb)
{
	mtCommonRecordCallBackList.push_back(cb);
	return true;
}

bool NFKernelModule::RegisterClassPropertyEvent(const std::string & strClassName, const PROPERTY_EVENT_FUNCTOR_PTR & cb)
{
	if (mtClassPropertyCallBackList.find(strClassName) == mtClassPropertyCallBackList.end())
	{
		std::list<PROPERTY_EVENT_FUNCTOR_PTR> xList;
		xList.push_back(cb);

		mtClassPropertyCallBackList.insert(std::map< std::string, std::list<PROPERTY_EVENT_FUNCTOR_PTR>>::value_type(strClassName, xList));

		return true;
	}

	std::map< std::string, std::list<PROPERTY_EVENT_FUNCTOR_PTR>>::iterator it = mtClassPropertyCallBackList.find(strClassName);
	it->second.push_back(cb);

	return false;
}

bool NFKernelModule::RegisterClassRecordEvent(const std::string & strClassName, const RECORD_EVENT_FUNCTOR_PTR & cb)
{
	if (mtClassRecordCallBackList.find(strClassName) == mtClassRecordCallBackList.end())
	{
		std::list<RECORD_EVENT_FUNCTOR_PTR> xList;
		xList.push_back(cb);

		mtClassRecordCallBackList.insert(std::map< std::string, std::list<RECORD_EVENT_FUNCTOR_PTR>>::value_type(strClassName, xList));

		return true;
	}

	std::map< std::string, std::list<RECORD_EVENT_FUNCTOR_PTR>>::iterator it = mtClassRecordCallBackList.find(strClassName);
	it->second.push_back(cb);

	return true;
}

bool NFKernelModule::DestroyAll()
{
	for (auto& it : m_mapDisableObject)
	{
		m_hashObject[it.first] = it.second;
	}
	m_mapDisableObject.clear();

	for (auto& it : m_hashObject)
	{
		auto& pObj = it.second;
		if (pObj == nullptr)
		{
			continue;
		}

		mtDeleteSelfList.push_back(pObj->Self());
	}

	Execute();

	return true;
}

bool NFKernelModule::BeforeShut()
{
	DestroyAll();

	mtCommonPropertyCallBackList.clear();
	mtCommonRecordCallBackList.clear();

	mtClassPropertyCallBackList.clear();
	mtClassRecordCallBackList.clear();

	return true;
}

bool NFKernelModule::AddHeartBeat(const NFGUID& obj, const char* func, const OBJECT_HEARTBEAT_FUNCTOR_PTR& cb, int time, int count)
{
	return m_pHeartBeatModule->AddHeartBeat(obj, func, cb, time, count);
}

bool NFKernelModule::RemoveHeartBeat(const NFGUID& obj, const char* func)
{
	return m_pHeartBeatModule->RemoveHeartBeat(obj, func);
}

bool NFKernelModule::ClearHeartBeat(const NFGUID& obj)
{
	return m_pHeartBeatModule->ClearHeartBeat(obj);
}

bool NFKernelModule::FindHeartBeat(const NFGUID& obj, const char* func)
{
	return m_pHeartBeatModule->FindHeartBeat(obj, func);
}

int NFKernelModule::GetHeartBeatCount(const NFGUID& obj, const char* func)
{
	beat_data_t* pHeartBeat = m_pHeartBeatModule->GetHeartBeat(obj, func);
	if (pHeartBeat == nullptr)
	{
		return 0;
	}

	return pHeartBeat->nMax;
}

void NFKernelModule::SetHeartBeatCount(const NFGUID& obj, const char* func, int nCount)
{
	if (nCount <= 0)
	{
		QLOG_WARING << __FUNCTION__ << " count must above 0" << " func:" << func 
			<< " obj:" << obj;
		return;
	}

	beat_data_t* pHeartBeat = m_pHeartBeatModule->GetHeartBeat(obj, func);
	if (pHeartBeat == nullptr)
	{
		return;
	}

	pHeartBeat->nMax = nCount;
	pHeartBeat->nCount = 0;
}

int NFKernelModule::GetHeartBeatTime(const NFGUID& obj, const char* func)
{
	beat_data_t* pHeartBeat = m_pHeartBeatModule->GetHeartBeat(obj, func);
	if (pHeartBeat == nullptr)
	{
		return 0;
	}

	return pHeartBeat->nTime;
}

bool NFKernelModule::AddEventCallBack(
	const std::string& strClassName, 
	EventType eType, 
	NFIModule* pOwner, 
	EVENT_HOOK pHook, 
	int nPrior /*= 0*/)
{
	return m_pClassModule->AddEventCallBack(strClassName, eType, pOwner, pHook, nPrior);
}

bool NFKernelModule::RemoveEventCallBack(
	const std::string& strClassName, 
	EventType eType, 
	NFIModule* pOwner, 
	EVENT_HOOK pHook)
{
	return m_pClassModule->RemoveEventCallBack(strClassName, eType, pOwner, pHook);
}

bool NFKernelModule::RemoveEventCallBack(
	const std::string& strClassName, 
	EventType eType,
	NFIModule* pOwner)
{
	return m_pClassModule->RemoveEventCallBack(strClassName, eType, pOwner);
}

bool NFKernelModule::RemoveEventCallBack(const std::string& strClassName, NFIModule* pOwner)
{
	return m_pClassModule->RemoveEventCallBack(strClassName, pOwner);
}

bool NFKernelModule::RemoveEventCallBack(NFIModule* pOwner)
{
	return m_pClassModule->RemoveEventCallBack(pOwner);
}

bool NFKernelModule::FindEventCallBack(
	const std::string& strClassName, 
	EventType eType, 
	NFIModule* pOwner, 
	EVENT_HOOK pHook)
{
	return m_pClassModule->FindEventCallBack(strClassName, eType, pOwner, pHook);
}

bool NFKernelModule::AddCommondCallBack(const std::string& strClassName, int nMsgId, NFIModule* pOwner, EVENT_HOOK pHook, int nPrior /*= 0*/)
{
	NF_SHARE_PTR<NFIClass> pClass = m_pClassModule->GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->AddCommandCallBack(nMsgId, pOwner, pHook, nPrior);
}

bool NFKernelModule::AddExtraCallBack(const std::string& strClassName, NF_SERVER_TYPES eServerType, int nMsgId, NF_SHARE_PTR<IMethodMsg> pMethod)
{
	NF_SHARE_PTR<NFIClass> pClass = m_pClassModule->GetElement(strClassName);
	if (pClass == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " pClass == NULL";
		return false;
	}

	return pClass->AddExtraCallBack(eServerType, nMsgId, pMethod);
}

bool NFKernelModule::SendExtraMessage(
	NF_SERVER_TYPES eServerType,
	int nMsgId,
	const NFGUID& self,
	const google::protobuf::Message& msg)
{
	std::string strMsgData;
	if (!msg.SerializeToString(&strMsgData))
	{
		QLOG_WARING << __FUNCTION__ << " SerializeToString failed";
		return false;
	}

	OuterMsg::ExtraMsg xMsg;
	xMsg.set_server_type(static_cast<int>(eServerType));
	xMsg.set_msg_id(nMsgId);
	xMsg.set_game_id(pPluginManager->GetAppID());
	xMsg.set_data(strMsgData);
	if (!PBConvert::ToPB(self, *xMsg.mutable_object_id()))
	{
		QLOG_WARING << __FUNCTION__ << " ToPB failed";
		return false;
	}

	std::string strMsg;
	if (!xMsg.SerializeToString(&strMsg))
	{
		QLOG_WARING << "SerializeToString failed";
		return false;
	}

	if (!m_pNetClientModule->SendMsgByType(eServerType, OuterMsg::SS_REQ_EXTRA_SERVER, strMsg))
	{
		return m_pNetServerInside->SendMsgToTypeServer(eServerType, OuterMsg::SS_REQ_EXTRA_SERVER, strMsg);
	}
	
	return true;
}

/// \breif 添加维护端口发来消息回调
void NFKernelModule::AddMaintainCallBack(const uint16_t nMsgID, NFIModule* pBase, NetMsgFun fun)
{
	m_pNetServerMaintainModule->AddReceiveCallBack(nMsgID, pBase, fun);
}
/// \breif 发送消息给维护端口
bool NFKernelModule::SendMaintainMessage(const uint16_t nMsgID, const std::string& xData, const NFSOCK nSockIndex)
{
	return m_pNetServerMaintainModule->SendMsg(nMsgID, xData, nSockIndex);
}

// 加载配置
const google::protobuf::Message* NFKernelModule::LoadRes(const char* szPath, ResReloadFunction pfReload/* = nullptr*/)
{
	return m_pResModule->Load(szPath, pfReload);
}

bool NFKernelModule::ReloadRes(const char* szPath)
{
	return m_pResModule->Reload(szPath);
}

bool NFKernelModule::ReloadResAll()
{
	return m_pResModule->ReloadAll();
}

bool NFKernelModule::ConvertMapData(
	const google::protobuf::Message* pRes, 
	ResMapStrData& mapData, 
	const std::string& strKeyName)
{
	return m_pResModule->ConvertMapData(pRes, mapData, strKeyName);
}
bool NFKernelModule::ConvertMapData(
	const google::protobuf::Message* pRes, 
	ResMapIntData& mapData, 
	const std::string& strKeyName)
{
	return m_pResModule->ConvertMapData(pRes, mapData, strKeyName);
}

int64_t NFKernelModule::GetResInt(const std::string& strConfigID, const std::string& strPropName)
{
	return m_pElementModule->GetPropertyInt(strConfigID, strPropName);
}

double NFKernelModule::GetResFloat(const std::string& strConfigID, const std::string& strPropName)
{
	return m_pElementModule->GetPropertyFloat(strConfigID, strPropName);
}

const std::string& NFKernelModule::GetResString(const std::string& strConfigID, const std::string& strPropName)
{
	return m_pElementModule->GetPropertyString(strConfigID, strPropName);
}

//增加一个敏感词
bool NFKernelModule::AddSensitiveWord(const std::string& strSensitiveWord)
{
	return m_pWordsFilterModule->AddSensitiveWord(strSensitiveWord);
}

//查找字符串里所有的敏感词
void NFKernelModule::FindAllSensitiveWords(const std::string& text, std::set<std::string>& out)
{
	m_pWordsFilterModule->FindAllSensitiveWords(text, out);
}

//把字符串里所有的敏感词替换 返回新串
void NFKernelModule::ReplaceAllSensitiveWords(const std::string& text, std::string& out, const unsigned char replacedChar)
{
	m_pWordsFilterModule->ReplaceAllSensitiveWords(text, out, replacedChar);
}

//检测字符串里是否有敏感词
bool NFKernelModule::ExistSensitiveWord(const std::string& text) const
{
	return m_pWordsFilterModule->ExistSensitiveWord(text);
}

bool NFKernelModule::PlayerLeaveGame(const NFGUID& player)
{
	NF_SHARE_PTR<NFPlayer> pPlayer = dynamic_pointer_cast<NFPlayer>(GetObject(player));
	if (pPlayer == nullptr)
	{
		QLOG_WARING << __FUNCTION__ << " player not exist";
		return false;
	}

	OuterMsg::GUID xMsg;
	PBConvert::ToPB(player, xMsg);
	std::string strMsg;
	if (!xMsg.SerializeToString(&strMsg))
	{
		QLOG_ERROR << __FUNCTION__ << " GUID SerializeToString failed:" << player;
		return false;
	}

	return m_pNetClientModule->SendMsgByType(NF_ST_WORLD, OuterMsg::SS_REQ_PLAYER_LEAVE_GAME, strMsg);
}

prometheus::Family<prometheus::Counter>& NFKernelModule::GetMetricsCounter(int nTempIndex)
{
	return pPluginManager->GetMetricsCounter(nTempIndex);
}

prometheus::Family<prometheus::Gauge>& NFKernelModule::GetMetricsGauge(int nTempIndex)
{
	return pPluginManager->GetMetricsGauge(nTempIndex);
}

prometheus::Family<prometheus::Histogram>& NFKernelModule::GetMetricsHistogram(int nTempIndex)
{
	return pPluginManager->GetMetricsHistogram(nTempIndex);
}

prometheus::Family<prometheus::Summary>& NFKernelModule::GetMetricsSummary(int nTempIndex)
{
	return pPluginManager->GetMetricsSummary(nTempIndex);
}
