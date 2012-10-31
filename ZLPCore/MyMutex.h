#pragma once
#include <WINDOWS.H>
class CMyMutex
{
public:
	__declspec(dllexport) CMyMutex(void);
	__declspec(dllexport) ~CMyMutex(void);
private:
	CRITICAL_SECTION CriticalSection; 
public:
	__declspec(dllexport) void Lock()
	{
		EnterCriticalSection(&CriticalSection);
	}
	__declspec(dllexport) void Unlock()
	{
		LeaveCriticalSection(&CriticalSection);
	}
};

