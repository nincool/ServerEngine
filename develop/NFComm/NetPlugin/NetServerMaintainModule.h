///--------------------------------------------------------------------
/// �ļ���:		NetServerMaintainModule.h
/// ��  ��:		ά���õ��������ģ��
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#ifndef __H_NetServerMaintainModule_H__
#define __H_NetServerMaintainModule_H__
#include "NetServerModuleBase.h"
#include <functional>
#include <vector>
#include <string>

typedef std::string(NFIModule::* MaintainCBFun)(std::vector<std::string>& vec);

class NetServerMaintainModule : public NetServerModuleBase
{
public:
	NetServerMaintainModule(NFIPluginManager* p);

	virtual bool Init();

	virtual bool InitNet(const size_t bufferMax,
		const int nMaxClient, const std::string& ip, const int port);

	virtual void AddCommandCallBack(const std::string& commName, NFIModule* pBase, MaintainCBFun fun);

private:
	void OnCommandMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen);

private:
	std::map<std::string, std::function<std::string(std::vector<std::string>&)>> mCommandFunMap;
};

#endif //__H_NetServerMaintainModule_H__