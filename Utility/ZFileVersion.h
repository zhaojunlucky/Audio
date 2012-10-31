#pragma once
#include <io.h>

#pragma comment(lib,"Version.lib")

struct CZVersionInfo
{
	DWORD fileLeftMost;
	DWORD fileSecondLeft; 
	DWORD fileSecondRight; 
	DWORD fileRightMost; 

	DWORD productLeftMost;
	DWORD productSecondLeft; 
	DWORD productSecondRight; 
	DWORD productRightMost; 
};

class CZFileVersion
{
public:
	__declspec(dllexport) CZFileVersion(void);
	__declspec(dllexport) ~CZFileVersion(void);

	__declspec(dllexport) void GetVersionInfo(const wchar_t *pszFilePath,CZVersionInfo *verInfo);

	__declspec(dllexport) void GetVersionInfo(const wchar_t fileDir[MAX_PATH],const wchar_t fileName[MAX_PATH],CZVersionInfo *zverInfo);
};

