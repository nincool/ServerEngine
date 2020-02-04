///--------------------------------------------------------------------
/// �ļ���:		WinMutex.h
/// ��  ��:		Windows���̻�����
/// ˵  ��:		
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __WIN_MUTEX_H__
#define __WIN_MUTEX_H__

#include <windows.h>
#include <string.h>

// ���̻�����

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

	// �Ƿ����ָ�����ֵĻ�����
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

	// ��֤Ψһ
	static bool Exclusive(const char* name)
	{
		HANDLE hMutex = CreateMutex(NULL, FALSE, name);

		if (NULL == hMutex)
		{
			return false;
		}

		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			// �Ѿ�����
			return false;
		}

		// Ϊ�˱�֤�������ɹرջ�����
		return true;
	}

	// �������
	const std::string& GetName() const
	{	
		return m_strName;
	}
	
	// �Ƿ���Ч
	bool IsValid() const
	{
		return (m_hMutex != nullptr);
	}
	
	// ����������
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

	// ɾ����
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

	// ����
	void Lock()
	{
		WaitForSingleObject(m_hMutex, INFINITE);
	}

	// ����
	void Unlock()
	{
		ReleaseMutex(m_hMutex);
	}

private:
	std::string m_strName = "";
	HANDLE m_hMutex = nullptr;
};

#endif // __WIN_MUTEX_H__
