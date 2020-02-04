///--------------------------------------------------------------------
/// �ļ���:		NFIModule.h
/// ��  ��:		ģ��ӿ�
/// ˵  ��:		
/// ��������:	2019��8��10��
/// ������:		��ΰ��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef NFI_MODULE_H
#define NFI_MODULE_H

#include <string>
#include "NFIPluginManager.h"
#include "NFComm/NFCore/NFMap.hpp"
#include "NFComm/NFCore/NFList.hpp"
#include "NFComm/NFCore/NFDataList.hpp"

class NFIModule
{

public:
    NFIModule()
    {
    }

    virtual ~NFIModule() {}

    virtual bool Awake()
    {
        return true;
    }

    virtual bool Init()
    {
        return true;
    }

	virtual bool InitLua()
	{
		return true;
	}

    virtual bool AfterInit()
    {
        return true;
    }

	virtual bool InitNetServer()
	{
		return true;
	}

	virtual bool InitNetClient()
	{
		return true;
	}

    virtual bool ReadyExecute()
    {
        return true;
    }

    virtual bool Execute()
    {
        return true;
    }

    virtual bool BeforeShut()
    {
        return true;
    }

    virtual bool Shut()
    {
        return true;
    }

    virtual bool Finalize()
    {
        return true;
    }

	virtual bool CommandShut()
	{
		return true;
	}

	virtual bool OnReloadPlugin()
	{
		return true;
	}

    virtual NFIPluginManager* GetPluginManager() const
    {
        return pPluginManager;
    }

    std::string strName;

protected:
	NFIPluginManager* pPluginManager = NULL;
};
#endif
