///--------------------------------------------------------------------
/// �ļ���:		NetServerMaintainModule.cpp
/// ��  ��:		ά���õ��������ģ��
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "NetServerMaintainModule.h"
#include "NFComm/NFMessageDefine/OuterMsgDefine.h"
#include "NFComm/NFUtils/StringUtil.h"

NetServerMaintainModule::NetServerMaintainModule(NFIPluginManager* p)
{
	pPluginManager = p;
	mClassName = "NetServerMaintainModule";
}

bool NetServerMaintainModule::Init()
{
	AddReceiveCallBack(OuterMsg::RUN_NET_COMMAND, this, (NetMsgFun)&NetServerMaintainModule::OnCommandMsg);

	return true;
}

bool NetServerMaintainModule::InitNet(const size_t bufferMax,
	const int nMaxClient, const std::string& ip, const int port)
{
	return NetServerModuleBase::Initialization(bufferMax, bufferMax, nMaxClient, false, ip, port);
}

void NetServerMaintainModule::AddCommandCallBack(const std::string& commName, NFIModule* pBase, MaintainCBFun fun)
{
	mCommandFunMap[commName] = std::bind(fun, pBase, std::placeholders::_1);
}

void NetServerMaintainModule::OnCommandMsg(const NFSOCK nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	std::vector<std::string> vec = StringUtil::SplitString(std::string(msg, nLen), " ");
	if (vec.empty())
	{
		return;
	}

	auto it = mCommandFunMap.find(vec[0]);
	if (mCommandFunMap.end() == it)
	{
		SendMsg(OuterMsg::RUN_NET_COMMAND, "command does not exist.", nSockIndex);
		return;
	}

	const std::string& reply = it->second(vec);
	SendMsg(OuterMsg::RUN_NET_COMMAND, reply, nSockIndex);
}