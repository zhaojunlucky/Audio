#include "MyMutex.h"


CMyMutex::CMyMutex(void)
{
	InitializeCriticalSection(&CriticalSection);
}


CMyMutex::~CMyMutex(void)
{
	DeleteCriticalSection(&CriticalSection);
}
