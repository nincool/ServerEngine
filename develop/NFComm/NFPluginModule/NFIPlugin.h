///--------------------------------------------------------------------
/// 文件名:		NFIPlugin.h
/// 内  容:		插件接口
/// 说  明:		
/// 创建日期:	2019年8月10日
/// 创建人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NFI_PLUGIN_H
#define NFI_PLUGIN_H

#include <iostream>
#include <assert.h>
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFPluginModule/NFIModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

#define REGISTER_MODULE(pManager, classBaseName, className)  \
	assert((TIsDerived<classBaseName, NFIModule>::Result));	\
	assert((TIsDerived<className, classBaseName>::Result));	\
	NFIModule* pRegisterModule##className= new className(pManager); \
    pRegisterModule##className->strName = (#className); \
    pManager->AddModule( #classBaseName, pRegisterModule##className );\
    AddElement( #classBaseName, pRegisterModule##className );

#define UNREGISTER_MODULE(pManager, classBaseName, className) \
    NFIModule* pUnRegisterModule##className = dynamic_cast<NFIModule*>( pManager->FindModule( #classBaseName )); \
	pManager->RemoveModule( #classBaseName ); \
    RemoveElement( #classBaseName ); \
    delete pUnRegisterModule##className;

#define CREATE_PLUGIN(pManager, className)  NFIPlugin* pCreatePlugin##className = new className(pManager); pManager->Registered( pCreatePlugin##className );

#define DESTROY_PLUGIN(pManager, className) pManager->UnRegistered( pManager->FindPlugin((#className)) );

class NFIPluginManager;

class NFIPlugin : public NFIModule, public NFMap<std::string, NFIModule>
{
public:
	NFIPlugin()
	{
	}

    virtual const int GetPluginVersion() = 0;

    virtual const std::string GetPluginName() = 0;

    virtual void Install() = 0;

	virtual bool Awake()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);

			bool bRet = pModule->Awake();
			if (!bRet)
			{
				std::cout << pModule->strName << std::endl;
				assert(0);
			}

			pModule = Next();
		}
		return true;
	}

    virtual bool Init()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
			pPluginManager->SetCurrenModule(pModule);
            bool bRet = pModule->Init();
            if (!bRet)
            {
				std::cout << pModule->strName << std::endl;
                assert(0);
            }

            pModule = Next();
        }
        return true;
    }

	virtual bool InitLua()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);
			bool bRet = pModule->InitLua();
			if (!bRet)
			{
				std::cout << pModule->strName << std::endl;
				assert(0);
			}

			pModule = Next();
		}

		return true;
	}

    virtual bool AfterInit()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
			pPluginManager->SetCurrenModule(pModule);
            bool bRet = pModule->AfterInit();
            if (!bRet)
            {
				std::cout << pModule->strName << std::endl;
                assert(0);
            }

            pModule = Next();
        }
        return true;
    }
	virtual bool InitNetServer()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);
			bool bRet = pModule->InitNetServer();
			if (!bRet)
			{
				std::cout << pModule->strName << std::endl;
				assert(0);
			}

			pModule = Next();
		}

		return true;
	}

	virtual bool InitNetClient()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);
			bool bRet = pModule->InitNetClient();
			if (!bRet)
			{
				std::cout << pModule->strName << std::endl;
				assert(0);
			}

			pModule = Next();
		}

		return true;
	}

	virtual bool ReadyExecute()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);
			pModule->ReadyExecute();

			pModule = Next();
		}

		return true;
	}

    virtual bool Execute()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
			pPluginManager->SetCurrenModule(pModule);
            pModule->Execute();

            pModule = Next();
        }

        return true;
    }

    virtual bool BeforeShut()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
			pPluginManager->SetCurrenModule(pModule);
            pModule->BeforeShut();

            pModule = Next();
        }
        return true;
    }

    virtual bool Shut()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
			pPluginManager->SetCurrenModule(pModule);
            pModule->Shut();

            pModule = Next();
        }

        return true;
    }

    virtual bool Finalize()
    {
        NFIModule* pModule = First();
        while (pModule)
        {
			pPluginManager->SetCurrenModule(pModule);
            pModule->Finalize();

            pModule = Next();
        }

        return true;
    }

	virtual bool CommandShut()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);
			pModule->CommandShut();

			pModule = Next();
		}

		return true;
	}

	virtual bool OnReloadPlugin()
	{
		NFIModule* pModule = First();
		while (pModule)
		{
			pPluginManager->SetCurrenModule(pModule);
			pModule->OnReloadPlugin();

			pModule = Next();
		}

		return true;
	}

    virtual void Uninstall() = 0;
};

#endif
