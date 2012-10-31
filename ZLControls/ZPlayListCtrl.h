#pragma once


// CZPlayListCtrl
#include "..\ZLP\ZPlayList.h"
#include "ZBitmap.h"
#include <GdiPlus.h>

class CZPlayListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CZPlayListCtrl)

public:
	__declspec(dllexport) CZPlayListCtrl();
	__declspec(dllexport) virtual ~CZPlayListCtrl();

	enum {  TVS_CAMERA = 1 << 0, TVS_EMAIL = 1 << 1, TVS_MOBILE = 1 << 2,TVS_ONLINEUSER = 1 << 3};

	__declspec(dllexport) void SetBackBitmap(LPCTSTR lpszImage){};
	__declspec(dllexport) int InsertItem(int nItem, LPCTSTR szItemText, int nImageIndex, const Media* media = NULL);
	__declspec(dllexport) BOOL GetUserItemInfo(CString guid,Media* userInfo);
	__declspec(dllexport) CRect GetTailIconRect(int nItem,DWORD dwServeFlag);
	__declspec(dllexport) BOOL DeleteItem(int nItem);
    __declspec(dllexport) BOOL DeleteAllItems();  
	__declspec(dllexport) void SetHeight(int height);
	__declspec(dllexport) void SetSelectedBitmap(UINT id);
	__declspec(dllexport) void SetBkColor(DWORD);
protected:
	DECLARE_MESSAGE_MAP()
private:
	COLORREF	m_crHighlight;
	COLORREF	m_crHighlightBorder;
	COLORREF	m_crHover;
	CZBitmap	m_bitmap;
	int m_nHighlight;
	HICON m_hTailIconCamera;
	HICON m_hTailIconEmail;
	HICON m_hTailIconMobile;
	CZBitmap m_bmpItemSelected;
	CZBitmap m_bmpFaceBG;
	int nHoverItem;
	int nLastHoverItem;
	DWORD bkColor;
	//bool cleaBk;
public:
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDestroy();
	
	//virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};


