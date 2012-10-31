// ZListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZListCtrl.h"
#include "ZGraphics.h"

// CZListCtrl

IMPLEMENT_DYNAMIC(CZListCtrl, CListCtrl)

CZListCtrl::CZListCtrl()
{
	m_crHighlight = RGB(254,242,205);
	m_crHighlightBorder = RGB(233,175,0);
	m_crHover = RGB(7,162,254);
	nHoverItem = -1;
	nLastHoverItem = -1;
	//m_bmpItemSelected.LoadImage(IDB_ITEMSELECT);

}

CZListCtrl::~CZListCtrl()
{

	//DeleteAllItems(); 
}


BEGIN_MESSAGE_MAP(CZListCtrl, CListCtrl)
	ON_WM_NCCALCSIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_LBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_MEASUREITEM_REFLECT()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// CZListCtrl message handlers


void CZListCtrl::SetHeight(int height)
{
	CRect rcWin;
	GetWindowRect(&rcWin);
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcWin.Width();
	wp.cy = rcWin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);
}

void CZListCtrl::SetSelectedBitmap(UINT id)
{
	m_bmpItemSelected.LoadImage(id);
}
void CZListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	ShowScrollBar(SB_HORZ,FALSE);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CZListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	lpLVCustomDraw->clrText = RGB(0,0,0); // white text

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;
}

int CZListCtrl::InsertItem(int nItem, LPCTSTR szItemText,ZCheckState checked)
{

	nItem = CListCtrl::InsertItem(nItem,szItemText);

	chkSts.push_back(checked);
	return nItem;
}

BOOL CZListCtrl::DeleteItem(int nItem)
{

	chkSts.erase(chkSts.begin()+nItem);
	return CListCtrl::DeleteItem(nItem);
}

BOOL CZListCtrl::DeleteAllItems()
{

	chkSts.clear();
    return CListCtrl::DeleteAllItems();  
}

void CZListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	//CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;

	// Save dc state
	int nSavedDC = pDC->SaveDC();

	// Get item image and state info
	LV_ITEM lvi;
	lvi.mask = LVIF_IMAGE | LVIF_STATE;
	lvi.iItem = nItem;
	lvi.iSubItem = 0;
	lvi.stateMask = 0xFFFF;		// get all state flags
	GetItem(&lvi);

	// Should the item be highlighted
	BOOL bHighlight =((lvi.state & LVIS_DROPHILITED)
				|| ( (lvi.state & LVIS_SELECTED)
					&& ((GetFocus() == this)
						|| (GetStyle() & LVS_SHOWSELALWAYS)
						)
					)
				);
				
	// Get rectangles for drawing
	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcBounds, rcLabel, rcIcon;
	GetItemRect(nItem, rcBounds, LVIR_BOUNDS);
	GetItemRect(nItem, rcLabel, LVIR_LABEL);
	GetItemRect(nItem, rcIcon, LVIR_ICON);
	CRect rcCol( rcBounds );


	
	/*printf("rcclient:%d,%d\n",rcClient.left,rcClient.right);
	printf("rcBounds:%d,%d\n",rcBounds.left,rcBounds.right);
	printf("rcLabel:%d,%d\n",rcLabel.left,rcLabel.right);*/

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);

	CString sLabel = GetItemText( nItem, 0 );

	// Labels are offset by a certain amount  
	// This offset is related to the width of a space character
	int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

	CRect rcHighlight;

	int nExt;
	switch( m_nHighlight )
	{
	case 0:
		nExt = pDC->GetOutputTextExtent(sLabel).cx + offset;
		rcHighlight = rcLabel;
		if( rcLabel.left + nExt < rcLabel.right )
			rcHighlight.right = rcLabel.left + nExt;
		break;
	case 1:
		rcHighlight = rcBounds;
		rcHighlight.left = rcLabel.left;
		break;
	default:
		rcHighlight = rcBounds;
		rcHighlight.left = rcBounds.left+1;
		rcHighlight.right = rcClient.right-2;
		break;
	}

	// »­±³¾°
	CRect rcIconBG(rcIcon);
	rcIconBG.right += 12;
	int glowSize = 0;
	CZBitmap bmpFaceBG;
	if( bHighlight )
	{
		rcHighlight.left = 0;
		rcHighlight.right = rcBounds.right;;
		

		//printf("rcHighlight:%d,%d\n",rcHighlight.left,rcHighlight.right);

		
		glowSize = 10;
	}

	// Draw normal and overlay icon
	pImageList = GetImageList(LVSIL_SMALL);
	if (pImageList)
	{
		UINT nOvlImageMask=lvi.state & LVIS_OVERLAYMASK;

		pImageList->Draw(pDC, lvi.iImage,
			CPoint(rcIcon.left+6, rcIcon.top+10),ILD_TRANSPARENT | nOvlImageMask );
	}
	
 
	// Draw item label - Column 0
	rcLabel.top += 5;
	
	rcLabel.left += offset/2 + 7;
	
	rcLabel.right -= offset;
	CZGraphics zg;
	zg.DrawGlowingText(pDC->GetSafeHdc(),sLabel.AllocSysString(),rcLabel,*(this->GetFont()),DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
		|DT_END_ELLIPSIS,glowSize);

	CRect chk(0,rcLabel.top,10+rcLabel.left,rcLabel.bottom);
	if(chkSts[nItem] == ZChecked)
		pDC->DrawFrameControl(&chk,DFC_BUTTON,DFCS_CHECKED);
	else if(chkSts[nItem] == ZUnChecked)
		pDC->DrawFrameControl(&chk,DFC_BUTTON,DFCS_PUSHED);
	else
		pDC->DrawFrameControl(&chk,DFC_BUTTON,DFCS_INACTIVE);


	// Draw labels for remaining columns
	LV_COLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH;

	if( m_nHighlight == 0 )		// Highlight only first column
	{
		pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
		pDC->SetBkColor(::GetSysColor(COLOR_WINDOW));
	}

	rcBounds.right = rcHighlight.right > rcBounds.right ? rcHighlight.right+10 :
		rcBounds.right + 10;
	rcCol.top = rcLabel.top;
	rcCol.bottom = rcLabel.bottom;
	rcCol.right = rcLabel.right ;
	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcCol.left = rcCol.right ;
		
		rcCol.right += lvc.cx;

		//Draw the background if needed
		//if( m_nHighlight == HIGHLIGHT_NORMAL )
		//	pDC->FillRect(rcCol, &CBrush(::GetSysColor(COLOR_WINDOW)));

		sLabel = GetItemText(nItem, nColumn);
		if (sLabel.GetLength() == 0)
			continue;

		// Get the text justification
		UINT nJustify = DT_LEFT;
		switch(lvc.fmt & LVCFMT_JUSTIFYMASK)
		{
		case LVCFMT_RIGHT:
			nJustify = DT_RIGHT;
			break;
		case LVCFMT_CENTER:
			nJustify = DT_CENTER;
			break;
		default:
			break;
		}

		rcLabel = rcCol;
		rcLabel.left += offset;
		rcLabel.right -= offset;
		//CZGraphics zg;
		BSTR bstr = sLabel.AllocSysString();
		zg.DrawGlowingText(pDC->GetSafeHdc(),bstr,rcLabel,*(this->GetFont()),DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
			|DT_END_ELLIPSIS,glowSize);
		SysFreeString(bstr);
	}
	// Restore dc
	pDC->RestoreDC( nSavedDC );

  
}


void CZListCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	nHoverItem = HitTest(point);
	if(nLastHoverItem >= 0 )
	{
		CRect rcItem;
		GetItemRect(nLastHoverItem,rcItem,LVIR_BOUNDS);
		InvalidateRect(rcItem);
	}
	nLastHoverItem = nHoverItem;
	if (nHoverItem >= 0)
	{
		CRect rcItem;
		GetItemRect(nHoverItem,rcItem,LVIR_BOUNDS);
		InvalidateRect(rcItem);
		//SetTimer(1,100,NULL);
	}
	CListCtrl::OnMouseMove(nFlags, point);
}


void CZListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	SetColumnWidth(0 ,cx);
	RedrawWindow();
}

void CZListCtrl::MeasureItem( LPMEASUREITEMSTRUCT lpMis )
{
	lpMis->itemHeight = 52;
}

void CZListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	int nItem = HitTest(point);
	if(nItem != -1)
	{
		CString strTip;
		CRect rcLabel;

		GetItemRect(nItem, rcLabel, LVIR_LABEL);
		CDC *pDC = this->GetDC();
		int offset = pDC->GetTextExtent(_T(" "), 1 ).cx*2;

		rcLabel.top += 5;

		rcLabel.left += offset/2 + 7;

		rcLabel.right -= offset;

		CRect chk(0,rcLabel.top,10+rcLabel.left,rcLabel.bottom);
		if(chk.PtInRect(point))
		{
			//0 for Checked 1 for not checked ,2 for disable
			//MessageBox(strTip);
			if(chkSts[nItem] == ZChecked)
			  chkSts[nItem] = ZUnChecked;
			else if(chkSts[nItem] == ZUnChecked)
			{
				chkSts[nItem] = ZChecked;
			}
		}
			
	}
	CListCtrl::OnLButtonDown(nFlags, point);
}


BOOL CZListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}


void CZListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages

	CRect rcClient;
	GetClientRect(&rcClient);

	if(zAeroHelper.IsAeroEnabled())
	{
		CDC memdc;
		memdc.CreateCompatibleDC(&dc);	
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
		memdc.SelectObject( &bitmap );

		memdc.FillSolidRect(rcClient,RGB(0,0,0));
		CWnd::DefWindowProc(WM_PAINT, (WPARAM)memdc.m_hDC , 0);

		dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &memdc, 
			0, 0,SRCCOPY);
	}
	else
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		dc.FillSolidRect(rcClient,GetSysColor( COLOR_WINDOW));
	}
}


void CZListCtrl::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default

	if (nIDEvent == 1)
	{
		CPoint pt(GetMessagePos()); ScreenToClient(&pt);
		CRect rc; GetClientRect(&rc);
		if (!rc.PtInRect(pt))
		{
			nHoverItem = -1;
			CRect rcItem;
			GetItemRect(nLastHoverItem,rcItem,LVIR_BOUNDS);
			InvalidateRect(rcItem);
			KillTimer(1);
		}
	}
	
	CListCtrl::OnTimer(nIDEvent);
}



void CZListCtrl::OnDestroy()
{
	chkSts.clear();
	CListCtrl::OnDestroy();
	// TODO: Add your message handler code here
}

ZCheckState CZListCtrl::GetItemState(unsigned int item)
{
	assert(item >=0 && item<chkSts.size());
	return chkSts[item];
}
void CZListCtrl::SetItemState(unsigned int item,ZCheckState zchs)
{
	assert(item >=0 && item<chkSts.size());
	chkSts[item] = zchs;
}


void CZListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnLButtonDown(nFlags, point);
}
