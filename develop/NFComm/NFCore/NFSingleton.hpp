///--------------------------------------------------------------------
/// 文件名:		NFSingleton.cpp
/// 内  容:		单例
/// 说  明:		
/// 创建日期:	2019.8.10
/// 修改人:		王伟明
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef NF_SINGLETON_H
#define NF_SINGLETON_H

#include <iostream>

template <class T>
class NFSingleton
{
public:
    NFSingleton()
    {
        m_pInstance = static_cast< T* >(this);
    }

    ~NFSingleton()
    {
        m_pInstance = NULL;
    }

public:
    static T* GetSingletonPtr()
    {
        if (NULL == m_pInstance)
        {
            m_pInstance = new T;
        }

        return m_pInstance;
    }

    static T& GetSingletonRef()
    {
        return *GetSingletonPtr();
    }

    static T* Instance()
    {
        return GetSingletonPtr();
    }

    static T& InstanceRef()
    {
        return GetSingletonRef();
    }

    static void ReleaseInstance()
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }

private:
    static T* m_pInstance;
};

template <class T>
T* NFSingleton<T>::m_pInstance = NULL;

#endif