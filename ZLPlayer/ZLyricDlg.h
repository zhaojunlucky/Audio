#pragma once

#include <GdiPlus.h>
#include "LRCBase.h"
#include "ZManager.h"
#include "..\ZLControls\ZGraphics.h"
#include "..\ZLControls\ZFileWatch.h"
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")
// CZLyricDlg dialog

class CZLyricDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CZLyricDlg)

public:
	CZLyricDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CZLyricDlg();

// Dialog Data
	enum { IDD = IDD_ZLYRICDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL UpdateDisplay(int Transparent=255);

	BLENDFUNCTION m_Blend;
	HDC m_hdcMemory;

	void ReadLRC(const Media &m);
	void PlayControlMessage(ZMessage message);
	void StartTimer(int Interval = 100);
	void StopTimer();

	void SetZManager(CZManager *pManager);
	void EnableLyric(bool enable);
	void ProcessFontSize();
private:
	BOOL m_bBack;
	//GdiplusStartupInput gdiplusStartupInput;
    //ULONG_PTR           gdiplusToken;
	wchar_t m_defaultMes[80];
	CZLRC lyricer;
	CZManager *pManager;
	ZMessage controlMes;

	long lastTagTime;
	long lastTagIndex; 
	bool showDefault;
	int screenX;
	int screenY;
	int taskH;
	CZGraphics zGraphics ;

	RECT rct;
	POINT ptWinPos;
	POINT ptSrc;
	FontFamily *fontFamily;
	Gdiplus::Font *font;
	int fontSize ;
	SizeF drawSize;
	SizeF drawSize2;
	SizeF orgSize;
	SIZE sizeWindow;

	Pen *pen;
	LinearGradientBrush *linGrBrush;
	LinearGradientBrush *linGrBrushW;
	bool showlyric;
	int yOffset;
	CRITICAL_SECTION  m_cs;
	bool timerStart;
	CZFileWatch *fileWatch;
	Media m;

	void static __stdcall  MyDIRCallback( ZDDIRAction act, wchar_t*, wchar_t*, void *client);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnPaint();
};
