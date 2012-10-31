#pragma once
#include "afxwin.h"
class CZBitmap :
	public CBitmap
{
public:
	__declspec(dllexport) CZBitmap(void);
	__declspec(dllexport) ~CZBitmap(void);

	__declspec(dllexport) HBITMAP SetBitmap(HBITMAP hBitmap);
	__declspec(dllexport) CRect GetRect();
	__declspec(dllexport) void AlphaDisplay(CDC  *pDC,BYTE bAlpha);
	__declspec(dllexport) BOOL ExtendDraw(CDC *pDC,CRect rc, int nX, int nY,BOOL bTran = FALSE,UINT colorTransparent = RGB(255,0,255));
	__declspec(dllexport) BOOL ExtendDrawImage(CZBitmap &bmp,CRect rc, int nX, int nY);

	__declspec(dllexport) BOOL LoadImage(LPCTSTR szImagePath, COLORREF crBack = 0);
	__declspec(dllexport) BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL, COLORREF crBack = 0); 
	__declspec(dllexport) BOOL LoadImage(UINT uIDRes);
	// helpers
	__declspec(dllexport) static HBITMAP LoadImageFile(LPCTSTR szImagePath, COLORREF crBack = 0);
	__declspec(dllexport) static HBITMAP LoadImageResource(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL, COLORREF crBack = 0); 
	__declspec(dllexport) static BOOL GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize);
	__declspec(dllexport) static IPicture* LoadFromBuffer(BYTE* pBuff, int nSize);
	__declspec(dllexport) HRGN BitmapToRegion(COLORREF cTransparentColor = 0,
					COLORREF cTolerance = 0x101010);
	__declspec(dllexport) BOOL Draw(CDC *pDC, LPRECT r);
	__declspec(dllexport) BOOL DrawImage(CZBitmap &bmp,int nX,int nY,int nCol,int nRow);
	__declspec(dllexport) void TransparentBlt(CDC &dc, CRect rc,UINT crTransparent );	
	__declspec(dllexport) int	 Width()
	{
		return GetWidth();
	}
	__declspec(dllexport) int	 GetWidth()
	{
		BITMAP bm;
		memset( &bm, 0, sizeof(bm) );
		GetBitmap(&bm);
		return bm.bmWidth;
	}

	__declspec(dllexport) int	 Height()
	{
		return GetHeight();
	}

	__declspec(dllexport) int	 GetHeight()
	{
		BITMAP bm;
		memset( &bm, 0, sizeof(bm) );
		GetBitmap(&bm);
		return bm.bmHeight;
	}
	
	__declspec(dllexport) int CZBitmap::Grey( int r, int g, int b )
	{
		return ((( b * 11 ) + ( g * 59 ) + ( r * 30 )) / 100 );
	}
	__declspec(dllexport) void DrawGreyScale( CDC *pDC);
	__declspec(dllexport) void DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth, 
				int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);							
	__declspec(dllexport) HPALETTE CreateReservedPalette(CDC *pDC);
	__declspec(dllexport) void FadeColorToGrayScale( CDC *pDC, int xDest, int yDest, int nLoops, 
									int nDelay ) ;
	__declspec(dllexport) HRGN InflateRegion(HRGN hRgn, int nXInflate, int nYInflate);
	__declspec(dllexport) HRGN CreateRegionExt(DWORD nCount, CONST RGNDATA *pRgnData );
	__declspec(dllexport) BOOL StretchDraw(CDC *pDC, LPRECT r, LPRECT sr);
protected:
	__declspec(dllexport) static HBITMAP ExtractBitmap(IPicture* pPicture, COLORREF crBack);
	__declspec(dllexport) static int GetFileType(LPCTSTR szImagePath);
};

