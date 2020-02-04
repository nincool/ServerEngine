///--------------------------------------------------------------------
/// �ļ���:		PlayerRedis.h
/// ��  ��:		������ݿ�
/// ˵  ��:		
/// ��������:	2019��8��27��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef _PLAYER_REDIS_MODULE_H
#define _PLAYER_REDIS_MODULE_H

#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFINoSqlModule.h"
#include "NFComm/NFMessageDefine/OuterMsg.pb.h"
#include "ObjectRedis.h"
#include "CommonRedis.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"
#include "DBServer.h"

class CPlayerRedis : public CObjectRedis
{
public:
	CPlayerRedis(NFIPluginManager* p) : CObjectRedis(p)
	{
	}

	virtual bool Init();
	virtual bool AfterInit();

	virtual bool ExistPlayerName(const std::string& strPlayerName, int nServerID);
	//�����ݿⴴ����ɫ
	virtual OuterMsg::EGameEventCode CreateRole(OuterMsg::ReqCreateRole& createRoleData, int nServerID);
	//����
	virtual bool Rename(const std::string& strAccount, const NFGUID& self, const std::string& oldName, const std::string& newName, int nServerID);
	
protected:
	void OnRequireRoleListProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnPreCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnCreateRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnDeleteRoleGameProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnLoadRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnSaveRoleDataProcess(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	//����
	void OnRename(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ȡ���PB����
	void OnGetPlayerData(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ȡ�������ID������
	void OnGetPlayerGuidName(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	// ��ȡ��Ҫ���ı��������
	bool GetFrankData(const OuterMsg::ObjectDataPack& xMsg, string_vector& vec_key, string_vector& vec_value);
private:
	NFINoSqlModule* m_pNoSqlModule = nullptr;
	ICommonRedis* m_pCommonRedis = nullptr;
	NFIKernelModule* m_pKernelModule = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
	NetClientModule* m_pNetClient = nullptr;
	CDBServer* m_pDBServer = nullptr;
};

#endif
