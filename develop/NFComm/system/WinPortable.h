///--------------------------------------------------------------------
/// �ļ���:		WinPortable.h
/// ��  ��:		Windows������ز���
/// ˵  ��:		
/// ��������:	2019��11��1��
/// ������:		л��
/// ��Ȩ����:	Ѫ��������
///--------------------------------------------------------------------

#ifndef __WIN_PORTABLE_H__
#define __WIN_PORTABLE_H__

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <objbase.h>

// ���ں�ʱ��
struct port_date_time_t
{
	int nYear;
	int nMonth;
	int nDay;
	int nHour;
	int nMinute;
	int nSecond;
	int nMilliseconds;
	int nDayOfWeek;
};

// �ڴ�ʹ�����
struct port_memory_info_t
{
	double dMemoryLoad;
	double dTotalMemory;
	double dAvailMemory;
};

// ��ñ��ص�ǰʱ��
inline bool Port_GetLocalTime(port_date_time_t* dt)
{
	SYSTEMTIME st;
	
	GetLocalTime(&st);
	
	dt->nYear = st.wYear;
	dt->nMonth = st.wMonth;
	dt->nDay = st.wDay;
	dt->nHour = st.wHour;
	dt->nMinute = st.wMinute;
	dt->nSecond = st.wSecond;
	dt->nMilliseconds = st.wMilliseconds;
	dt->nDayOfWeek = st.wDayOfWeek;
	
	return true;
}

// ���ϵͳ��ʱ
inline unsigned int Port_GetTickCount()
{
	return GetTickCount();
}

// ��õ�ǰ�߾���ʱ��
inline double Port_GetPerformanceTime()
{
	//static LARGE_INTEGER s_u64Freq;
	//static BOOL s_init = QueryPerformanceFrequency(&s_u64Freq);

	//LARGE_INTEGER u64Tick;

	//QueryPerformanceCounter(&u64Tick);

	//return (double)u64Tick.QuadPart / (double)s_u64Freq.QuadPart;
	
	//xenServerƽ̨�����޸�
	return (double)GetTickCount() / (double)1000;
}

// ϵͳ����
inline void Port_Sleep(unsigned int ms)
{
	Sleep(ms);
}

// �Ƿ�Ƿ��ĸ�����
inline int Port_IsNaN(float value)
{
	return _isnan(value);
}
//
//// ����Ψһ��ʶ��
//inline const char* Port_CreateGuid(char* buffer, size_t size)
//{
//	GUID id;
//	
//	if (CoCreateGuid(&id) != S_OK)
//	{
//		return NULL;
//	}
//	
//	_snprintf(buffer, size - 1, 
//		"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X",
//		id.Data1, id.Data2, id.Data3, 
//		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
//		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
//	buffer[size - 1] = 0;
//		
//	return buffer;
//}

// ������Ϣ����
inline void Port_PopupMsgBox(const char* info, const char* title)
{
	MessageBox(NULL, info, title, MB_OK);
}

// ����������Ϣ����
inline void Port_PopupErrorMsgBox(const char* info, const char* title)
{
	MessageBox(NULL, info, title, MB_OK | MB_ICONERROR);
}

// ��õ�ǰ�Ľ���ID
inline unsigned int Port_GetCurrentProcessId()
{
	return GetCurrentProcessId();
}

// ��õ�ǰ���߳�ID
inline unsigned int Port_GetCurrentThreadId()
{
	return GetCurrentProcessId();
}

// ��õ�ǰִ���ļ���·��
inline bool Port_GetCurrentModuleFileName(char* buffer, size_t size)
{
	if (GetModuleFileName(NULL, buffer, (DWORD)size) == 0)
	{
		buffer[0] = 0;
		return false;
	}
	
	return true;
}

// ��ֹ����
inline bool Port_KillProcess(unsigned int proc_id)
{
	HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, proc_id);
	
	if (NULL == hProcess)
	{
		return false;
	}
	
	if (!TerminateProcess(hProcess, 0))
	{
		CloseHandle(hProcess);
		return false;
	}
	
	CloseHandle(hProcess);
	
	return true;
}

// ����ڴ�ʹ�����
inline bool Port_GetMemoryInfo(port_memory_info_t* pInfo)
{
	memset(pInfo, 0, sizeof(port_memory_info_t));
	
	MEMORYSTATUS ms;

	GlobalMemoryStatus(&ms);
	
	pInfo->dMemoryLoad = (double)ms.dwMemoryLoad / 100.0;
	pInfo->dTotalMemory = (double)ms.dwTotalPhys;
	pInfo->dAvailMemory = (double)ms.dwAvailPhys;
	
	return true;
}

// ���CPUʹ����
inline double Port_GetCpuRate()
{
	// �޷�ֱ�ӻ�ȡ
	return 0.0;
}

// ��ʼ���ػ�����
inline bool Port_InitDaemon()
{
	return true;
}

// ��������̨����
inline bool Port_Execute(const char* cmd_line)
{
	char cmd[256];
	size_t size = strlen(cmd_line) + 1;
	
	if (size > sizeof(cmd))
	{
		return false;
	} 
	
	memcpy(cmd, cmd_line, size);
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	memset(&pi, 0, sizeof(pi));
	
	BOOL res = CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, 
		NULL, NULL, &si, &pi);

	if (res)
	{
		// �ͷ���Դ
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
	}
	
	return res == TRUE;
}

// ���õ�ǰ������CPU���ĵ��׺���
inline bool Port_SetProcessAffinity(int core_id)
{
	if ((core_id < 0) || (core_id >= 64))
	{
		return false;
	}
	
	return SetProcessAffinityMask(GetCurrentProcess(), 0x1 << core_id) == TRUE;
}


#pragma comment(lib, "version.lib")

inline bool Port_GetModuleVersion(char* buffer, size_t len, const char* path)
{
	assert(path != NULL);
	assert(buffer != NULL);
	assert(len > 0);

	buffer[0] = 0;

	DWORD dwHandle, dwLen;
	LPTSTR lpData;

	dwLen = GetFileVersionInfoSize((char*)path, &dwHandle);
	if (!dwLen) 
		return false;

	lpData = (LPTSTR) malloc (dwLen);
	if (!lpData) 
		return false;

	if( !GetFileVersionInfo( (char*)path, dwHandle, dwLen, lpData ) )
	{
		free (lpData);
		return false;
	}

	UINT nQuerySize;  
	DWORD* pTransTable;  
	if (!::VerQueryValue(lpData, "\\VarFileInfo\\Translation",  
		(void **)&pTransTable, &nQuerySize) )
	{  
		free (lpData);
		return false;  
	}  

	DWORD m_dwLangCharset = MAKELONG(HIWORD(pTransTable[0]), LOWORD(pTransTable[0]));  

	char s[256] = {0};

	::sprintf(s, "\\StringFileInfo\\%08lx\\FileVersion", m_dwLangCharset);

	LPTSTR strValue;
	UINT querySize;
	if( VerQueryValue( lpData, s, (LPVOID *) &strValue, (PUINT)&querySize ) ) 
	{
		assert(::strlen(strValue) > 0);
		
		const size_t SIZE1 = strlen(strValue) + 1;

		if (SIZE1 <= len)
		{
			memcpy(buffer, strValue, SIZE1);
		}
		else
		{
			memcpy(buffer, strValue, len - 1);
			buffer[len - 1] = 0;
		}

		free (lpData);

		return true;
	} 

	free (lpData);

	return false;
}


#endif // __WIN_PORTABLE_H__
