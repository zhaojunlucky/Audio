#pragma once
#include "ZAeroHelper.h"

// CZSliderCtrl
#define ZSLIDERMESSAGE WM_USER + 1000

class CZSliderCtrl : public CSliderCtrl
{
	DECLARE_DYNAMIC(CZSliderCtrl)

public:
	__declspec(dllexport) CZSliderCtrl();
	__declspec(dllexport) virtual ~CZSliderCtrl();
	__declspec(dllexport) int GetPostionCursor();
	__declspec(dllexport) void SetMessageType(UINT);
	__declspec(dllexport) void SetBkColor(DWORD color);
protected:
	DECLARE_MESSAGE_MAP()
private:
	int mousePos;
	UINT message;
	HWND parent;
	bool isBtnDown;
	bool m_bDragging;
	bool m_bDragChanged;
	bool SetThumb(const CPoint& pt);
	void PostMessageToParent(const int nTBCode) const;
	void NoticePosition(int pos);
	DWORD bkColor;
	SolidBrush *brush1;
	SolidBrush *brush2;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


