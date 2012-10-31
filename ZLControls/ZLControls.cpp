// ZLControls.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ZLControls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Memory leak detecting
#include <stdlib.h>
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CZLControlsApp

BEGIN_MESSAGE_MAP(CZLControlsApp, CWinApp)
END_MESSAGE_MAP()


// CZLControlsApp construction

CZLControlsApp::CZLControlsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	//_crtBreakAlloc = 304;
}

CZLControlsApp::~CZLControlsApp()
{

}
// The one and only CZLControlsApp object

CZLControlsApp theApp;


// CZLControlsApp initialization

BOOL CZLControlsApp::InitInstance()
{
	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	//GdiplusStartupInput gdiplusStartupInput;
	//GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	return TRUE;
}


int CZLControlsApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	
	//_CrtDumpMemoryLeaks();
	TRACE(_T("ExitInstance() for ZLControls DLL: TESTDLL\n"));
	return CWinApp::ExitInstance();
}
