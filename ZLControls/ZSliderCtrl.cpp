// ZSliderCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZSliderCtrl.h"


// CZSliderCtrl

IMPLEMENT_DYNAMIC(CZSliderCtrl, CSliderCtrl)

CZSliderCtrl::CZSliderCtrl()
{
	isBtnDown = false;
	m_bDragging = false;
	m_bDragChanged = false;
	message = ZSLIDERMESSAGE;
	CZAeroHelper zAero;
	BOOL opaque = FALSE;
	bkColor = RGB(0,0,0);
	if(!zAero.GetColorizationColor(&bkColor,&opaque))
		bkColor = zAero.IsAeroEnabled()?RGB (1, 1, 1):GetSysColor(COLOR_WINDOW);
	brush1 = new SolidBrush(Color(205, 0, 0));
	brush2 = new SolidBrush(Color(139, 0, 255));
}

CZSliderCtrl::~CZSliderCtrl()
{
	delete brush2;
	delete brush1;
}


BEGIN_MESSAGE_MAP(CZSliderCtrl, CSliderCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CZSliderCtrl message handlers




void CZSliderCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	isBtnDown =  true;   
	m_bDragging = true;
	m_bDragChanged = false;
	SetCapture();
	SetFocus();
	if (SetThumb(point))
	{
		m_bDragChanged = true;
		PostMessageToParent(TB_THUMBTRACK);
	}
	NoticePosition(GetPos());
}


void CZSliderCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CSliderCtrl::OnPaint() for painting messages

	CRect rcClient;
	GetClientRect(rcClient);

	/*CDC MemDC;
	CBitmap m_bitmap;

	MemDC.CreateCompatibleDC(NULL);
	m_bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height() );

	CBitmap *oldbitmap = (CBitmap*)MemDC.SelectObject(&m_bitmap);
	//MemDC.SetBkColor(bkColor);
	MemDC.FillSolidRect(rcClient, bkColor); //用内存DC绘背景

	CRect rc1, rc2;
	GetChannelRect(rc1);
	GetThumbRect(rc2);
	MemDC.FillSolidRect(rc1, RGB(205, 0, 0)); //绘中间那条滑块区域

	if(this->IsWindowEnabled())
		MemDC.FillSolidRect(rc2, RGB(139, 0, 255)); //绘滑块按钮的样子, 这两个地方决定了控件的样子.

	dc.BitBlt(0, 0, rcClient.Width(), rcClient.Height(), &MemDC, 0, 0, SRCCOPY);*/
	CRect rc1, rc2;
	GetChannelRect(rc1);
	GetThumbRect(rc2);
	Graphics *MyGraph = new Graphics(dc.m_hDC);

	MyGraph->Clear(Color(0,0,0));
	MyGraph->FillRectangle(brush1,rc1.left,rc1.top,rc1.Width(),rc1.Height());
	
	if(this->IsWindowEnabled())
	{
		MyGraph->FillRectangle(brush2,rc2.left,rc2.top,rc2.Width(),rc2.Height());
	}
	delete MyGraph;
}


void CZSliderCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if(m_bDragging)
	{
		m_bDragging = false;
		::ReleaseCapture();
		if (SetThumb(point))
		{
			PostMessageToParent(TB_THUMBTRACK);
			m_bDragChanged = true;
		}
		if (m_bDragChanged)
		{
			PostMessageToParent(TB_THUMBPOSITION);
			m_bDragChanged = false;
		}
	}
	else
		CSliderCtrl::OnLButtonUp(nFlags, point);
	mousePos = this->GetPos();
	isBtnDown = false;
	NoticePosition(mousePos);
}


void CZSliderCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (m_bDragging)
	{
		if (SetThumb(point))
		{
			m_bDragChanged = true;
			PostMessageToParent(TB_THUMBTRACK);
		}
	}
	else
	{
		CSliderCtrl::OnMouseMove(nFlags, point);
	}
}

int CZSliderCtrl::GetPostionCursor()
{
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;
	CRect rc;
	GetChannelRect(rc);
	double dPos;
	double dCorrectionFactor = 0.0;
	if (GetStyle() & TBS_VERT) 
	{
		// note: there is a bug in GetChannelRect, it gets the orientation of the rectangle mixed up
		dPos = (double)(point.y - rc.left)/(rc.right - rc.left);
	}
	else
	{
		dPos = (double)(point.x - rc.left)/(rc.right - rc.left);
	}
	// This correction factor is needed when you click inbetween tick marks
	// so that the thumb will move to the nearest one
	dCorrectionFactor = 0.5 *(1-dPos) - 0.5 *dPos;
	return (int)(nMin + (nMax-nMin)*dPos + dCorrectionFactor);
}

 void CZSliderCtrl::SetMessageType(UINT msg)
 {
	 assert(msg > WM_USER);
	 message = msg;
 }

 bool CZSliderCtrl::SetThumb(const CPoint& pt)
 {
	const int nMin = GetRangeMin();
	const int nMax = GetRangeMax()+1;
	CRect rc;
	GetChannelRect(rc);
	double dPos;
	double dCorrectionFactor = 0.0;
	if (GetStyle() & TBS_VERT) 
	{
		// note: there is a bug in GetChannelRect, it gets the orientation of the rectangle mixed up
		dPos = (double)(pt.y - rc.left)/(rc.right - rc.left);
	}
	else
	{
		dPos = (double)(pt.x - rc.left)/(rc.right - rc.left);
	}
	// This correction factor is needed when you click inbetween tick marks
	// so that the thumb will move to the nearest one
	dCorrectionFactor = 0.5 *(1-dPos) - 0.5 *dPos;
	int nNewPos = (int)(nMin + (nMax-nMin)*dPos + dCorrectionFactor);
	const bool bChanged = (nNewPos != GetPos());
	if(bChanged)
	{
		SetPos(nNewPos);
	}
	return bChanged;
 }

 void CZSliderCtrl::PostMessageToParent(const int nTBCode) const
 {
	 CWnd* pWnd = GetParent();
	 if(pWnd) 
		 pWnd->PostMessage(WM_HSCROLL, (WPARAM)((GetPos() << 16) | nTBCode), (LPARAM)GetSafeHwnd());
 }

 void CZSliderCtrl::NoticePosition(int pos)
 {
	 CWnd* pWnd = GetParent();
	 if(pWnd) 
		 pWnd->SendMessage(message,(WPARAM)pos,(LPARAM)GetSafeHwnd());
 }


 //LRESULT CZSliderCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
 //{
	// // TODO: Add your specialized code here and/or call the base class
	// if(message == WM_DWMCOLORIZATIONCOLORCHANGED)
	// {
	//	 CZAeroHelper cz;
	//	 bkColor = wParam;
	//	 return FALSE;
	// }
	// else if(message == WM_DWMCOMPOSITIONCHANGED)
	// {
	//	 CZAeroHelper cz;
	//	 if(!cz.IsAeroEnabled())
	//	 {
	//		 bkColor = GetSysColor(COLOR_WINDOW);
	//	 }
	//	 return FALSE;
	// }
	// return CSliderCtrl::WindowProc(message, wParam, lParam);
 //}

 void CZSliderCtrl::SetBkColor(DWORD color)
 {
	 bkColor = color;
	 Invalidate();
 }


 BOOL CZSliderCtrl::OnEraseBkgnd(CDC* pDC)
 {
	 // TODO: Add your message handler code here and/or call default
	 //CRect rcClient;
	 //GetClientRect(rcClient);
	 //pDC->FillSolidRect(rcClient,bkColor);
	 return TRUE;
	 //return CSliderCtrl::OnEraseBkgnd(pDC);
 }
