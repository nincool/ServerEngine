#include "StructDefine.h"
#include <iostream>
#include <chrono>

#include "NFComm/NFPluginModule/NFPlatform.h"

int main(int argc, const char *argv[])
{
	StructDefine s;

	if (argc > 2)
	{
		s.ParseClass(std::string(argv[1]), std::string(argv[2]));
		s.Save("LogicClassDefine", "LC");
	}
	else
	{
		std::cout << "param error" << endl;
	}

	system("pause");
	return 0;
}
