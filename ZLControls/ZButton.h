#pragma once
#include "CGdiPlusBitmap.h"

// CZButton

class CZButton : public CMFCButton
{
	DECLARE_DYNAMIC(CZButton)

public:
	__declspec(dllexport) CZButton();
	__declspec(dllexport) virtual ~CZButton();
	__declspec(dllexport) void SetBkColor(DWORD);
	__declspec(dllexport) void LoadPngBitmap(UINT normal,UINT hover,UINT press,UINT disable); 
protected:
	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void DrawTransparent(int x,int y,CDC *pDC,CBitmap *pBitmap,COLORREF Color,CRect* rect);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
private:

	CGdiPlusBitmapResource		*bDisable;
	CGdiPlusBitmapResource		*bPress;
	CGdiPlusBitmapResource		*bHover;
	CGdiPlusBitmapResource		*bNormal;

	bool m_tracking;
	//ULONG_PTR m_gdiplusToken;  
	DWORD bkColor;
	void DrawImage(HDC hdc,Bitmap *bmp);
	void DrawImage(HDC hdc,CGdiPlusBitmapResource *bmp);
	
};


