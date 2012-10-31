// ZLabel.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZLabel.h"


// CZLabel

IMPLEMENT_DYNAMIC(CZLabel, CStatic)

CZLabel::CZLabel()
{

}

CZLabel::~CZLabel()
{
}


BEGIN_MESSAGE_MAP(CZLabel, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CZLabel message handlers




void CZLabel::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CStatic::OnPaint() for painting messages
	CRect rcCtrl;
	GetClientRect(&rcCtrl);
	CString txt;
	this->GetWindowTextW(txt);
	BSTR bstr = txt.AllocSysString();
	zGraphics.DrawGlowingText(dc.GetSafeHdc(),bstr,rcCtrl,*(this->GetFont()), DT_LEFT | DT_VCENTER | DT_SINGLELINE,10);
	SysFreeString(bstr);
}


BOOL CZLabel::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;//CStatic::OnEraseBkgnd(pDC);
}
