#include "StdAfx.h"
#include "ZSingleton.h"


CZSingleton::CZSingleton(void)
{
}


CZSingleton::~CZSingleton(void)
{
}


BOOL CZSingleton::Check(wchar_t **arg,int num,const wchar_t *key)
{
	CString gg(AfxGetApp()->m_pszExeName);
	HANDLE hMutex = ::CreateMutex(NULL,TRUE,key); 
	if(GetLastError() == ERROR_ALREADY_EXISTS) 
	{ 
		CloseHandle(hMutex);
		hMutex = NULL;
		// note:this will not work if you don't run as an adminstrator
		HWND  hWndPrevious = ::GetWindow(::GetDesktopWindow(),GW_CHILD);   
		while   (::IsWindow(hWndPrevious))   
		{ 
			if(::GetProp(hWndPrevious,gg))   
			{   
				if(::IsIconic(hWndPrevious))   
					::ShowWindow(hWndPrevious,SW_RESTORE); 
				if(num >=2 )
				{
					CString mes;
					for(int i = 1;i < num;i++)
						mes.AppendFormat(L"%s|",arg[i]);
					COPYDATASTRUCT cpd; 
					cpd.dwData = 0;
					cpd.cbData = mes.GetLength()*sizeof(wchar_t) ;
					cpd.lpData = (void*)mes.GetBuffer();
					SendMessage(hWndPrevious,WM_COPYDATA,0,(LPARAM)&cpd);
					mes.ReleaseBuffer();
					mes.Empty();
				}
				::SetForegroundWindow(hWndPrevious);   
				::SetForegroundWindow(::GetLastActivePopup(hWndPrevious)); 
				gg.Empty();
				return TRUE; 
			}    
			hWndPrevious   =   ::GetWindow(hWndPrevious,GW_HWNDNEXT); 
		}
		return TRUE;
	} 
	return FALSE;
}

void CZSingleton::SetDrop()
{
	CString gg(AfxGetApp()->m_pszExeName);
	::SetProp(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), gg, (HANDLE)1);
	gg.Empty();
}

void CZSingleton::RemoveDrop()
{
	CString gg(AfxGetApp()->m_pszExeName);
	::RemoveProp(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),gg);
	gg.Empty();
}