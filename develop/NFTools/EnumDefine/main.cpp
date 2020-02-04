#include "EnumDefine.h"
#include <iostream>
#include <chrono>

#include "NFComm/NFPluginModule/NFPlatform.h"

int main(int argc, const char *argv[])
{
	EnumDefine e;

	if (argc > 1)
	{
		e.LoadDataFromExcel(std::string(argv[1]));
	}
	else
	{
		std::cout << "file empty" << std::endl;
	}

	if (argc > 4)
	{
		e.SaveForCPP(argv[2], argv[4]);
		e.SaveForCS(argv[3], argv[4]);
	}
	else
	{
		std::string file = "";
		std::string space = "Msg";
		e.SaveForCPP(file, space);
		e.SaveForCS(file, space);
	}

	return 0;
}
