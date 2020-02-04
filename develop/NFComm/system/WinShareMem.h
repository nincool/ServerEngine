///--------------------------------------------------------------------
/// �ļ���:		ShareMem.cpp
/// ��  ��:		���̼乲���ڴ�
/// ˵  ��:		
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __SHARE_MEMORY_H__
#define __SHARE_MEMORY_H__

#include <windows.h>
#include <string>

class CShareMem final
{
public:
	CShareMem()
	{
	}

	~CShareMem()
	{
		Destroy();
	}

private:
	CShareMem(const CShareMem&) = delete;
	CShareMem& operator=(const CShareMem&) = delete;

public:

	// �������
	const std::string& GetName() const
	{		
		return m_strName.c_str();
	}

	// ��ó���
	size_t GetSize() const
	{
		return m_nSize;
	}

	// ����ڴ��ַ
	void* GetMem() const
	{
		return m_pMem;
	}

	// �Ƿ���Ч
	bool IsValid() const
	{
		return (m_hFile != INVALID_HANDLE_VALUE) && (m_pMem != nullptr);
	}
	
	// ��û򴴽������ڴ�
	bool Create(const char* name, size_t size)
	{
		m_strName = name;
		m_nSize = size;

		m_hFile = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, DWORD(m_nSize), name);
		if (m_hFile == INVALID_HANDLE_VALUE || m_hFile == 0)
		{
			m_hFile = INVALID_HANDLE_VALUE;
			m_nSize = 0;
			return false;
		}

		m_pMem = MapViewOfFile(m_hFile, FILE_MAP_ALL_ACCESS, 0, 0, m_nSize);
		if (m_pMem == nullptr)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
			m_nSize = 0;
			return false;
		}
		
		return true;
	}

	// ɾ�������ڴ�
	bool Destroy()
	{
		if (m_pMem != nullptr)
		{
			UnmapViewOfFile(m_pMem);
			m_pMem = nullptr;
		}

		if (m_hFile != INVALID_HANDLE_VALUE)
		{
			if (!CloseHandle(m_hFile))
			{
				return false;
			}
	
			m_hFile = INVALID_HANDLE_VALUE;
		}
		
		return true;
	}

private:
	std::string m_strName = "";	// ����
	size_t m_nSize = 0;			// ����
	HANDLE m_hFile = INVALID_HANDLE_VALUE;	// �ļ����
	void* m_pMem = nullptr;	// �����ڴ�
};

#endif // __SHARE_MEMORY_H__

