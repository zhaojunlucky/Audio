#pragma once
#include <afxdialogex.h>

class CZAeroHelper
{
public:
	__declspec(dllexport) CZAeroHelper(void);
	__declspec(dllexport) virtual ~CZAeroHelper(void);

	__declspec(dllexport) bool IsAeroEnabled();
	__declspec(dllexport) bool GetOSVersionInfo(OSVERSIONINFO *);
	__declspec(dllexport) bool EnableAero(CDialogEx *);
	__declspec(dllexport) bool DisableAero(CDialogEx *);
	__declspec(dllexport) bool EnableBlurBehindWindow(HWND window,bool enable = true,HRGN region = 0,bool transitionOnMaximized = false);
	__declspec(dllexport) bool GetColorizationColor(DWORD *pcrColorization,BOOL *pfOpaqueBlend);
};

