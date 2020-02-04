///--------------------------------------------------------------------
/// 文件名:		WorldServerLogicModule.h
/// 内  容:		world作为服务器逻辑模块
/// 说  明:		
/// 创建日期:	2019年9月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#ifndef _WORLD_SERVER_LOGIC_MODULE_H__
#define _WORLD_SERVER_LOGIC_MODULE_H__

#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "WorldPlayers.h"
#include "NFComm/NetPlugin/NetClientModule.h"
#include "NFComm/NetPlugin/NetServerInsideModule.h"

class CWorldServerLogicModule : public NFIModule
{
public:
	CWorldServerLogicModule(NFIPluginManager* p);
	~CWorldServerLogicModule();

	virtual bool Init();
	virtual bool AfterInit();

	virtual const InsideServerData* GetSuitProxyForEnter();
private:
	//收Command消息转发请求
	void OnCommandTransmit(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

	//网络通信测试
	void OnNetTest(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
	void OnNetTestAck(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);
private:
	NetClientModule* m_pNetClientModule = nullptr;
	NFILogModule* m_pLogModule = nullptr;
	CWorldPlayers* m_pWorldPlayers = nullptr;
	NetServerInsideModule* m_pNetServerInsideModule = nullptr;
};

#endif 