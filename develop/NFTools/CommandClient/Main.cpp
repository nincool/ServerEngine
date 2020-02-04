///--------------------------------------------------------------------
/// �ļ���:		Main.cpp
/// ��  ��:		����ͻ���
/// ˵  ��:		
/// ��������:	2019��11��5��
/// ������:		Ф���
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------
#include "CommandModule.h"
#include <thread>
#include <iostream>
#include "StringUtil.h"

static bool bExit = false;
CommandModule* m_pCommandModule = nullptr;

void ThreadFunc()
{
	while (bExit == false)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));

		std::string s;
		std::getline(std::cin, s);
		if (s == "exit")
		{
			bExit = true;
			return;
		}

		m_pCommandModule->Run(s);
	}
}

int main(int argc, const char* argv[])
{
	m_pCommandModule = new CommandModule();

	std::vector<std::string> vec;
	for (int i = 1; i < argc; i++)
	{
		vec.push_back(argv[i]);
	}
	size_t size = vec.size();
	for (size_t i = 0; i < size; ++i)
	{
		if ("exit" == vec[i])
		{
			bExit = true;
		}
		else
		{
			std::cout << vec[i] << std::endl;
			m_pCommandModule->Run(vec[i]);
		}
	}

	std::thread t1(ThreadFunc);

	while (bExit == false)
	{
		std::this_thread::sleep_for(std::chrono::microseconds(10));

		m_pCommandModule->Execute();
	}

	t1.join();
	delete m_pCommandModule;

	return 0;
}