
// ZLPlayer.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ZLPlayer.h"
#include "ZLPlayerDlg.h"
#include "..\ZLControls\ZRegHelper.h"
#include "..\ZLControls\ZSingleton.h"
#include <Dbghelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Memory leak detecting
#include <stdlib.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC

// CZLPlayerApp

BEGIN_MESSAGE_MAP(CZLPlayerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CZLPlayerApp construction

CZLPlayerApp::CZLPlayerApp()
{
	//_crtBreakAlloc = 304;
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);
}



// The one and only CZLPlayerApp object

CZLPlayerApp theApp;


// CZLPlayerApp initialization

BOOL CZLPlayerApp::InitInstance()
{
//TODO: call AfxInitRichEdit2() to initialize richedit2 library.
#ifndef _DEBUG
	int nNumArgs = 0;

	PWSTR *ppArgv = CommandLineToArgvW(::GetCommandLineW(),&nNumArgs);

	if(2 == nNumArgs)
	{
		if(0 == wcsnicmp(ppArgv[1],L"/reg",4) && wcslen(ppArgv[1]) > 4)//
		{
			CZRegHelper reg;
			reg.ParseReg(ppArgv,nNumArgs);
			return FALSE;
		}
	}
	CZSingleton sg;
	if(sg.Check(ppArgv,nNumArgs,L"ZLPlayer"))
		return FALSE;
#endif
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	AfxInitRichEdit2();
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CZLPlayerDlg dlg;
	m_pMainWnd = &dlg;
	
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

CZLPlayerApp::~CZLPlayerApp()
{
	_CrtDumpMemoryLeaks();
}

LONG CZLPlayerApp::ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	// 这里弹出一个错误对话框并退出程序  
	//  
	
	SYSTEMTIME   sysTime; 
	GetLocalTime(&sysTime); 
	CString   strMsg; 
	strMsg.Format(   L"ErrorDmp\\%d-%d-%d-%d--%d",   sysTime.wYear,sysTime.wMonth, 
		sysTime.wDay,sysTime.wHour,sysTime.wMinute   ); 
	
	CreateDumpFile(strMsg+".dmp",pException);
	strMsg.Empty();
	FatalAppExit(-1,  _T("*** Unhandled Exception! ***")); 
	return EXCEPTION_EXECUTE_HANDLER;  
}  

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
	// 创建Dump文件  
	//  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);    

	// Dump信息     
	//     
	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;    
	dumpInfo.ExceptionPointers = pException;    
	dumpInfo.ThreadId = GetCurrentThreadId();    
	dumpInfo.ClientPointers = TRUE;    

	// 写入Dump文件内容     
	//     
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);    

	CloseHandle(hDumpFile);    
}  

int CZLPlayerApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	GdiplusShutdown(m_gdiplusToken);
	TRACE(_T("ExitInstance() for regular DLL: ZLPlyer\n"));
	return CWinApp::ExitInstance();
}
