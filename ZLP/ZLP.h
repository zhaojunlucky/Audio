// ZLP.h : main header file for the ZLP DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CZLPApp
// See ZLP.cpp for the implementation of this class
//

class CZLPApp : public CWinApp
{
public:
	CZLPApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};
