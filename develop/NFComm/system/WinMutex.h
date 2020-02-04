///--------------------------------------------------------------------
/// 文件名:		WinMutex.h
/// 内  容:		Windows进程互斥锁
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
///--------------------------------------------------------------------

#ifndef __WIN_MUTEX_H__
#define __WIN_MUTEX_H__

#include <windows.h>
#include <string.h>

// 进程互斥锁

class CMutex
{
private:
	CMutex(const CMutex&) = delete;
	CMutex& operator=(const CMutex&) = delete;
public:
	CMutex()
	{
	}

	~CMutex()
	{
		Destroy();
	}

	// 是否存在指定名字的互斥锁
	static bool Exists(const char* name)
	{
		HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, name);

		if (NULL == mutex)
		{
			return false;
		}

		CloseHandle(mutex);

		return true;
	}

	// 保证唯一
	static bool Exclusive(const char* name)
	{
		HANDLE hMutex = CreateMutex(NULL, FALSE, name);

		if (NULL == hMutex)
		{
			return false;
		}

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// 已经存在
			return false;
		}

		// 为了保证锁定不可关闭互斥体
		return true;
	}

	// 获得名字
	const std::string& GetName() const
	{	
		return m_strName;
	}
	
	// 是否有效
	bool IsValid() const
	{
		return (m_hMutex != nullptr);
	}
	
	// 创建或获得锁
	bool Create(const char* name)
	{
		m_strName = name;
		m_hMutex = CreateMutex(NULL, FALSE, name);
		if (m_hMutex == nullptr)
		{
			return false;
		}

		return true;
	}

	// 删除锁
	bool Destroy()
	{
		if (!CloseHandle(m_hMutex))
		{
			return false;
		}

		m_strName = "";
		m_hMutex = nullptr;
		
		return true;
	}

	// 加锁
	void Lock()
	{
		WaitForSingleObject(m_hMutex, INFINITE);
	}

	// 解锁
	void Unlock()
	{
		ReleaseMutex(m_hMutex);
	}

private:
	std::string m_strName = "";
	HANDLE m_hMutex = nullptr;
};

#endif // __WIN_MUTEX_H__
