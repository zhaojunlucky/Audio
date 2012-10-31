// ZImageCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZImageCtrl.h"

// CZImageCtrl

IMPLEMENT_DYNAMIC(CZImageCtrl, CStatic)

CZImageCtrl::CZImageCtrl()
{
	/*GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);*/
	pImg = 0;
}

CZImageCtrl::~CZImageCtrl()
{
	if(pImg)
		delete pImg;
	/*GdiplusShutdown(m_gdiplusToken);*/
}


BEGIN_MESSAGE_MAP(CZImageCtrl, CStatic)
END_MESSAGE_MAP()



// CZImageCtrl message handlers


void CZImageCtrl::SetBitmap(const HBITMAP hBmp)
{
	m_mutex.Lock();
	if(pImg)
		delete pImg;
	pImg = new Bitmap(hBmp,NULL);
	m_mutex.Unlock();
	Invalidate();
}

void CZImageCtrl::LoadFromFile(const wchar_t *file)
{
	m_mutex.Lock();
	if(pImg)
		delete pImg;
	pImg = new Bitmap(file);
	m_mutex.Unlock();
	Invalidate();
}

void CZImageCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO:  Add your code to draw the specified item
   if (NULL!=pImg)
    {
        CRect rtClient;
        GetClientRect(&rtClient);
        Graphics MyGraph(this->GetSafeHwnd());
        
        RectF destRect(REAL(rtClient.left),    REAL(rtClient.top),    REAL(rtClient.Width()),    REAL(rtClient.Height())),
            srcRect;
        Unit  units;
        pImg->GetBounds(&srcRect,&units);
        MyGraph.DrawImage(pImg, destRect, srcRect.X, srcRect.Y, srcRect.Width,    srcRect.Height,    UnitPixel, NULL);
    }
}
