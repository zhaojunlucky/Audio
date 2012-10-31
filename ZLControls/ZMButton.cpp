// ZMButton.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZMButton.h"


// CZMButton

IMPLEMENT_DYNAMIC(CZMButton, CMFCButton)

CZMButton::CZMButton()
{
	/*GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);*/
	bStdImage = new CGdiPlusBitmapResource;
	bAtlImage = new CGdiPlusBitmapResource;
	m_tracking = false;
	stdImage = true;
	pressed = false;
	hasAtlImage = false;
	bkColor = RGB(0,0,0);
}

CZMButton::~CZMButton()
{
	if(bStdImage)
		delete bStdImage;
	if(bAtlImage)
		delete bAtlImage;
	/*GdiplusShutdown(m_gdiplusToken);*/
}


BEGIN_MESSAGE_MAP(CZMButton, CMFCButton)
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// CZMButton message handlers


void CZMButton::LoadAtlPngImage(UINT atl)
{
	bAtlImage->Empty();
	bAtlImage->Load(atl,L"PNG");
	hasAtlImage = true;
}

void CZMButton::SetBkColor(DWORD color)
{
	if(RGB(0,0,0) == bkColor)
		bkColor =color;
	else 
		bkColor = RGB(0,0,0);
	Invalidate();
}

void CZMButton::LoadStdPngImage(UINT std)
{
	bStdImage->Empty();
	bStdImage->Load(std,L"PNG");
}

void CZMButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Add your code to draw the specified item
	CRect rect =  lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSaveDC=pDC->SaveDC();
    //pDC->SetBkMode(TRANSPARENT);
	//pDC->SetBkColor(bkColor);

	UINT state = lpDrawItemStruct->itemState;
	disabled = false;
	pressed = false;
	if(state &  ODS_SELECTED )// бЁжа
	{
		if(hasAtlImage &!btnUp)
			stdImage = !stdImage;
		pressed = true;
		
	}
	else if ( m_tracking )
	{
		
	}
	else if(state &  (ODS_DISABLED | ODS_GRAYED))// disabled
	{
		stdImage = true;
		disabled = true;
	}

	if(stdImage)
		DrawImage(lpDrawItemStruct->hDC,bStdImage);
	else
		DrawImage(lpDrawItemStruct->hDC,bAtlImage);
	pDC->RestoreDC(nSaveDC);
}


void CZMButton::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	m_tracking = false;
	Invalidate();
	CMFCButton::OnMouseLeave();
}

void CZMButton::DrawImage(HDC hdc,CGdiPlusBitmapResource *bmp)
{
	DrawImage(hdc,bmp->m_pBitmap);
}

void CZMButton::DrawImage(HDC hdc,Bitmap *bmp)
{
	CRect rtClient;
	GetClientRect(&rtClient);
	Graphics MyGraph(hdc);
	MyGraph.Clear(Color(0,0,0));    
	RectF destRect(REAL(rtClient.left),    REAL(rtClient.top),    REAL(rtClient.Width()),    REAL(rtClient.Height())),
		srcRect;
	Unit  units;
	bmp->GetBounds(&srcRect,&units);
	if(!disabled)
	{
		if(pressed)
		{
			destRect.X += 1;
			destRect.Y += 1;
			destRect.Width -= 1;
			destRect.Height -= 1;

			MyGraph.DrawImage(bmp, destRect, srcRect.X, srcRect.Y, srcRect.Width,    srcRect.Height,    UnitPixel, NULL);
		}
		else if(m_tracking)
		{
			ColorMatrix HotMat = {	1.05f, 0.00f, 0.00f, 0.00f, 0.00f,
				0.00f, 1.05f, 0.00f, 0.00f, 0.00f,
				0.00f, 0.00f, 1.05f, 0.00f, 0.00f,
				0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
				0.05f, 0.05f, 0.05f, 0.00f, 1.00f	};
			ImageAttributes ia;
			ia.SetColorMatrix(&HotMat);
			destRect.X -= 1;
			destRect.Y -= 1;
			destRect.Width += 1;
			destRect.Height += 1;
			MyGraph.DrawImage(bmp, destRect, srcRect.X, srcRect.Y, srcRect.Width,    srcRect.Height,    UnitPixel,&ia);
		}
		else
			MyGraph.DrawImage(bmp, destRect, srcRect.X, srcRect.Y, srcRect.Width,    srcRect.Height,    UnitPixel, NULL);
	}
	else
	{
		ColorMatrix GrayMat = {	0.30f, 0.30f, 0.30f, 0.00f, 0.00f,
			0.59f, 0.59f, 0.59f, 0.00f, 0.00f,
			0.11f, 0.11f, 0.11f, 0.00f, 0.00f,
			0.00f, 0.00f, 0.00f, 1.00f, 0.00f,
			0.00f, 0.00f, 0.00f, 0.00f, 1.00f	};
		ImageAttributes ia;
		ia.SetColorMatrix(&GrayMat);
		MyGraph.DrawImage(bmp, destRect, srcRect.X, srcRect.Y, srcRect.Width,    srcRect.Height,    UnitPixel, &ia);
	}
}

void CZMButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_tracking) 
	{
		TRACKMOUSEEVENT t = {
			sizeof(TRACKMOUSEEVENT),
			TME_LEAVE,
			m_hWnd,
			0
		};
		if (::_TrackMouseEvent(&t)) {
			//TRACE("* Mouse enter\n");
			m_tracking = true;
			//hover = true;
			Invalidate();
		}
	}
	CMFCButton::OnMouseMove(nFlags, point);
}

//void CZMButton::OnLButtonDblClk(UINT nFlags, CPoint point)
//{
//	// TODO: Add your message handler code here and/or call default
//
//	CMFCButton::OnLButtonDown(nFlags, point);
//}

bool CZMButton::IsAtlCommand()
{
	return stdImage;
}

bool CZMButton::SetAtlCommand(bool atl)
{
	if(!hasAtlImage)
		return false;
	stdImage = !atl;
	Invalidate();	
	return true;
}

BOOL CZMButton::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_LBUTTONDBLCLK)
		pMsg->message = WM_LBUTTONDOWN;

	if(pMsg->message == WM_LBUTTONDOWN)
		btnUp = false;
	else if(pMsg->message == WM_LBUTTONUP)
		btnUp = true;

	return CMFCButton::PreTranslateMessage(pMsg);
}

