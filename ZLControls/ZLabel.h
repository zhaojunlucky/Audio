#pragma once

#include "ZGraphics.h"

// CZLabel

class CZLabel : public CStatic
{
	DECLARE_DYNAMIC(CZLabel)

public:
	__declspec(dllexport) CZLabel();
	__declspec(dllexport) virtual ~CZLabel();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
private:
	CZGraphics zGraphics;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


