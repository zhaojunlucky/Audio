// ZLControls.h : main header file for the ZLControls DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CZLControlsApp
// See ZLControls.cpp for the implementation of this class
//


class CZLControlsApp : public CWinApp
{
public:
	CZLControlsApp();
	virtual ~CZLControlsApp();
// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
	//ULONG_PTR m_gdiplusToken;  
private:
};
