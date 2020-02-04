///--------------------------------------------------------------------
/// 文件名:		PlayerRenameModule.h
/// 内  容:		改名
/// 说  明:		
/// 创建日期:	2019年9月7日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_PlayerRenameModule_H__
#define __H_PlayerRenameModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "CreateRoleModule.h"
#include "NFComm/NetPlugin/NetClientModule.h"

class PlayerRenameModule : public NFIModule
{
public:
	PlayerRenameModule(NFIPluginManager* p);
	virtual bool Init();
	virtual bool AfterInit();

	bool PlayerRename(const NFGUID& self, const std::string& strName);

private:
	void OnPlayerRename(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	NetClientModule* m_pNetClientModule = nullptr;
	//通用内核模块
	NFIKernelModule* m_pKernelModule = nullptr;
	ICreateRoleModule* m_pCreateRoleModule = nullptr;
	NFIClassModule* m_pNFIClassModule = nullptr;
};

#endif //__H_PlayerRenameModule_H__