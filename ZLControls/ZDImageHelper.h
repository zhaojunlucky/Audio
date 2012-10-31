#pragma once
#include "stdafx.h"

namespace DDImage
{
	class ZDImageHelper
	{
	public:
		static BOOL ZDLoadImage(CBitmap &bmp,HINSTANCE  hInst,UINT ID_RES, LPCWSTR lpType)
		{
			//HINSTANCE   hInst   =   AfxGetResourceHandle(); 
			HRSRC   hRsrc   =   ::FindResource   (hInst, MAKEINTRESOURCE(ID_RES), lpType);   //   type 
			if   (!hRsrc) 
			return   FALSE; 

			//   load   resource   into   memory 
			DWORD   len   =   SizeofResource(hInst,   hRsrc); 
			BYTE*   lpRsrc   =   (BYTE*)LoadResource(hInst,   hRsrc); 
			if   (!lpRsrc) 
			return   FALSE; 

			//   Allocate   global   memory   on   which   to   create   stream 
			HGLOBAL   m_hMem   =   GlobalAlloc(GMEM_FIXED,   len); 
			BYTE*   pmem   =   (BYTE*)GlobalLock(m_hMem); 
			memcpy(pmem,lpRsrc,len); 
			IStream*   pstm; 
			CreateStreamOnHGlobal(m_hMem,FALSE,&pstm); 

			//   load   from   stream这是关键一句，通过FromStream返回以各Image*，然后在Graphic的DrawImage地方调用就行了！ 
			CImage img;
			img.Load(pstm);
		
			if(bmp.m_hObject)
				bmp.Detach();
			bmp.Attach(img.Detach());

			//   free/release   stuff 
			GlobalUnlock(m_hMem); 
			pstm-> Release(); 
			FreeResource(lpRsrc);
			return TRUE;
		}
	};
}