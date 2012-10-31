#pragma once
#pragma once
#include <Uxtheme.h>
#include "ZAeroHelper.h"
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"Uxtheme.lib")

class CZGraphics
{
private:
	CZAeroHelper zAeroHelper;
public:
	__declspec(dllexport) CZGraphics(void);
	__declspec(dllexport) virtual ~CZGraphics(void);

	__declspec(dllexport) void DrawGlowingText(HDC hDC, LPWSTR szText, RECT &rcArea,CFont &font,DWORD dwTextFlags = DT_LEFT | DT_VCENTER | DT_SINGLELINE, int iGlowSize = 10) ;
	__declspec(dllexport) void static DestoryHBITMAP(HBITMAP hBmp)
	{
		if(hBmp)
		{
			::DeleteObject(hBmp);
			hBmp = 0;
		}
	}
	__declspec(dllexport) HBITMAP  static CopyBitmap(HBITMAP  hSourcehBitmap )
	{
		 CDC sourcedc;
		 CDC destdc;
		 sourcedc.CreateCompatibleDC(NULL);
		 destdc.CreateCompatibleDC(NULL);
		 //the   bitmap   information.
		 BITMAP   bm = {0};
		 //get   the   bitmap   information.
		 ::GetObject(hSourcehBitmap,   sizeof(bm),   &bm);
		 //   create   a   bitmap   to   hold   the   result
		 HBITMAP   hbmresult   =   ::CreateCompatibleBitmap(CClientDC(NULL),   bm.bmWidth ,   bm.bmHeight );
		 HBITMAP   hbmoldsource   =   (HBITMAP)::SelectObject(   sourcedc.m_hDC ,   hSourcehBitmap);
		 HBITMAP   hbmolddest   =   (HBITMAP)::SelectObject(   destdc.m_hDC ,   hbmresult   );
		 destdc.BitBlt(0,0,bm.bmWidth ,   bm.bmHeight ,   &sourcedc,   0,   0,   SRCCOPY   );

		 //   restore   dcs
		 ::SelectObject(  sourcedc.m_hDC ,   hbmoldsource   );
		 ::SelectObject(  destdc.m_hDC ,   hbmolddest   );
		 ::DeleteObject(  sourcedc.m_hDC );
		 ::DeleteObject(  destdc.m_hDC );

		 return   hbmresult; 
	}

	__declspec(dllexport) SizeF static GetTextBounds(const Gdiplus::Font& font,const Gdiplus::StringFormat& strFormat,const LPWSTR szText)
	{
		GraphicsPath path;
		FontFamily fontfamily;
		font.GetFamily(&fontfamily);
		path.AddString(szText,-1,&fontfamily,font.GetStyle(),font.GetSize(),PointF(0,0),&strFormat);
		RectF rcBound;
		path.GetBounds(&rcBound);
		return SizeF(rcBound.Width,rcBound.Height);
	}

};

