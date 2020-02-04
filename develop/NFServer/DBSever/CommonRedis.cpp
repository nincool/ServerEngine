///--------------------------------------------------------------------
/// 文件名:		CommonRedis.cpp
/// 内  容:		通用数据库
/// 说  明:		
/// 创建日期:	2019年8月27日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include "CommonRedis.h"
#include "NFComm/NFCore/NFPropertyManager.h"
#include "NFComm/NFCore/NFRecordManager.h"
#include "NFComm/NFMessageDefine/LogicClassDefine.hpp"
#include "NFComm/NFCore/PBConvert.hpp"
#include "DBServer.h"
#include "ColdData.h"

const std::string PLAYER_LIST = "PlayerList";
const std::string GUILD_LIST = "GuildList";
const std::string PUB_LIST = "PubList";
const std::string PLATFORM_LIST = "PlatformList";
const std::string OBJECT_DATA = "_object_data_";
const std::string RANK_LIST = "RankList";
const std::string OBJECT_INFO_FLAG = "_ObjectInfo";
const std::string RANK_DATA_FLAG = "_RankData";
const std::string LETTER_INFO_FLAG = "_LetterInfo";
const std::string UPDATE_TIME = "_update_time_";

CCommonRedis::CCommonRedis(NFIPluginManager * p)
{
	pPluginManager = p;
}

const std::string& CCommonRedis::GetLetterCacheFlag()
{
	return LETTER_INFO_FLAG;
}

const std::string CCommonRedis::GetLetterCacheKey(const NFGUID& self)
{
	std::string key = self.ToString() + LETTER_INFO_FLAG;
	m_pColdData->RefreshData(LETTER_INFO_FLAG, key);

	return std::move(key);
}

const std::string CCommonRedis::GetAccountCacheKey(const std::string & strAccount, int nServerID)
{
	return std::move(strAccount + "_AccountInfo_" + to_string(nServerID));
}

const std::string CCommonRedis::GetPasswordCacheKey(const std::string& strAccount)
{
	return std::move(strAccount + "_Password");
}

const std::string CCommonRedis::GetRankCacheKey(const std::string& strName)
{
	return std::move(strName + "_RankInfo");
}

const std::string& CCommonRedis::GetRankDataCacheFlag()
{
	return RANK_DATA_FLAG;
}

const std::string CCommonRedis::GetRankDataCacheKey(const NFGUID& self)
{
	std::string key = self.ToString() + RANK_DATA_FLAG;
	m_pColdData->RefreshData(RANK_DATA_FLAG, key);

	return std::move(key);
}

const std::string CCommonRedis::GetRankListCacheKey(int nServerID)
{
	return std::move(RANK_LIST + "_" + to_string(nServerID));
}

const std::string CCommonRedis::GetPlayerListCacheKey(int nServerID)
{
	return std::move(PLAYER_LIST + "_" + to_string(nServerID));
}

const std::string CCommonRedis::GetGuildListCacheKey(int nServerID)
{
	return std::move(GUILD_LIST + "_" + to_string(nServerID));
}

const std::string CCommonRedis::GetPubListCacheKey(int nServerID)
{
	return std::move(PUB_LIST + "_" + to_string(nServerID));
}

const std::string CCommonRedis::GetPlatformListCacheKey(int nServerID)
{
	return std::move(PLATFORM_LIST + "_" + to_string(nServerID));
}

const std::string& CCommonRedis::GetObjectCacheFlag()
{
	return OBJECT_INFO_FLAG;
}

const std::string CCommonRedis::GetObjectCacheKey(const NFGUID& self)
{
	std::string key = self.ToString() + OBJECT_INFO_FLAG;
	m_pColdData->RefreshData(OBJECT_INFO_FLAG, key);

	return std::move(key);
}

const std::string& CCommonRedis::GetObjectDataCacheKey()
{
	return OBJECT_DATA;
}

const std::string CCommonRedis::GetPlayerNameCacheKey(const std::string& strName)
{
	return std::move(strName + "_PlayerNameTemp");
}

const std::string& CCommonRedis::GetUpdateTime()
{
	return UPDATE_TIME;
}

bool CCommonRedis::Init()
{
	m_pLogicClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pNoSqlModule = pPluginManager->FindModule<NFINoSqlModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	m_pColdData = pPluginManager->FindModule<CColdData>();

	return true;
}

bool CCommonRedis::AfterInit()
{
    return true;
}

NF_SHARE_PTR<NFIPropertyManager> CCommonRedis::NewPropertyManager(const std::string& strClassName)
{
    NF_SHARE_PTR<NFIPropertyManager> pStaticClassPropertyManager = m_pLogicClassModule->GetClassPropertyManager(strClassName);
    if (pStaticClassPropertyManager)
    {
        NFGUID ident;
        NF_SHARE_PTR<NFIPropertyManager> pPropertyManager(NF_NEW NFPropertyManager(ident));

        NF_SHARE_PTR<NFIProperty> pStaticConfigPropertyInfo = pStaticClassPropertyManager->First();
        while (pStaticConfigPropertyInfo)
        {
			if (pStaticConfigPropertyInfo->GetSave())
			{
				NF_SHARE_PTR<NFIProperty> xProperty = pPropertyManager->AddProperty(ident, pStaticConfigPropertyInfo->GetName(), pStaticConfigPropertyInfo->GetTypeEx());

				xProperty->SetPublic(pStaticConfigPropertyInfo->GetPublic());
				xProperty->SetPrivate(pStaticConfigPropertyInfo->GetPrivate());
				xProperty->SetSave(pStaticConfigPropertyInfo->GetSave());
			}

            pStaticConfigPropertyInfo = pStaticClassPropertyManager->Next();
        }

        return pPropertyManager;
    }

    return NF_SHARE_PTR<NFIPropertyManager>(NULL);
}

NF_SHARE_PTR<NFIRecordManager> CCommonRedis::NewRecordManager(const std::string& strClassName)
{
    NF_SHARE_PTR<NFIRecordManager> pStaticClassRecordManager = m_pLogicClassModule->GetClassRecordManager(strClassName);
    if (pStaticClassRecordManager)
    {
        NFGUID ident;
        NF_SHARE_PTR<NFIRecordManager> pRecordManager(NF_NEW NFRecordManager(ident));

        NF_SHARE_PTR<NFIRecord> pConfigRecordInfo = pStaticClassRecordManager->First();
        while (pConfigRecordInfo)
		{
			if (pConfigRecordInfo->GetSave())
			{
				NF_SHARE_PTR<NFRecord> xRecord = dynamic_pointer_cast<NFRecord>(pRecordManager->AddRecord(ident,
					pConfigRecordInfo->GetName(),
					pConfigRecordInfo->GetColData(),
					pConfigRecordInfo->GetMaxRows()));

				xRecord->SetPublic(pConfigRecordInfo->GetPublic());
				xRecord->SetPrivate(pConfigRecordInfo->GetPrivate());
				xRecord->SetSave(pConfigRecordInfo->GetSave());
			}

            pConfigRecordInfo = pStaticClassRecordManager->Next();
        }

        return pRecordManager;
    }

    return NF_SHARE_PTR<NFIRecordManager>(NULL);
}

bool CCommonRedis::ConvertVectorToPropertyManager(string_pair_vector& values, NF_SHARE_PTR<NFIPropertyManager> pPropertyManager)
{
	for (int i = values.size() -1; i >= 0; --i)
	{
		const std::string& strKey = values[i].first;
		const std::string& strValue = values[i].second;
		if (strValue.empty())
		{
			continue;
		}

		NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->GetElement(strKey);
		if (pProperty == nullptr || !pProperty->GetSave())
		{
			continue;
		}

		if (!pProperty->FromString(strValue))
		{
			//TODO
			//error
		}

		values.erase(values.begin() + i);
	}
		
    return true;
}

bool CCommonRedis::ConvertVectorToRecordManager(string_pair_vector& values, NF_SHARE_PTR<NFIRecordManager> pRecordManager)
{
	for (int i = values.size() - 1; i >= 0; --i)
	{
		const std::string& strKey = values[i].first;
		const std::string& strValue = values[i].second;
		if (strValue.empty())
		{
			continue;
		}

		NF_SHARE_PTR<NFIRecord> pRecord = pRecordManager->GetElement(strKey);
		if (pRecord == nullptr || !pRecord->GetSave())
		{
			continue;
		}

		OuterMsg::Record xRecordData;
		if (xRecordData.ParseFromString(strValue))
		{
			PBConvert::ToNFRecord(&xRecordData, pRecord.get());
		}
		else
		{
			//TODO
			//error
		}

		values.erase(values.begin() + i);
	}

    return true;
}

bool CCommonRedis::ConvertPropertyManagerToVector(NF_SHARE_PTR<NFIPropertyManager> pPropertyManager, std::vector<std::string>& vKeyList, std::vector<std::string>& vValueList)
{
	for (NF_SHARE_PTR<NFIProperty> pProperty = pPropertyManager->First(); pProperty != NULL; pProperty = pPropertyManager->Next())
	{
		const int nType = pProperty->GetType();
		if (!pProperty->GetSave())
		{
			continue;
		}

		const std::string& strPropertyName = pProperty->GetName();
		const std::string& strPropertyValue = pProperty->ToString();

		vKeyList.push_back(strPropertyName);
		vValueList.push_back(strPropertyValue);
	}

	return true;
}

bool CCommonRedis::ConvertRecordManagerToVector(NF_SHARE_PTR<NFIRecordManager> pRecordManager, std::vector<std::string>& vKeyList, std::vector<std::string>& vValueList)
{
	for (NF_SHARE_PTR<NFIRecord> pRecord = pRecordManager->First(); pRecord != NULL; pRecord = pRecordManager->Next())
	{
		if (!pRecord->GetSave() || pRecord->IsEmpty())
		{
			continue;
		}

		OuterMsg::Record xRecordData;
		PBConvert::ToPBRecord(pRecord.get(), &xRecordData);

		std::string strValue;
		if (!xRecordData.SerializeToString(&strValue))
		{
			continue;
		}

		vKeyList.push_back(xRecordData.name());
		vValueList.push_back(strValue);
	}

	return true;
}
