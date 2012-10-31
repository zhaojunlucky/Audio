#pragma once

#include "ZBitmap.h"
#include <vector>
#include "ZAeroHelper.h"

using namespace std;
enum ZCheckState
{
	ZChecked = 0,
	ZUnChecked ,
	ZDisable
};
// CZListCtrl

class CZListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CZListCtrl)

public:
	__declspec(dllexport) CZListCtrl();
	__declspec(dllexport) virtual ~CZListCtrl();

	__declspec(dllexport) void SetBackBitmap(LPCTSTR lpszImage){};
	__declspec(dllexport) int InsertItem(int nItem, LPCTSTR szItemText, ZCheckState checked = ZChecked);
	__declspec(dllexport) BOOL DeleteItem(int nItem);
	__declspec(dllexport) BOOL DeleteAllItems();  
	__declspec(dllexport) void SetHeight(int height);
	__declspec(dllexport) void SetSelectedBitmap(UINT id);
	__declspec(dllexport) ZCheckState GetItemState(unsigned int);
	__declspec(dllexport) void SetItemState(unsigned int item,ZCheckState);
protected:
	DECLARE_MESSAGE_MAP()
private:
	COLORREF	m_crHighlight;
	COLORREF	m_crHighlightBorder;
	COLORREF	m_crHover;
	CZBitmap	m_bitmap;
	int m_nHighlight;

	CZBitmap m_bmpItemSelected;
	CZBitmap m_bmpFaceBG;
	int nHoverItem;
	int nLastHoverItem;
	vector<ZCheckState> chkSts;
	//ULONG_PTR m_gdiplusToken; 
	CZAeroHelper zAeroHelper;
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
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


