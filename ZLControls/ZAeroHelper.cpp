#include "StdAfx.h"
#include "ZAeroHelper.h"

#include <shobjidl.h>
#include <dwmapi.h>

#pragma comment (lib , "dwmapi.lib" ) 

CZAeroHelper::CZAeroHelper(void)
{
}


CZAeroHelper::~CZAeroHelper(void)
{
}

bool CZAeroHelper::IsAeroEnabled()
{
	BOOL bDwm = FALSE;
	OSVERSIONINFO osvi;
	if(GetOSVersionInfo(&osvi))
	{
		if(osvi.dwMajorVersion >= 6 )
		{			
			DwmIsCompositionEnabled (&bDwm );			
		}
	}
	return BOOLTobool(bDwm);
}

bool CZAeroHelper::GetOSVersionInfo(OSVERSIONINFO *osvi)
{
	assert(NULL != osvi);
	ZeroMemory(osvi, sizeof(OSVERSIONINFO));  
	osvi->dwOSVersionInfoSize = sizeof(OSVERSIONINFO);  
	BOOL b = GetVersionEx(osvi);
	return BOOLTobool(b);
}


bool CZAeroHelper::EnableAero(CDialogEx *dlg)
{
	HRESULT hr = S_FALSE;
	if(IsAeroEnabled())
	{
		//SetWindowLong(dlg->GetSafeHwnd(),GWL_EXSTYLE,WS_EX_LAYERED);
		//SetLayeredWindowAttributes (dlg->GetSafeHwnd(),RGB (200, 201, 202) , 0, LWA_COLORKEY );
		MARGINS mrg = {-1};  
		hr = DwmExtendFrameIntoClientArea (dlg->GetSafeHwnd() , &mrg );
		dlg->SetBackgroundColor (RGB (0, 0, 0) );
	}
	return HRESULTTobool(hr);
}


bool CZAeroHelper::DisableAero(CDialogEx *dlg)
{
	HRESULT hr = S_FALSE;

	MARGINS mrg = {0};  
	hr = DwmExtendFrameIntoClientArea (dlg->GetSafeHwnd() , &mrg );
	dlg->SetBackgroundColor (GetSysColor( COLOR_WINDOW) );
	return HRESULTTobool(hr);
}

bool CZAeroHelper::EnableBlurBehindWindow(HWND window,
	bool enable ,
	HRGN region ,
	bool transitionOnMaximized)
{
	DWM_BLURBEHIND blurBehind = { 0 };

	blurBehind.dwFlags = DWM_BB_ENABLE | DWM_BB_TRANSITIONONMAXIMIZED;
	blurBehind.fEnable = enable;
	blurBehind.fTransitionOnMaximized = transitionOnMaximized;

	if (enable && 0 != region)
	{
		blurBehind.dwFlags |= DWM_BB_BLURREGION;
		blurBehind.hRgnBlur = region;
	}

	return HRESULTTobool(::DwmEnableBlurBehindWindow(window,
		&blurBehind));
}


bool CZAeroHelper::GetColorizationColor(DWORD *pcrColorization,BOOL *pfOpaqueBlend)
{
	return HRESULTTobool(DwmGetColorizationColor(pcrColorization, pfOpaqueBlend));
}