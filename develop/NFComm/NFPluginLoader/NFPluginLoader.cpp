///--------------------------------------------------------------------
/// 文件名:		NFPluginLoader.h
/// 内  容:		模块加载
/// 说  明:		
/// 创建日期:	2019年10月31日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <iostream>
#include <utility>
#include <thread>
#include <chrono>
#include <future>
#include <functional>
#include <atomic>
#include "NFPluginManager.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

#include "NFComm/system/WinMutex.h"
#include "prometheus/exposer.h"
#include "prometheus/counter.h"
#include "../NFPluginModule/KConstDefine.h"

std::thread gThread;
std::vector<std::thread> gExporterThreadVec;
std::string strArgvList;
std::string strServerID;
std::string strTitleName;

//退出状态
bool gIsExit = false;
//服务器插件
std::vector<NFPluginManager*> mPluginManagerVec;

void MainExecute();

void ReleaseNF()
{
	int size = (int)mPluginManagerVec.size();
	for (int i = 0; i < size; ++i)
	{
		mPluginManagerVec[i]->BeforeShut();
		
	}
	for (int i = 0; i < size; ++i)
	{
		mPluginManagerVec[i]->Shut();
	}
	for (int i = 0; i < size; ++i)
	{
		mPluginManagerVec[i]->Finalize();
	}

	gIsExit = true;
}

//处理dmp文件
#pragma comment( lib, "DbgHelp" )
bool ApplicationCtrlHandler(DWORD fdwctrltype)
{
	switch (fdwctrltype)
	{
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	{
		int size = (int)mPluginManagerVec.size();
		for (int i = 0; i < size; ++i)
		{
			mPluginManagerVec[i]->SetExit(true);
		}
		gIsExit = true;
	}
	return true;
	default:
		return false;
	}
}

void CreateDumpFile(const std::string& strDumpFilePathName, EXCEPTION_POINTERS* pException)
{
	//Dump
	HANDLE hDumpFile = CreateFile(strDumpFilePathName.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);

	CloseHandle(hDumpFile);
}

long ApplicationCrashHandler(EXCEPTION_POINTERS* pException)
{
	time_t t = time(0);
	char szDmupName[MAX_PATH];
	tm* ptm = localtime(&t);

	sprintf_s(szDmupName, "%04d_%02d_%02d_%02d_%02d_%02d.dmp", ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	CreateDumpFile(szDmupName, pException);

	FatalAppExit(-1, szDmupName);

	return EXCEPTION_EXECUTE_HANDLER;
}

void CloseXButton()
{
	HWND hWnd = GetConsoleWindow();
	if (hWnd)
	{
		HMENU hMenu = GetSystemMenu(hWnd, FALSE);
		EnableMenuItem(hMenu, SC_CLOSE, MF_DISABLED | MF_BYCOMMAND);
	}
}

void ThreadFunc()
{
	while (!gIsExit)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		std::string s;
		std::getline(std::cin, s);
		if (0 == stricmp(s.c_str(), "exit"))
		{
			//特殊处理的关服事件
			int size = (int)mPluginManagerVec.size();
			for (int i = 0; i < size; ++i)
			{
				mPluginManagerVec[i]->CommandShut();
				gIsExit = mPluginManagerVec[i]->GetExit();
			}
			gThread.detach();
		}
	}
}

void CreateBackThread()
{
	gThread = std::thread(std::bind(&ThreadFunc));
	auto f = std::async(std::launch::deferred, std::bind(ThreadFunc));
	std::cout << "CreateBackThread, thread ID = " << gThread.get_id() << std::endl;
}

void ThreadExporterFunc(std::string& strIP, NFPluginManager* pPluginManager)
{
	// 创建exporter
	prometheus::Exposer exposer(strIP);
	// 创建注册器
	auto registry = NF_MAKE_SPTR<prometheus::Registry>();
	pPluginManager->SetMetricsRegistry(registry);
	//初始化所有exporter
	pPluginManager->InitExporter();
	exposer.RegisterCollectable(registry);

	while (!gIsExit)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void CreateExporter()
{
	int size = (int)mPluginManagerVec.size();
	for (int i = 0; i < size; ++i)
	{
		std::string strTemp = mPluginManagerVec[i]->GetMetricsInfo();
		if (strTemp.empty())
		{
			return;
		}

		//加载exporter信息
		mPluginManagerVec[i]->LoadMetricsConfig();
	
		gExporterThreadVec.push_back(std::thread(std::bind(&ThreadExporterFunc, strTemp, mPluginManagerVec[i])));
		auto f = std::async(std::launch::deferred, std::bind(ThreadExporterFunc, strTemp, mPluginManagerVec[i]));
		
		std::cout << "CreateExporter, thread ID = " << gExporterThreadVec[gExporterThreadVec.size() - 1].get_id() << std::endl;
	}
}

void PrintfLogo()
{
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin, &mode);
	mode &= ~ENABLE_QUICK_EDIT_MODE;  //移除快速编辑模式
	mode &= ~ENABLE_INSERT_MODE;      //移除插入模式
	mode &= ~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin, mode);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

vector<string> SplitString(const string& str, const string& delim)
{
	regex re{ delim };
	return vector<string>{
		sregex_token_iterator(str.begin(), str.end(), re, -1),
			sregex_token_iterator()
	};
}

void ProcessParameter(int argc, char* argv[])
{
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ApplicationCtrlHandler, true);

	//关闭菜单按钮
	CloseXButton();

	//创建加载器
	for (int i = 0; i < argc; i++)
	{
		strArgvList.append(" ");
		strArgvList.append(argv[i]);

		std::vector<string>& vec = SplitString(argv[i], "=");
		if (vec.size() == 2 && vec[0] == "Server")
		{
			NFPluginManager* pPM = new NFPluginManager();
			pPM->SetConfigName(vec[1]);
			pPM->GetFileConfig();
			mPluginManagerVec.push_back(pPM);
		}
	}

	if (mPluginManagerVec.empty())
	{
		std::cout << "null Server!!" << std::endl;
		return;
		}

	// 获取服务ID
	if (strArgvList.find("ServerID=") != string::npos)
	{
		for (int i = 0; i < argc; i++)
		{
			strServerID = argv[i];
			if (strServerID.find("ServerID=") != string::npos)
			{
				strServerID.erase(0, 9);
				break;
			}
		}

		int size = (int)mPluginManagerVec.size();
		for (int i = 0; i < size; ++i)
		{
			mPluginManagerVec[i]->SetServerID(lexical_cast<int>(strServerID));
		}
	}
	else
	{
		strServerID = lexical_cast<std::string>(mPluginManagerVec[0]->GetServerID());
	}


	int size = (int)mPluginManagerVec.size();
	for (int i = 0; i < size; ++i)
	{
		strTitleName = mPluginManagerVec[0]->GetConfigName() + "_" + strServerID + "_" + lexical_cast<std::string>(mPluginManagerVec[0]->GetAppID());
		strTitleName.replace(strTitleName.find("Server"), 6, "");
		mPluginManagerVec[i]->SetTitleName(strTitleName);
	}

	if (mPluginManagerVec.size() > 1)
	{
		std::string n = "MultipleServer_" + strServerID;
		SetConsoleTitle(n.c_str());
	}
	else
	{
		SetConsoleTitle(strTitleName.c_str());
	}
}

void MainExecute()
{
	while (true)
	{
#ifdef NF_DEBUG_MODE
		std::this_thread::sleep_for(std::chrono::microseconds(10));
#endif
		
			int size = (int)mPluginManagerVec.size();
			for (int i = 0; i < size; ++i)
			{
				if (!mPluginManagerVec[i]->GetExit())
				{
					mPluginManagerVec[i]->Execute();
				}
				else
				{
					return;
				}
			}
	
	}
}

void Start()
{
	__try
	{
		PrintfLogo();
		CreateBackThread();
		//开启prometheus
		CreateExporter();

		int size = (int)mPluginManagerVec.size();
		for (int i = 0; i < size; ++i)
		{
			mPluginManagerVec[i]->LoadPlugin();
			mPluginManagerVec[i]->Awake();
			mPluginManagerVec[i]->Init();
			mPluginManagerVec[i]->InitLua();
			mPluginManagerVec[i]->AfterInit();
			mPluginManagerVec[i]->InitNetServer();
			mPluginManagerVec[i]->InitNetClient();
			mPluginManagerVec[i]->ReadyExecute();
		}

		MainExecute();

	}
	__except (ApplicationCrashHandler(GetExceptionInformation()))
	{
	}
}

int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
	ProcessParameter(argc, argv);

	// 进程唯一
	std::string strMutexApp = "hd_mutex_" + strTitleName;
	if (!CMutex::Exclusive(strMutexApp.c_str()))
	{
		return 0;
	}
	
	Start();

	ReleaseNF();

	if (gThread.joinable())
	{
		gThread.detach();
	}

	for (int i = 0; i < (int)gExporterThreadVec.size(); ++i)
	{
		if (gExporterThreadVec[i].joinable())
		{
			gExporterThreadVec[i].detach();
		}
	}

	exit(0);

	return 0;
}
