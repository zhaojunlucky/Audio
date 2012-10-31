#include "StdAfx.h"
#include "Helper.h"


CHelper::CHelper(void)
{
}


CHelper::~CHelper(void)
{
}

void AddFileImage(CImageList *imgList,CString ext,COLORREF mask)
{
	assert(NULL != imgList && !ext.IsEmpty());
	SHFILEINFO   sfi;   
	SHGetFileInfo(ext,0,&sfi,sizeof(sfi),SHGFI_ICON|SHGFI_LARGEICON|SHGFI_USEFILEATTRIBUTES);
	int i=sfi.iIcon; 

	HICON hIcon=sfi.hIcon; 
	ICONINFO   info; 
	::GetIconInfo   (hIcon,   &info); 

	BITMAP   bmp; 
	::GetObject   (info.hbmColor,   sizeof   (BITMAP),   (LPVOID)   &bmp); 

	HBITMAP   hBmp   =   (HBITMAP)   ::CopyImage   (info.hbmColor,   IMAGE_BITMAP,   0,   0,   0); 

	CBitmap mm;
	mm.Attach(hBmp);

	imgList->Add(&mm,mask);
	mm.Detach();
	//สอทลืสิด 
	::DeleteObject   (hBmp); 
	::DeleteObject   (info.hbmColor); 
	::DeleteObject   (info.hbmMask);
	DestroyIcon(hIcon);
}

