#pragma once

class CZElevation
{
public:
	__declspec(dllexport) CZElevation();
	__declspec(dllexport) virtual ~CZElevation();
	__declspec(dllexport) BOOL GetProcessElevation(TOKEN_ELEVATION_TYPE* pElevationType, BOOL* pIsAdmin);
	__declspec(dllexport) DWORD StartElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine);
};