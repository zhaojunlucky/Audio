#pragma once
#include "CGdiPlusBitmap.h"

// CZMButton

class CZMButton : public CMFCButton
{
	DECLARE_DYNAMIC(CZMButton)

public:
	__declspec(dllexport) CZMButton();
	__declspec(dllexport) virtual ~CZMButton();


	__declspec(dllexport) void LoadStdPngImage(UINT std);
	__declspec(dllexport) void LoadAtlPngImage(UINT atl);
	__declspec(dllexport) bool IsAtlCommand();
	__declspec(dllexport) bool SetAtlCommand(bool);
	__declspec(dllexport) void SetBkColor(DWORD color);
protected:
	DECLARE_MESSAGE_MAP()
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	void DrawTransparent(int x,int y,CDC *pDC,CBitmap *pBitmap,COLORREF Color,CRect* rect);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	//afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	bool m_tracking;
	bool pressed;
	bool disabled;
	bool stdImage;
	bool hasAtlImage;
	bool btnUp;
	void DrawImage(HDC hdc,Bitmap *bmp);
	void DrawImage(HDC hdc,CGdiPlusBitmapResource *bmp);
private:
	CGdiPlusBitmapResource *bStdImage;
	CGdiPlusBitmapResource *bAtlImage;
	DWORD bkColor;
	//ULONG_PTR m_gdiplusToken;  
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


