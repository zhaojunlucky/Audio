
// ZLEndoder.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CZLEndoderApp:
// See ZLEndoder.cpp for the implementation of this class
//

class CZLEndoderApp : public CWinApp
{
public:
	CZLEndoderApp();
	~CZLEndoderApp();
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CZLEndoderApp theApp;