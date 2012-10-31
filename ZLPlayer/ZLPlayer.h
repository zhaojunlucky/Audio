
// ZLPlayer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CZLPlayerApp:
// See ZLPlayer.cpp for the implementation of this class
//
static ULONG_PTR m_gdiplusToken; 
static void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException) ;
class CZLPlayerApp : public CWinApp
{
public:
	CZLPlayerApp();
	virtual ~CZLPlayerApp();

// Overrides
public:
	virtual BOOL InitInstance();
	static LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException) ;
// Implementation

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();

	
};

extern CZLPlayerApp theApp;