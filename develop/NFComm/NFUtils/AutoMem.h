#ifndef _PUBLIC_AUTOMEM_H
#define _PUBLIC_AUTOMEM_H

#include <crtdbg.h>

#ifdef _DEBUG
#define NEW ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW ::new
#endif

// TAutoMem
// 自动管理的内存，可根据需要使用局部栈或堆
template<typename TYPE, size_t SIZE>
class TAutoMem
{
public:
	explicit TAutoMem(size_t len)
	{
		if (len > SIZE)
		{
			m_size = len;
			m_pMem = NEW TYPE[len];
		}
		else
		{
			m_size = SIZE;
			m_pMem = m_stack;
		}
	}

	~TAutoMem()
	{
		if (m_pMem != m_stack)
		{
			delete[] m_pMem;
		}
	}

	TYPE* GetBuffer()
	{
		return m_pMem;
	}

private:
	TAutoMem();
	TAutoMem(const TAutoMem&);
	TAutoMem& operator=(const TAutoMem&);

private:
	TYPE m_stack[SIZE];
	TYPE* m_pMem;
	size_t m_size;
};

#endif // _PUBLIC_AUTOMEM_H
