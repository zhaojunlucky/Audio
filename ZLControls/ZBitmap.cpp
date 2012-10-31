#include "StdAfx.h"
#include "ZBitmap.h"

const int HIMETRIC_INCH	= 2540;

enum 
{
	FT_BMP,
	FT_ICO,
	FT_JPG,
	FT_GIF,

	FT_UNKNOWN
};

CZBitmap::CZBitmap(void)
{
}


CZBitmap::~CZBitmap(void)
{
}

BOOL CZBitmap::LoadImage(UINT uIDRes)
{
	return CBitmap::LoadBitmapW(uIDRes);
}
BOOL CZBitmap::LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy

	if (m_hObject != NULL)
		return FALSE;

	return Attach(LoadImageResource(uIDRes, szResourceType, hInst, crBack));
}

BOOL CZBitmap::LoadImage(LPCTSTR szImagePath, COLORREF crBack)
{
	//如果原先存在就去除
	if (m_hObject != NULL)
	{
		DeleteObject();
		Detach();
	}

	return Attach(LoadImageFile(szImagePath, crBack));
}

HBITMAP CZBitmap::LoadImageFile(LPCTSTR szImagePath, COLORREF crBack)
{
	int nType = GetFileType(szImagePath);

	switch (nType)
	{
		// the reason for this is that i suspect it is more efficient to load
		// bmps this way since it avoids creating device contexts etc that the 
		// IPicture methods requires. that method however is still valuable
		// since it handles other image types and transparency
		case FT_BMP:
			return (HBITMAP)::LoadImage(NULL, szImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		case FT_UNKNOWN:
			return NULL;

		default: // all the rest
		{
			USES_CONVERSION;
			IPicture* pPicture = NULL;
			
			HBITMAP hbm = NULL;
			HRESULT hr = OleLoadPicturePath(A2OLE(W2A(szImagePath)), NULL, 0, crBack, IID_IPicture, (LPVOID *)&pPicture);
					
			if (pPicture)
			{
				hbm = ExtractBitmap(pPicture, crBack);
				pPicture->Release();
			}

			return hbm;
		}
	}

	return NULL; // can't get here
}

HBITMAP CZBitmap::LoadImageResource(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst, COLORREF crBack)
{
	BYTE* pBuff = NULL;
	int nSize = 0;
	HBITMAP hbm = NULL;

	// first call is to get buffer size

	if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, 0, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];
			
			// this loads it
			if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, pBuff, nSize))
			{
				IPicture* pPicture = LoadFromBuffer(pBuff, nSize);

				if (pPicture)
				{
					hbm = ExtractBitmap(pPicture, crBack);
					pPicture->Release();
				}
			}
			
			delete [] pBuff;
		}
	}

	return hbm;
}

IPicture* CZBitmap::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuff, nSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	IPicture* pPicture = NULL;

	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		HRESULT hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&pPicture);
		pStream->Release();
	}

	return pPicture; // caller releases
}

BOOL CZBitmap::GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	// Find the resource
	hResInfo = FindResource(hInst, lpName, lpType);

	if (hResInfo == NULL) 
		return false;

	// Load the resource
	hRes = LoadResource(hInst, hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(hInst, hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(hInst, hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}

HBITMAP CZBitmap::ExtractBitmap(IPicture* pPicture, COLORREF crBack)
{
	ASSERT(pPicture);

	if (!pPicture)
		return NULL;

	CBitmap bmMem;
	CDC dcMem;
	CDC* pDC = CWnd::GetDesktopWindow()->GetDC();

	if (dcMem.CreateCompatibleDC(pDC))
	{
		long hmWidth;
		long hmHeight;

		pPicture->get_Width(&hmWidth);
		pPicture->get_Height(&hmHeight);
		
		int nWidth	= MulDiv(hmWidth, pDC->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
		int nHeight	= MulDiv(hmHeight, pDC->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);

		if (bmMem.CreateCompatibleBitmap(pDC, nWidth, nHeight))
		{
			CBitmap* pOldBM = dcMem.SelectObject(&bmMem);

			if (crBack != -1)
				dcMem.FillSolidRect(0, 0, nWidth, nHeight, crBack);
			
			HRESULT hr = pPicture->Render(dcMem, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
			dcMem.SelectObject(pOldBM);
		}
	}

	CWnd::GetDesktopWindow()->ReleaseDC(pDC);

	return (HBITMAP)bmMem.Detach();
}

int CZBitmap::GetFileType(LPCTSTR szImagePath)
{
	CString sPath(szImagePath);
	sPath.MakeUpper();

	if (sPath.Find(L".BMP") > 0)
		return FT_BMP;

	else if (sPath.Find(L".ICO") > 0)
		return FT_ICO;

	else if (sPath.Find(L".JPG") > 0 || sPath.Find(L".JPEG") > 0)
		return FT_JPG;

	else if (sPath.Find(L".GIF") > 0)
		return FT_GIF;

	// else
	return FT_UNKNOWN;
}

/////////////////////////////////////////////////////////////////////
// Converts a bitmap to a region
//
//	BitmapToRegion :	Create a region from the "non-transparent" pixels of a bitmap
//	Author :			Jean-Edouard Lachand-Robert (http://www.geocities.com/Paris/LeftBank/1160/resume.htm), June 1998.
//
//	hBmp :				Source bitmap
//	cTransparentColor :	Color base for the "transparent" pixels (default is black)
//	cTolerance :		Color tolerance for the "transparent" pixels.
//
//	A pixel is assumed to be transparent if the value of each of its 3 components (blue, green and red) is 
//	greater or equal to the corresponding value in cTransparentColor and is lower or equal to the 
//	corresponding value in cTransparentColor + cTolerance.
//
HRGN CZBitmap::BitmapToRegion( COLORREF cTransparentColor ,COLORREF cTolerance)
{
	HBITMAP hBmp = (HBITMAP)GetSafeHandle();
	HRGN hRgn = NULL;

	if(hBmp)
	{
		// Create a memory DC inside which we will scan the bitmap content
		HDC hMemDC = CreateCompatibleDC(NULL);
		if(hMemDC)
		{
			// Get bitmap size
			BITMAP bm;
			::GetObject(hBmp, sizeof(bm), &bm);

			// Create a 32 bits depth bitmap and select it into the memory DC 
			BITMAPINFOHEADER RGB32BITSBITMAPINFO = {	
					sizeof(BITMAPINFOHEADER),	// biSize 
					bm.bmWidth,					// biWidth; 
					bm.bmHeight,				// biHeight; 
					1,							// biPlanes; 
					32,							// biBitCount 
					BI_RGB,						// biCompression; 
					0,							// biSizeImage; 
					0,							// biXPelsPerMeter; 
					0,							// biYPelsPerMeter; 
					0,							// biClrUsed; 
					0							// biClrImportant; 
			};
			VOID * pbits32; 
			HBITMAP hbm32 = CreateDIBSection(hMemDC, (BITMAPINFO *)&RGB32BITSBITMAPINFO, DIB_RGB_COLORS, &pbits32, NULL, 0);
			if(hbm32)
			{
				HBITMAP holdBmp = (HBITMAP)SelectObject(hMemDC, hbm32);

				// Create a DC just to copy the bitmap into the memory DC
				HDC hDC = CreateCompatibleDC(hMemDC);
				if(hDC)
				{
					// Get how many bytes per row we have for the bitmap bits (rounded up to 32 bits)
					BITMAP bm32;
					::GetObject(hbm32, sizeof(bm32), &bm32);
					while(bm32.bmWidthBytes % 4)
						bm32.bmWidthBytes++;

					// Copy the bitmap into the memory DC
					HBITMAP holdBmp = (HBITMAP)SelectObject(hDC, hBmp);
					BitBlt(hMemDC, 0, 0, bm.bmWidth, bm.bmHeight, hDC, 0, 0, SRCCOPY);

					// For better performances, we will use the ExtCreateRegion() function to create the
					// region. This function take a RGNDATA structure on entry. We will add rectangles by
					// amount of ALLOC_UNIT number in this structure.
					#define ALLOC_UNIT	100
					DWORD maxRects = ALLOC_UNIT;
					HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects));
					RGNDATA *pData = (RGNDATA *)GlobalLock(hData);
					pData->rdh.dwSize = sizeof(RGNDATAHEADER);
					pData->rdh.iType = RDH_RECTANGLES;
					pData->rdh.nCount = pData->rdh.nRgnSize = 0;
					SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);

					// Keep on hand highest and lowest values for the "transparent" pixels
					BYTE lr = GetRValue(cTransparentColor);
					BYTE lg = GetGValue(cTransparentColor);
					BYTE lb = GetBValue(cTransparentColor);
					BYTE hr = min(0xff, lr + GetRValue(cTolerance));
					BYTE hg = min(0xff, lg + GetGValue(cTolerance));
					BYTE hb = min(0xff, lb + GetBValue(cTolerance));

					// Scan each bitmap row from bottom to top (the bitmap is inverted vertically)
					BYTE *p32 = (BYTE *)bm32.bmBits + (bm32.bmHeight - 1) * bm32.bmWidthBytes;
					for(int y = 0; y < bm.bmHeight; y++)
					{
						// Scan each bitmap pixel from left to right
						for(int x = 0; x < bm.bmWidth; x++)
						{
							// Search for a continuous range of "non transparent pixels"
							int x0 = x;
							LONG *p = (LONG *)p32 + x;
							while(x < bm.bmWidth)
							{
								BYTE b = GetRValue(*p);
								if(b >= lr && b <= hr)
								{
									b = GetGValue(*p);
									if(b >= lg && b <= hg)
									{
										b = GetBValue(*p);
										if(b >= lb && b <= hb)
											// This pixel is "transparent"
											break;
									}
								}
								p++;
								x++;
							}

							if(x > x0)
							{
								// Add the pixels (x0, y) to (x, y+1) as a new rectangle in the region
								if(pData->rdh.nCount >= maxRects)
								{
									GlobalUnlock(hData);
									maxRects += ALLOC_UNIT;
									hData = GlobalReAlloc(hData, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), GMEM_MOVEABLE);
									pData = (RGNDATA *)GlobalLock(hData);
								}
								RECT *pr = (RECT *)&pData->Buffer;
								SetRect(&pr[pData->rdh.nCount], x0, y, x, y+1);
								if(x0 < pData->rdh.rcBound.left)
									pData->rdh.rcBound.left = x0;
								if(y < pData->rdh.rcBound.top)
									pData->rdh.rcBound.top = y;
								if(x > pData->rdh.rcBound.right)
									pData->rdh.rcBound.right = x;
								if(y+1 > pData->rdh.rcBound.bottom)
									pData->rdh.rcBound.bottom = y+1;
								pData->rdh.nCount++;

								// On Windows98, ExtCreateRegion() may fail if the number of rectangles is too
								// large (ie: > 4000). Therefore, we have to create the region by multiple steps.
								if(pData->rdh.nCount == 2000)
								{
									HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
									if(hRgn)
									{
										CombineRgn(hRgn, hRgn, h, RGN_OR);
										::DeleteObject(h);
									}
									else
										hRgn = h;
									pData->rdh.nCount = 0;
									SetRect(&pData->rdh.rcBound, MAXLONG, MAXLONG, 0, 0);
								}
							}
						}

						// Go to next row (remember, the bitmap is inverted vertically)
						p32 -= bm32.bmWidthBytes;
					}

					// Create or extend the region with the remaining rectangles
					HRGN h = ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + (sizeof(RECT) * maxRects), pData);
					if(hRgn)
					{
						CombineRgn(hRgn, hRgn, h, RGN_OR);
						::DeleteObject(h);
					}
					else
						hRgn = h;

					// Clean up
					GlobalFree(hData);
					SelectObject(hDC, holdBmp);
					DeleteDC(hDC);
				}

				::DeleteObject(SelectObject(hMemDC, holdBmp));
			}

			DeleteDC(hMemDC);
		}	
	}

	return hRgn;
}

BOOL CZBitmap::Draw(CDC *pDC, LPRECT r)
{
	CDC dc;
	dc.CreateCompatibleDC( pDC );
	CBitmap * bmp = dc.SelectObject( this );	
	pDC->BitBlt( r->left, r->top, r->right - r->left, r->bottom - r->top, &dc, 0, 0 ,
		  SRCCOPY );

	dc.SelectObject( bmp );		
	return TRUE;
}

// TransparentBlt	- Copies a bitmap transparently onto the destination DC
// hdcDest		- Handle to destination device context 
// nXDest		- x-coordinate of destination rectangle's upper-left corner 
// nYDest		- y-coordinate of destination rectangle's upper-left corner 
// nWidth		- Width of destination rectangle 
// nHeight		- height of destination rectangle 
// hBitmap		- Handle of the source bitmap
// nXSrc		- x-coordinate of source rectangle's upper-left corner 
// nYSrc		- y-coordinate of source rectangle's upper-left corner 
// colorTransparent	- The transparent color
// hPal			- Logical palette to be used with bitmap. Can be NULL

void CZBitmap::TransparentBlt(CDC &dc,CRect rc,UINT colorTransparent)
{
	CDC pDC;
	pDC.CreateCompatibleDC(&dc);	
	HBITMAP hOldBmp = (HBITMAP) ::SelectObject( pDC.m_hDC, m_hObject );
	
	::TransparentBlt(dc.GetSafeHdc(),rc.left ,rc.top ,rc.Width(),rc.Height(),pDC.GetSafeHdc()  ,0,0,GetWidth(),GetHeight(),colorTransparent);
	::SelectObject(pDC.m_hDC,hOldBmp);
	pDC.DeleteDC();

}



// DrawGreyScale    - Draws a bitmap in Grey scale
// pDC              - Pointer to target device context
// hDIB             - Handle of device-independent bitmap
//
void CZBitmap::DrawGreyScale( CDC *pDC )
{
    CPalette pal;
    CPalette *pOldPalette = 0;

	HBITMAP hBmp = (HBITMAP)this->GetSafeHandle();

	// allocate memory for extended image information
	BITMAPINFO &bmInfo= *(LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( &bmInfo, 0, sizeof(BITMAPINFO) + 8 );
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits(pDC->GetSafeHdc() , hBmp, 0, GetHeight(), 0, &bmInfo, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bmInfo.bmiHeader.biSizeImage + 4 ];

	res = GetDIBits( pDC->GetSafeHdc(), hBmp, 0,  bmInfo.bmiHeader.biHeight, pBits, &bmInfo, DIB_RGB_COLORS );

    int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
                1 << bmInfo.bmiHeader.biBitCount;
    
    int nWidth = bmInfo.bmiHeader.biWidth;
    int nHeight = bmInfo.bmiHeader.biHeight;
    
    // Compute the address of the bitmap bits
    LPVOID lpDIBBits;
    if( bmInfo.bmiHeader.biBitCount > 8 )
        lpDIBBits = (LPVOID)((LPDWORD)(pBits + 
            bmInfo.bmiHeader.biClrUsed) + 
            ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
    else
        lpDIBBits = (LPVOID)(pBits + nColors);
    
    // Create the palette if needed
    if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE && nColors <= 256 )
    {
        // The device supports a palette and bitmap has color table
        
        // Allocate memory for a palette
        UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
        LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

        pLP->palVersion = 0x300;
        pLP->palNumEntries = nColors;
        
        for( int i=0; i < nColors; i++)
        {
            int nGrey = Grey( bmInfo.bmiColors[i].rgbRed, bmInfo.bmiColors[i].rgbGreen,
                bmInfo.bmiColors[i].rgbBlue );
            pLP->palPalEntry[i].peRed = nGrey;
            pLP->palPalEntry[i].peGreen = nGrey;
            pLP->palPalEntry[i].peBlue = nGrey;
            pLP->palPalEntry[i].peFlags = 0;
        }
        
        pal.CreatePalette( pLP );
        
        delete[] pLP;

        // Select the palette
        pOldPalette = pDC->SelectPalette(&pal, FALSE);
        pDC->RealizePalette();
    }
    else
    {
        // Modify the bitmaps pixels directly
        // Note : This ends up changing the DIB. If that is not acceptable then
        // copy the DIB and then change the copy rather than the original
        if ( bmInfo.bmiHeader.biBitCount == 24 ||  bmInfo.bmiHeader.biBitCount == 32)
        {
            BYTE *dst=(BYTE*)lpDIBBits;
            int Size=nWidth*nHeight;

            while ( Size-- )
            {
                int nGrey = Grey( dst[2], dst[1], dst[0] );

                dst[0]=(BYTE)nGrey;
                dst[1]=(BYTE)nGrey;
                dst[2]=(BYTE)nGrey;
                dst+=4;
            }
        }
        else if ( bmInfo.bmiHeader.biBitCount == 16 )
        {
            WORD *dst=(WORD*)lpDIBBits;
            int Size=nWidth*nHeight;

            while ( Size-- )
            {
                BYTE b = (BYTE)((*dst)&(0x1F));
                BYTE g = (BYTE)(((*dst)>>5)&(0x1F));
                BYTE r = (BYTE)(((*dst)>>10)&(0x1F));

                int nGrey = Grey( r, g, b );

                *dst++ = ((WORD)(((BYTE)(nGrey)|((WORD)((BYTE)(nGrey))<<5))|(((DWORD)(BYTE)(nGrey))<<10)));
            }
        }
    }

    // Draw the image 
    ::SetDIBitsToDevice(pDC->m_hDC,    // hDC
        0,                             // XDest
        0,                             // YDest
        nWidth,                        // nDestWidth
        nHeight,                       // nDestHeight
        0,                             // XSrc
        0,                             // YSrc
        0,                             // nStartScan
        nHeight,                       // nNumScans
        lpDIBBits,                     // lpBits
        &bmInfo,            // lpBitsInfo
        DIB_RGB_COLORS);               // wUsage
    
    
    if ( pOldPalette )
        pDC->SelectPalette(pOldPalette, FALSE);
}

//
//      DitherBlt :     Draw a bitmap dithered (3D grayed effect like disabled buttons in toolbars) into a destination DC
//      Author :        Jean-Edouard Lachand-Robert (iamwired@geocities.com), June 1997.
//
//      hdcDest :       destination DC
//      nXDest :        x coordinate of the upper left corner of the destination rectangle into the DC
//      nYDest :        y coordinate of the upper left corner of the destination rectangle into the DC
//      nWidth :        width of the destination rectangle into the DC
//      nHeight :       height of the destination rectangle into the DC
//      hbm :           the bitmap to draw (as a part or as a whole)
//      nXSrc :         x coordinates of the upper left corner of the source rectangle into the bitmap
//      nYSrc :         y coordinates of the upper left corner of the source rectangle into the bitmap
//
void CZBitmap::DitherBlt (HDC hdcDest, int nXDest, int nYDest, int nWidth, 
				int nHeight, HBITMAP hbm, int nXSrc, int nYSrc)
{
	ASSERT(hdcDest && hbm);
	ASSERT(nWidth > 0 && nHeight > 0);
	
	// Create a generic DC for all BitBlts
	HDC hDC = CreateCompatibleDC(hdcDest);
	ASSERT(hDC);
	
	if (hDC)
	{
		// Create a DC for the monochrome DIB section
		HDC bwDC = CreateCompatibleDC(hDC);
		ASSERT(bwDC);
		
		if (bwDC)
		{
			// Create the monochrome DIB section with a black and white palette
			struct {
				BITMAPINFOHEADER bmiHeader; 
				RGBQUAD 		 bmiColors[2]; 
			} RGBBWBITMAPINFO = {
				
				{		// a BITMAPINFOHEADER
					sizeof(BITMAPINFOHEADER),	// biSize 
						nWidth, 				// biWidth; 
						nHeight,				// biHeight; 
						1,						// biPlanes; 
						1,						// biBitCount 
						BI_RGB, 				// biCompression; 
						0,						// biSizeImage; 
						0,						// biXPelsPerMeter; 
						0,						// biYPelsPerMeter; 
						0,						// biClrUsed; 
						0						// biClrImportant; 
				},
				
				{
					{ 0x00, 0x00, 0x00, 0x00 }, { 0xFF, 0xFF, 0xFF, 0x00 }
					} 
			};
			VOID *pbitsBW;
			HBITMAP hbmBW = CreateDIBSection(bwDC,
				(LPBITMAPINFO)&RGBBWBITMAPINFO, DIB_RGB_COLORS, &pbitsBW, NULL, 0);
			ASSERT(hbmBW);
			
			if (hbmBW)
			{
				// Attach the monochrome DIB section and the bitmap to the DCs
				SelectObject(bwDC, hbmBW);
				SelectObject(hDC, hbm);
				
				// BitBlt the bitmap into the monochrome DIB section
				BitBlt(bwDC, 0, 0, nWidth, nHeight, hDC, nXSrc, nYSrc, SRCCOPY);
				
				// Paint the destination rectangle in gray
				FillRect(hdcDest, CRect(nXDest, nYDest, nXDest + nWidth, nYDest +
					nHeight), GetSysColorBrush(COLOR_3DFACE));
				
				// BitBlt the black bits in the monochrome bitmap into COLOR_3DHILIGHT bits in the destination DC
				// The magic ROP comes from the Charles Petzold's book
				HBRUSH hb = CreateSolidBrush(GetSysColor(COLOR_3DHILIGHT));
				HBRUSH oldBrush = (HBRUSH)SelectObject(hdcDest, hb);
				BitBlt(hdcDest, nXDest + 1, nYDest + 1, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
				
				// BitBlt the black bits in the monochrome bitmap into COLOR_3DSHADOW bits in the destination DC
				hb = CreateSolidBrush(GetSysColor(COLOR_3DSHADOW));
				::DeleteObject(SelectObject(hdcDest, hb));
				BitBlt(hdcDest, nXDest, nYDest, nWidth, nHeight, bwDC, 0, 0, 0xB8074A);
				::DeleteObject(SelectObject(hdcDest, oldBrush));
			}
			
			VERIFY(DeleteDC(bwDC));
		}
		
		VERIFY(DeleteDC(hDC));
	}
}

// CreateReservedPalette	- Create a palette using the PC_RESERVED flag
//				  Limit the colors to 236 colors
// Returns			- Handle to a palette object
// hDIB				- Handle to a device-independent bitmap
//
HPALETTE CZBitmap::CreateReservedPalette(CDC *pDC)
{
	HPALETTE hPal = NULL;    // handle to a palette
	
	HBITMAP hBmp = (HBITMAP)this->GetSafeHandle();
	// get image properties
	//BITMAP bmp = { 0 };
	//::GetObject( hBmp, sizeof(BITMAP), &bmp );
	// allocate memory for extended image information
	BITMAPINFO &bmInfo= *(LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( &bmInfo, 0, sizeof(BITMAPINFO) + 8 );
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits(pDC->GetSafeHdc() , hBmp, 0, GetHeight(), 0, &bmInfo, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bmInfo.bmiHeader.biSizeImage + 4 ];

	res = GetDIBits( pDC->GetSafeHdc(), hBmp, 0,  bmInfo.bmiHeader.biHeight, pBits, &bmInfo, DIB_RGB_COLORS );

    
    int nWidth = bmInfo.bmiHeader.biWidth;
    int nHeight = bmInfo.bmiHeader.biHeight;
 
	int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
					1 << bmInfo.bmiHeader.biBitCount;

	if( nColors > 256 ) 
		return NULL;		// No Palette   

	
	//allocate memory block for logical palette
	UINT nSize = sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * nColors);
	LOGPALETTE *pLP = (LOGPALETTE *) new BYTE[nSize];

	// Initialize the palette version
	pLP->palVersion = 0x300;

	// If it is a 256 color DIB, then let's find the most used 236 colors 
	// and make a palette out of those colors
	if (nColors > 236)
	{
		typedef struct _tagBESTCOLORS
		{
			DWORD dwColorCnt;	//Count of how many times a color is used
			BOOL  bDontUse;	//Should we use this color?
		} BESTCOLORS;
		
		BESTCOLORS bc[256];
		BYTE dwLeastUsed[20];		// Least used color indices
		LPSTR lpBits;			// pointer to D.I. bits of a DIB
		int   nWidth, nHeight, nBytesPerLine, cx, cy;   
		
		::ZeroMemory( bc, 256*sizeof(BESTCOLORS));
		
		lpBits = (LPSTR)(pBits + nColors);
		nWidth = bmInfo.bmiHeader.biWidth;
		nHeight = bmInfo.bmiHeader.biHeight;
		nBytesPerLine = ((((bmInfo.bmiHeader.biWidth * 
					bmInfo.bmiHeader.biBitCount) + 31) & ~31) / 8);
		
		// Traverse through all of the bits in the bitmap and place the 
		// color count of each color in the BESTCOLORS array
		for (cy = 0; cy < nHeight; cy++)
			for (cx = 0; cx < nWidth; cx++)
				bc[*(LPBYTE)(lpBits+cy*nBytesPerLine+cx)].dwColorCnt++;
			
		// Let's arbitrarily place the first few colors in the "Least Used" list.
		int nReject = nColors - 236;
		for (cx=0; cx < nReject; cx++)
		{
			bc[cx].bDontUse = TRUE;
			dwLeastUsed[cx] = cx;
		}
		
		// Now, let's traverse through all of the colors and 
		// sort out the least used
		for (cx=0; cx < nColors; cx++)
		{
			cy = 0;
			while ((!(bc[cx].bDontUse)) && cy < nReject)
			{
				if (bc[cx].dwColorCnt < bc[dwLeastUsed[cy]].dwColorCnt) 
				{     
					bc[dwLeastUsed[cy]].bDontUse = FALSE;
					dwLeastUsed[cy] = cx;
					bc[cx].bDontUse = TRUE;
				}
				cy++;
			}
		}
		
		// We want only 236 colors, so that the 20 system colors 
		// are left untouched
		pLP->palNumEntries = 236;
		
		cx = 0;
		for(int i = 0; i < nColors; i++)
		{
			// Should we use this color?
			if (!((bc[i].bDontUse)))
			{  
				pLP->palPalEntry[cx].peRed = bmInfo.bmiColors[i].rgbRed;
				pLP->palPalEntry[cx].peGreen = 
								bmInfo.bmiColors[i].rgbGreen;
				pLP->palPalEntry[cx].peBlue = bmInfo.bmiColors[i].rgbBlue;
				pLP->palPalEntry[cx].peFlags = PC_RESERVED;
				cx++;
			}
		}

	}
	else if (nColors)
	{
		// We have enough room for all the colors
		
		pLP->palNumEntries = nColors;
		
		// Copy the colors
		for(int i = 0; i < nColors; i++)
		{
			pLP->palPalEntry[i].peRed = bmInfo.bmiColors[i].rgbRed;
			pLP->palPalEntry[i].peGreen = bmInfo.bmiColors[i].rgbGreen;
			pLP->palPalEntry[i].peBlue = bmInfo.bmiColors[i].rgbBlue;
			pLP->palPalEntry[i].peFlags = PC_RESERVED;
		}
	}
	

	hPal = CreatePalette( pLP );
	delete[] pLP;
	
	// return handle to DIB's palette 
	return hPal;
}

// FadeColorToGrayScale	- Draws a bitmap in color slowly turns it to grayscale
// pDC				- Pointer to target device context
// hDIB				- Handle of device-independent bitmap
// xDest			- x-coordinate of upper-left corner of dest. rect. 
// yDest			- y-coordinate of upper-left corner of dest. rect. 
// nLoops			- How many loops to fade the image into color
// nDelay			- Delay in milli-seconds between each loop
//
void CZBitmap::FadeColorToGrayScale( CDC *pDC,int xDest, int yDest, int nLoops, 
									int nDelay ) 
{
	CPalette pal;
	CPalette *pOldPalette;
	PALETTEENTRY peAnimate[256];
	PALETTEENTRY peGray[256];
	PALETTEENTRY peOriginal[256];

	HBITMAP hBmp = (HBITMAP)this->GetSafeHandle();
	// get image properties
	//BITMAP bmp = { 0 };
	//::GetObject( hBmp, sizeof(BITMAP), &bmp );
	// allocate memory for extended image information
	BITMAPINFO &bmInfo= *(LPBITMAPINFO) new BYTE[ sizeof(BITMAPINFO) + 8 ];
	memset( &bmInfo, 0, sizeof(BITMAPINFO) + 8 );
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	// get extended information about image (length, compression, length of color table if exist, ...)
	DWORD res = GetDIBits(pDC->GetSafeHdc() , hBmp, 0, GetHeight(), 0, &bmInfo, DIB_RGB_COLORS );
	// allocate memory for image data (colors)
	LPBYTE pBits = new BYTE[ bmInfo.bmiHeader.biSizeImage + 4 ];

	res = GetDIBits( pDC->GetSafeHdc(), hBmp, 0,  bmInfo.bmiHeader.biHeight, pBits, &bmInfo, DIB_RGB_COLORS );

    int nColors = bmInfo.bmiHeader.biClrUsed ? bmInfo.bmiHeader.biClrUsed : 
                1 << bmInfo.bmiHeader.biBitCount;
    
    int nWidth = bmInfo.bmiHeader.biWidth;
    int nHeight = bmInfo.bmiHeader.biHeight;
    
    // Compute the address of the bitmap bits
    LPVOID lpDIBBits;
    if( bmInfo.bmiHeader.biBitCount > 8 )
        lpDIBBits = (LPVOID)((LPDWORD)(pBits + 
            bmInfo.bmiHeader.biClrUsed) + 
            ((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
    else
        lpDIBBits = (LPVOID)(pBits + nColors);
	
	int nReservedColors = nColors > 236 ? 236 : nColors;


	// Create the palette if needed
	if( pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE && nColors <= 256 )
	{
		// The device supports a palette and bitmap has color table
		
		HPALETTE hPal = CreateReservedPalette(pDC);
		pal.Attach( hPal );

		// Now save the original colors and get the grayscale colors
		int i=0;
		pal.GetPaletteEntries(0, nReservedColors, (LPPALETTEENTRY)&peOriginal);
		for( i=0; i < nReservedColors; i++)
		{
			int nGray = Grey( peOriginal[i].peRed, peOriginal[i].peGreen , peOriginal[i].peBlue );
			
			peGray[i].peRed = nGray;
			peGray[i].peGreen = nGray;
			peGray[i].peBlue = nGray;
		}


		
		// Select the palette
		pOldPalette = pDC->SelectPalette(&pal, FALSE);
		pDC->RealizePalette();
		
		::SetDIBitsToDevice(pDC->m_hDC, xDest, yDest, nWidth, nHeight, 0, 0, 0,
			nHeight, lpDIBBits, (LPBITMAPINFO)&bmInfo, DIB_RGB_COLORS);

		// Now animate palette to set the image to grayscale
		for( i=1; i <= nLoops; i++ )
		{
			for (int j = 0; j< nColors; j++) 
			{  
				peAnimate[j].peRed = peOriginal[j].peRed - 
					((peOriginal[j].peRed -peGray[j].peRed)*i)/nLoops;
				peAnimate[j].peGreen = peOriginal[j].peGreen - 
					((peOriginal[j].peGreen-peGray[j].peGreen)*i)
						/nLoops;
				peAnimate[j].peBlue = peOriginal[j].peBlue  - 
					((peOriginal[j].peBlue -peGray[j].peBlue)*i)/nLoops;

				peAnimate[j].peFlags = peOriginal[j].peFlags;
			}

			pal.AnimatePalette(0, nColors, (LPPALETTEENTRY)&peAnimate);

			// Delay...
			Sleep(nDelay);
		}

		// Select the old palette back
		pDC->SelectPalette(pOldPalette, FALSE);
	}
	else if( (pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) == 0 && nColors <= 256 )
	{
		// Now change the image to grayscale
		for(int i=1; i <= nLoops; i++ )
		{
 
			BYTE *dst=(BYTE*)lpDIBBits;
			int Size=nWidth*nHeight;

			while ( Size-- )
			{
				int nGrey = Grey( dst[2], dst[1], dst[0] );

				dst[2]=(BYTE)dst[2] -((dst[2] -nGrey)*i)/nLoops;
				dst[1]=(BYTE)dst[1] -((dst[1] -nGrey)*i)/nLoops;
				dst[0]=(BYTE)dst[0] -((dst[0] -nGrey)*i)/nLoops;
				dst+=4;
			}

			// Draw the image again
			::SetDIBitsToDevice(pDC->m_hDC, xDest, yDest, nWidth, nHeight, 0, 
				0, 0, nHeight, lpDIBBits, (LPBITMAPINFO)&bmInfo, 
				DIB_RGB_COLORS);

			// Delay...
			Sleep(nDelay);
		}
	}
	else
	{
		::SetDIBitsToDevice(pDC->m_hDC, xDest, yDest, nWidth, nHeight, 0, 0, 0,
			nHeight, lpDIBBits, (LPBITMAPINFO)&bmInfo, DIB_RGB_COLORS);
	}
}

void CZBitmap::AlphaDisplay(CDC  *pDC, BYTE bAlpha)
{
	CDC dc;
	dc.CreateCompatibleDC( pDC );
	CBitmap * bmp = dc.SelectObject( this );
      
	BLENDFUNCTION rBlendProps;
	rBlendProps.BlendOp = AC_SRC_OVER;
	rBlendProps.BlendFlags = 0;
	rBlendProps.AlphaFormat = 0;   
	rBlendProps.SourceConstantAlpha = bAlpha;
	BITMAP bmInfo;
	::GetObject(m_hObject, sizeof(BITMAP), &bmInfo );
	INT nWidth, nHeigh;
	nWidth = bmInfo.bmWidth;
	nHeigh = bmInfo.bmHeight;

    AlphaBlend(pDC->m_hDC, 0, 0, nWidth, nHeigh, dc.m_hDC , 0, 0, 
        nWidth, nHeigh, rBlendProps );
	dc.SelectObject( bmp );	
}

/* ExtCreateRegion replacement */
HRGN CZBitmap::CreateRegionExt(DWORD nCount, CONST RGNDATA *pRgnData )
{
		HRGN hRgn=CreateRectRgn(0, 0, 0, 0);
		const DWORD RDHDR = sizeof(RGNDATAHEADER);
		ASSERT( hRgn!=NULL );
		LPRECT pRects = (LPRECT)((LPBYTE)pRgnData + RDHDR);
		for(int i=0;i<(int)nCount;i++)
		{
			HRGN hr=CreateRectRgn(pRects[i].left, pRects[i].top, pRects[i].right, pRects[i].bottom);
			VERIFY(CombineRgn(hRgn, hRgn, hr, RGN_OR)!=ERROR);
			if (hr) 
				::DeleteObject(hr);
		}
		ASSERT( hRgn!=NULL );
		return hRgn;
}

///////////////////////////////////////////////////////////////////
// InflateRegion - Inflates a region by the x and y values
// specified in nXInflate and nYInflate
// Creates a new region that represents the inflated region
// (retains the contents of the old region)
// Returns NULL if unsuccessfull
HRGN CZBitmap::InflateRegion(HRGN hRgn, int nXInflate, int nYInflate)
{
	// Local Variables
	LPRGNDATA lpData;	// The RGNDATA structure
	LPRECT lpRect;		// Pointer to the array of RECT structures
	DWORD BufSize;		// The amount of memory required
	DWORD i;			// General index variable
	HRGN hRgnNew;		// The newly created region

	// Get the number of rectangles in the region
	BufSize = GetRegionData(hRgn, 0, NULL);
	if(BufSize == 0)
		return NULL;
	// Allocate memory for the RGNDATA structure
	lpData = (LPRGNDATA)malloc(BufSize);
	// Set the location of the RECT structures
	lpRect = (LPRECT)(lpData->Buffer);
	// Get the region data
	if(!GetRegionData(hRgn, BufSize, lpData))
	{
		free(lpData);
		return NULL;
	}
	// Expand (or contract) all the rectangles in the data
	for(i=0; i<lpData->rdh.nCount; i++)
		InflateRect(&lpRect[i], nXInflate, nYInflate);
	// Create the new region
	hRgnNew = CreateRegionExt(lpData->rdh.nCount, lpData);
	free((void*)lpData);
	return hRgnNew;
}

BOOL CZBitmap::StretchDraw(CDC *pDC, LPRECT r, LPRECT sr )
{
	if ( !r ) 
		return FALSE;
	CDC dc;
	dc.CreateCompatibleDC( pDC );
	CBitmap * bmp = dc.SelectObject( this );
	//pDC->SetStretchBltMode(COLORONCOLOR);
	
	if ( !sr )
		pDC->StretchBlt( r->left, r->top, r->right, r->bottom, &dc, 0, 0, GetWidth(), GetHeight() ,
			SRCCOPY );
	else
		pDC->StretchBlt( r->left, r->top, r->right - r->left, r->bottom - r->top, &dc, sr->left, sr->top, 
		sr->right - sr->left, sr->bottom - sr->top,
			SRCCOPY );

	
	dc.SelectObject( bmp );	
	return TRUE;	

}

BOOL CZBitmap::DrawImage(CZBitmap &bmp,int nX,int nY,int nCol,int nRow)
{
	nX-=1;
	nY-=1;
	//单个图片的长和宽
	int w = GetWidth()/nCol;
	int h = GetHeight()/nRow;
	
	CBitmap *OldBmp;
	CDC memDC;
	CClientDC dc(0);

	memDC.CreateCompatibleDC(&dc);
	bmp.CreateCompatibleBitmap(&dc, w, h);
	OldBmp = memDC.SelectObject( &bmp );

	StretchDraw(&memDC, CRect( 0, 0, w, h ),CRect(GetWidth()*nX/nCol, GetHeight()*nY/nRow, GetWidth()*nX/nCol+w ,GetHeight()*nY/nRow+h ));

	memDC.SelectObject(OldBmp);
	return TRUE;
}

//BOLL GetBmp()

CRect CZBitmap::GetRect()
{
	return CRect(0,0,GetWidth(),GetHeight());
}

HBITMAP CZBitmap::SetBitmap(HBITMAP hBitmap)
{
	CZBitmap *pBmp=(CZBitmap *)CZBitmap::FromHandle(hBitmap);
	HBITMAP hBmp=(HBITMAP)this->Detach(); 
	this->DeleteObject(); 
	pBmp->DrawImage(*this,1,1,1,1); 
	return hBmp;	
}

BOOL CZBitmap::ExtendDraw(CDC *pDC,CRect rc, int nX, int nY,BOOL bTran, UINT colorTransparent)
{
	CZBitmap bmp;	
	if (ExtendDrawImage(bmp,rc,nX,nY))
	{
		if(bTran)
			bmp.TransparentBlt(*pDC,&rc,colorTransparent);
		else
			bmp.Draw(pDC,&rc); 
		// 
		return TRUE;
	}
	return FALSE;
}

BOOL CZBitmap::ExtendDrawImage(CZBitmap &bmp,CRect rc, int nX, int nY)
{
	CBitmap *OldBmp;
	CDC memDC;
	CClientDC cdc(0);

	memDC.CreateCompatibleDC(&cdc);
	bmp.CreateCompatibleBitmap(&cdc, rc.Width() , rc.Height() );
	OldBmp = memDC.SelectObject( &bmp );

	if (nX==0 && nY==0)
	{
		StretchDraw(&memDC,&rc,GetRect());
		return TRUE;
	}
	CDC dc;
	dc.CreateCompatibleDC(&memDC);
	CBitmap * Bmp = dc.SelectObject( this );
	//dc.SetStretchBltMode(COLORONCOLOR);
	if (nX!=0 && nY==0)
	{

		//左上角
		memDC.BitBlt( 0, 0, nX, rc.Height(), &dc, 0, 0,
				SRCCOPY );
		memDC.StretchBlt(nX, 0, rc.Width()-GetWidth(), rc.Height() ,  &dc,nX, 0, 1, GetHeight(),
				SRCCOPY );
		//右上角
		memDC.BitBlt(rc.right-(GetWidth()-nX), 0, GetWidth()-nX, rc.Height() ,  &dc,nX, 0, 
				SRCCOPY );

	}
	else if (nX==0 && nY!=0)
	{
		//上角
		memDC.BitBlt( 0, 0, rc.Width(), nY, &dc, 0, 0,
				SRCCOPY );
		memDC.StretchBlt(0, nY, GetWidth(), rc.Height()-GetHeight(),   &dc,0, nY, GetWidth(), 1,
				SRCCOPY );
		//右上角
		memDC.BitBlt(0, rc.bottom-(GetHeight()-nY), GetWidth(), GetHeight()-nY ,  &dc,0, nY, 
				SRCCOPY );
	
	}
	else
	{
		//左上角
		memDC.StretchBlt( 0, 0, nX, nY, &dc, 0, 0, nX, nY ,
				SRCCOPY );
		//上中
		memDC.StretchBlt(nX, 0, rc.Width()-GetWidth(),nY, &dc, nX, 0, 1, nY ,
				SRCCOPY );
		//右上角
		memDC.StretchBlt(rc.Width()-(GetWidth()-nX), 0, GetWidth()-nX, nY ,  &dc,nX, 0, GetWidth()-nX, nY,
				SRCCOPY );
		//左中
		memDC.StretchBlt(0, nY, nX,rc.Height()-GetHeight(), &dc, 0, nY, nX, 1 ,
				SRCCOPY );
		//正中
		memDC.StretchBlt(nX, nY, rc.Width()-GetWidth(),rc.Height()-GetHeight(), &dc, nX, nY, 1, 1 ,
				SRCCOPY );
		//右中
		memDC.StretchBlt(rc.Width()-(GetWidth()-nX), nY, GetWidth()-nX,rc.Height()-GetHeight(), &dc, nX, nY, GetWidth()-nX, 1 ,
				SRCCOPY );

		//左下角
		memDC.StretchBlt( 0, rc.Height()-(GetHeight()-nY), nX, GetHeight()-nY, &dc, 0, nY, nX,GetHeight()-nY ,
				SRCCOPY );
		//下中
		memDC.StretchBlt(nX, rc.Height()-(GetHeight()-nY), rc.Width()-GetWidth(),GetHeight()-nY, &dc, nX, nY, 1, GetHeight()-nY ,
				SRCCOPY );
		//右下角
		memDC.StretchBlt( rc.Width()-(GetWidth()-nX), rc.Height()-(GetHeight()-nY), GetWidth()-nX, GetHeight()-nY, &dc, nX, nY, GetWidth()-nX,GetHeight()-nY ,
				SRCCOPY );
	}
	dc.SelectObject( Bmp );	
	memDC.SelectObject(OldBmp);

	return TRUE;
}