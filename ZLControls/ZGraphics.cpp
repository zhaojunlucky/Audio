#include "StdAfx.h"
#include "ZGraphics.h"


CZGraphics::CZGraphics(void)
{
}


CZGraphics::~CZGraphics(void)
{
}

void CZGraphics::DrawGlowingText(HDC hDC, LPWSTR szText, RECT &rcArea,CFont &font,DWORD dwTextFlags, int iGlowSize)  
{  
	if(zAeroHelper.IsAeroEnabled())
	{
		//获取主题句柄  
		HTHEME hThm = OpenThemeData(GetDesktopWindow(), L"TextStyle");  
		//创建DIB  
		HDC hMemDC = CreateCompatibleDC(hDC);  


		BITMAPINFO bmpinfo = {0};  
		bmpinfo.bmiHeader.biSize = sizeof(bmpinfo.bmiHeader);  
		bmpinfo.bmiHeader.biBitCount = 32;  
		bmpinfo.bmiHeader.biCompression = BI_RGB;  
		bmpinfo.bmiHeader.biPlanes = 1;  
		bmpinfo.bmiHeader.biWidth = rcArea.right - rcArea.left;  
		bmpinfo.bmiHeader.biHeight = -(rcArea.bottom - rcArea.top);  
		bmpinfo.bmiColors[0].rgbBlue = 200;
		bmpinfo.bmiColors[0].rgbGreen = 201;
		bmpinfo.bmiColors[0].rgbRed = 202;

		HBITMAP hBmp = CreateDIBSection(hMemDC, &bmpinfo, DIB_RGB_COLORS, 0, NULL, 0);  

		if (hBmp == NULL) 
		{
			return;  
		}
		SetBkColor(hMemDC,RGB (200, 201, 202));
		SetBkColor(hDC,RGB (200, 201, 202));
		HGDIOBJ hBmpOld = SelectObject(hMemDC, hBmp);  
		HGDIOBJ pOldFont = SelectObject(hMemDC,&font); 
		//绘制选项  
		DTTOPTS dttopts = {0};  
		dttopts.dwSize = sizeof(DTTOPTS);  
		dttopts.dwFlags = DTT_GLOWSIZE | DTT_COMPOSITED;  
		dttopts.iGlowSize = iGlowSize;  //发光的范围大小  


		//绘制文本  
		RECT rc = {8, 0, rcArea.right - rcArea.left, rcArea.bottom - rcArea.top};  
		HRESULT hr = DrawThemeTextEx(hThm, hMemDC, TEXT_LABEL, 0, szText, -1, dwTextFlags , &rc, &dttopts);  
		if(SUCCEEDED(hr)) 
		{
			BitBlt(hDC, rcArea.left, rcArea.top, rcArea.right - rcArea.left,   
				rcArea.bottom - rcArea.top, hMemDC, 0, 0, SRCCOPY | CAPTUREBLT);  
		}
		//Clear  

		SelectObject(hMemDC, hBmpOld);  
		SelectObject(hMemDC, pOldFont); 
		DeleteObject(hBmp);  
		DeleteDC(hMemDC);  
		CloseThemeData(hThm);  
	}
	else
	{
		CDC *pDC = CDC::FromHandle(hDC);
		pDC->TextOutW(0,0,szText);
	}
}
