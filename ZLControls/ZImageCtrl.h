#pragma once
#include <GdiPlus.h>
#include "..\ZLPCore\MyMutex.h"
// CZImageCtrl

class CZImageCtrl : public CStatic
{
	DECLARE_DYNAMIC(CZImageCtrl)

public:
	__declspec(dllexport) CZImageCtrl();
	__declspec(dllexport) virtual ~CZImageCtrl();
	__declspec(dllexport) void SetBitmap(const HBITMAP);
	__declspec(dllexport) void LoadFromFile(const wchar_t *);
protected:
	DECLARE_MESSAGE_MAP()
private:
	Gdiplus::Bitmap *pImg;
	//ULONG_PTR m_gdiplusToken;  
	CMyMutex m_mutex;
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


