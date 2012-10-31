// MyButton.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZButton.h"

// CMyButton

IMPLEMENT_DYNAMIC(CZButton, CMFCButton)

CZButton::CZButton()
{
	//GdiplusStartupInput gdiplusStartupInput;
	//GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	m_tracking = false;
	bNormal = new CGdiPlusBitmapResource;
	bHover = new CGdiPlusBitmapResource;
	bPress = new CGdiPlusBitmapResource;
	bDisable = new CGdiPlusBitmapResource;
	bkColor = RGB(0,0,0);
}

CZButton::~CZButton()
{
	if(bNormal)
		delete bNormal;

	if(bHover)
		delete bHover;

	if(bPress)
		delete bPress;

	if(bDisable)
		delete bDisable;
	/*GdiplusShutdown(m_gdiplusToken);*/
}


BEGIN_MESSAGE_MAP(CZButton, CMFCButton)
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CZButton message handlers

void CZButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Add your code to draw the specified item
	CRect rect =  lpDrawItemStruct->rcItem;
	CDC *pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	int nSaveDC=pDC->SaveDC();
	//pDC->SetBkMode(TRANSPARENT);
	//pDC->SetBkColor(bkColor);
	UINT state = lpDrawItemStruct->itemState;
	

	if(state &  ODS_SELECTED )//бЁжа
	{
		DrawImage(pDC->m_hDC,bPress);
	}
	else if ( m_tracking )
	{
		DrawImage(pDC->m_hDC,bHover);
	}
	else if(state &  (ODS_DISABLED | ODS_GRAYED))// disabled
	{
		DrawImage(pDC->m_hDC,bDisable);
	}
	else
	{
		DrawImage(pDC->m_hDC,bNormal);
	}
	pDC->RestoreDC(nSaveDC);
}

void CZButton::DrawTransparent(int x,int y,CDC *pDC,CBitmap *pBitmap,COLORREF Color,CRect* rect)
{
	BITMAP bm;
	pBitmap->GetObject(sizeof(BITMAP),&bm);
	CDC ImageDC;
	ImageDC.CreateCompatibleDC(pDC);
	CBitmap *pOldImageBitmap=ImageDC.SelectObject(pBitmap);
	CDC MaskDC;
	MaskDC.CreateCompatibleDC(pDC);
	CBitmap MaskBitmap;
	MaskBitmap.CreateBitmap(bm.bmWidth,bm.bmHeight,1,1,NULL);
	CBitmap *pOldMaskBitmap=MaskDC.SelectObject(&MaskBitmap);
	ImageDC.SetBkColor(Color);
	MaskDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&ImageDC,0,0,SRCCOPY);
	CDC OrDC;
	OrDC.CreateCompatibleDC(pDC);
	CBitmap OrBitmap;
	OrBitmap.CreateCompatibleBitmap(&ImageDC,bm.bmWidth,bm.bmHeight);
	CBitmap *pOldOrBitmap=OrDC.SelectObject(&OrBitmap);
	OrDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&ImageDC,0,0,SRCCOPY);
	OrDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&MaskDC,0,0,0x220326);
	CDC TempDC;
	TempDC.CreateCompatibleDC(pDC);
	CBitmap TempBitmap;
	TempBitmap.CreateCompatibleBitmap(&ImageDC,bm.bmWidth,bm.bmHeight);
	CBitmap *pOldTmepBitmap=TempDC.SelectObject(&TempBitmap);
	TempDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,pDC,x,y,SRCCOPY);
	TempDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&MaskDC,0,0,SRCAND);
	TempDC.BitBlt(0,0,bm.bmWidth,bm.bmHeight,&OrDC,0,0,SRCPAINT);
	if (rect!=NULL)
		pDC->BitBlt(x,y,rect->Width(),rect->Height(),&TempDC,rect->left,rect->top,SRCCOPY);
	else
		pDC->BitBlt(x,y,bm.bmWidth,bm.bmHeight,&TempDC,0,0,SRCCOPY);
	TempDC.SelectObject(pOldTmepBitmap);
	OrDC.SelectObject(pOldOrBitmap);
	MaskDC.SelectObject(pOldMaskBitmap);
	ImageDC.SelectObject(pOldImageBitmap);

	DeleteDC(TempDC);
	DeleteDC(OrDC);
	DeleteDC(MaskDC);
	DeleteDC(ImageDC);
}

void CZButton::SetBkColor(DWORD color)
{
	if(RGB(0,0,0) == bkColor)
		bkColor = color;
	else 
		bkColor = RGB(0,0,0);

	Invalidate();
}
void CZButton::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	m_tracking = false;
	Invalidate();
	CMFCButton::OnMouseLeave();
}

void CZButton::DrawImage(HDC hdc,CGdiPlusBitmapResource *bmp)
{
	DrawImage(hdc,bmp->m_pBitmap);
}

void CZButton::DrawImage(HDC hdc,Bitmap *bmp)
{
	CRect rtClient;
	GetClientRect(&rtClient);
	Graphics MyGraph(hdc);
	MyGraph.Clear(Color(0,0,0));    
	RectF destRect(REAL(rtClient.left),    REAL(rtClient.top),    REAL(rtClient.Width()),    REAL(rtClient.Height())),
		srcRect;
	Unit  units;
	bmp->GetBounds(&srcRect,&units);
	MyGraph.DrawImage(bmp, destRect, srcRect.X, srcRect.Y, srcRect.Width,    srcRect.Height,    UnitPixel, NULL);

}

void CZButton::OnMouseMove(UINT nFlags, CPoint point)
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


void CZButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CMFCButton::OnLButtonDown(nFlags, point);
}

void CZButton::LoadPngBitmap(UINT normal,UINT hover,UINT press,UINT disable)
{
	bNormal->Empty();
	bNormal->Load(normal,L"PNG");

	bHover->Empty();
	bHover->Load(hover,L"PNG");

	bPress->Empty();
	bPress->Load(press,L"PNG");

	bDisable->Empty();
	bDisable->Load(disable,L"PNG");
}