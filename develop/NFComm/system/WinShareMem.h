///--------------------------------------------------------------------
/// 文件名:		ShareMem.cpp
/// 内  容:		进程间共享内存
/// 说  明:		
/// 创建日期:	2019年11月1日
/// 创建人:		谢宇
/// 版权所有:	血帆海盗团
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

	// 获得名称
	const std::string& GetName() const
	{		
		return m_strName.c_str();
	}

	// 获得长度
	size_t GetSize() const
	{
		return m_nSize;
	}

	// 获得内存地址
	void* GetMem() const
	{
		return m_pMem;
	}

	// 是否有效
	bool IsValid() const
	{
		return (m_hFile != INVALID_HANDLE_VALUE) && (m_pMem != nullptr);
	}
	
	// 获得或创建共享内存
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

	// 删除共享内存
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
	std::string m_strName = "";	// 名称
	size_t m_nSize = 0;			// 长度
	HANDLE m_hFile = INVALID_HANDLE_VALUE;	// 文件句柄
	void* m_pMem = nullptr;	// 共享内存
};

#endif // __SHARE_MEMORY_H__

