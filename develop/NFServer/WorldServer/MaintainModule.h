///--------------------------------------------------------------------
/// 文件名:		MaintainModule.h
/// 内  容:		维护模块
/// 说  明:		
/// 创建日期:	2019年12月11日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef __H_MaintainModule_H__
#define __H_MaintainModule_H__
#include "NFComm/NFPluginModule/NFIModule.h"
#include "WorldServer.h"
#include "NFComm/NetPlugin/NetServerMaintainModule.h"
#include <map>
#include <string>
#include <vector>
#include <functional>
#include "WorldServerState.h"
#include "WorldPlayers.h"

class MaintainModule : public NFIModule
{
public:
	MaintainModule(NFIPluginManager* p);
	~MaintainModule();

	virtual bool Init();
	virtual bool AfterInit();
private:
	void OnGetServerList(const NFSOCK nSockIndex, const int nMsgID, const char* msg,
		const uint32_t nLen);

	std::string RunCloseServer(std::vector<std::string>& arg);
	std::string RunGetServerState(std::vector<std::string>& arg);
	std::string RunGetPlayerCount(std::vector<std::string>& arg);
	std::string RunSetClientVersion(std::vector<std::string>& arg);
	std::string RunSetPlayersMax(std::vector<std::string>& arg);

	//白名单
	std::string RunWhiteIPSwitch(std::vector<std::string>& arg);
	std::string RunAddWhiteIP(std::vector<std::string>& arg);
	std::string RunClearWhiteIP(std::vector<std::string>& arg);
	std::string RunIsWhiteIP(std::vector<std::string>& arg);
	

private:
	CWorldServer* m_pWorldServer = nullptr;
	NetServerMaintainModule* m_pMaintainNetServer = nullptr;
	CWorldServerState* m_pWorldServerState = nullptr;
	CWorldPlayers* m_pWorldPlayers = nullptr;
};

#endif //__H_MaintainModule_H__