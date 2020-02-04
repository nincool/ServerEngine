///--------------------------------------------------------------------
/// 文件名:		CommandModule.cpp
/// 内  容:		命令控制
/// 说  明:		
/// 创建日期:	2019年12月19日
/// 创建人:		肖庆军
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------
#include "CommandModule.h"
#include <iostream>
#include "StringUtil.h"
#include <thread>

CommandModule::CommandModule()
{
	Init();
}

CommandModule::~CommandModule()
{
	if (nullptr != m_pNetClient)
	{
		delete m_pNetClient;
	}
}

bool CommandModule::Init()
{
	mNetCallback.AddEventCallBack(this, (NetEventFun)&CommandModule::OnSocketEvent);
	mNetCallback.AddReceiveCallBack(this, (NetMsgFun)&CommandModule::OnMsg);

	mConnectFunMap["connect"] = std::bind(&CommandModule::RunConnect, this, std::placeholders::_1);
	mConnectFunMap["send"] = std::bind(&CommandModule::RunSendMsg, this, std::placeholders::_1);

	return true;
}

bool CommandModule::Connect(const char* strIP, const unsigned short nPort)
{
	if (nullptr != m_pNetClient)
	{
		delete m_pNetClient;
		m_pNetClient = nullptr;
	}
	m_pNetClient = new NetClient(&mNetCallback, 1024 * 8);
	mConnectIng = true;
	if (-1 == m_pNetClient->Connect(strIP, nPort))
	{
		mConnectIng = false;
		std::cout << "connect error." << std::endl;
		return false;
	}


	return true;
}

bool CommandModule::Execute()
{ 
	if (nullptr != m_pNetClient)
	{
		m_pNetClient->Execute();
	}

	return true;
}

bool CommandModule::Run(std::string& command)
{
	if (command.empty())
	{
		return true;
	}

	std::vector<std::string> vec = StringUtil::SplitString(command, " ");

	auto it = mConnectFunMap.find(vec[0]);
	if (mConnectFunMap.end() == it)
	{
		RunNetCommand(command);
	}
	else
	{
		it->second(command);
	}

	return true;
}

void CommandModule::OnSocketEvent(const __int64 nSockIndex, const NET_EVENT eEvent)
{
	mConnectIng = false;
	if (eEvent == NET_EVENT::NET_EVENT_CONNECTED)
	{
		std::cout << "connect ok" << std::endl;
	}
	else
	{
		std::cout << "connect disconnect" << std::endl;

		delete m_pNetClient;
		m_pNetClient = nullptr;
	}
}

void CommandModule::OnMsg(const __int64 nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen)
{
	std::cout << std::string(msg, nLen) << std::endl;
}

void CommandModule::RunConnect(std::string& command)
{
	std::vector<std::string> vec = StringUtil::SplitString(command, " ");
	if (vec.size() < 3)
	{
		std::cout << "command parameter error." << std::endl;
		return;
	}
	Connect(vec[1].c_str(), StringUtil::StringAsInt(vec[2].c_str()));

	while (mConnectIng == true)
	{
		Execute();
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
}

void CommandModule::RunNetCommand(std::string& command)
{
	if (nullptr == m_pNetClient)
	{
		std::cout << "connect disconnect." << std::endl;
		return;
	}

	m_pNetClient->SendMsg((int16_t)24, command.c_str(), (uint32_t)command.length());
}

void CommandModule::RunSendMsg(std::string& command)
{
	if (nullptr == m_pNetClient)
	{
		std::cout << "connect disconnect." << std::endl;
		return;
	}

	std::vector<std::string> vec = StringUtil::SplitString(command, " ");
	if (vec.size() < 3)
	{
		std::cout << "command parameter error." << std::endl;
		return;
	}

	m_pNetClient->SendMsg((int16_t)StringUtil::StringAsInt(vec[1].c_str()), vec[2].c_str(), (uint32_t)vec[2].length());
}