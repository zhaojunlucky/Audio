// ZPlayListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ZLControls.h"
#include "ZPlayListCtrl.h"
#include "..\ZLP\ZPlayList.h"
#include "ZGraphics.h"
#include "ZAeroHelper.h"

// CZPlayListCtrl

IMPLEMENT_DYNAMIC(CZPlayListCtrl, CListCtrl)

CZPlayListCtrl::CZPlayListCtrl()
{
	m_crHighlight = RGB(254,242,205);
	m_crHighlightBorder = RGB(233,175,0);
	m_crHover = RGB(7,162,254);
	CZAeroHelper zAero;
	BOOL opaque = FALSE;
	if(!zAero.GetColorizationColor(&bkColor,&opaque))
		bkColor = zAero.IsAeroEnabled()?RGB(0,0,0):GetSysColor(COLOR_WINDOW);

	nHoverItem = -1;
	nLastHoverItem = -1;
	//m_bmpItemSelected.LoadImage(IDB_ITEMSELECT);
	m_hTailIconCamera =NULL;
	m_hTailIconEmail=NULL;
	m_hTailIconMobile=NULL;
	//cleaBk = false;
}

CZPlayListCtrl::~CZPlayListCtrl()
{
	if(m_hTailIconCamera)
		::DestroyIcon(m_hTailIconCamera);
	if(m_hTailIconEmail)
		::DestroyIcon(m_hTailIconEmail);
	if(m_hTailIconMobile)
		::DestroyIcon(m_hTailIconMobile);
	//DeleteAllItems(); 
}


BEGIN_MESSAGE_MAP(CZPlayListCtrl, CListCtrl)
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
END_MESSAGE_MAP()



// CZPlayListCtrl message handlers


void CZPlayListCtrl::SetHeight(int height)
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

void CZPlayListCtrl::SetSelectedBitmap(UINT id)
{
	m_bmpItemSelected.LoadImage(id);
}
void CZPlayListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: Add your message handler code here and/or call default

	ShowScrollBar(SB_HORZ,FALSE);
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void CZPlayListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	LPNMLVCUSTOMDRAW lpLVCustomDraw = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);

	lpLVCustomDraw->clrText = RGB(0,0,0); // white text

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;
	*pResult |= CDRF_NOTIFYITEMDRAW;
	*pResult |= CDRF_NOTIFYSUBITEMDRAW;

}

int CZPlayListCtrl::InsertItem(int nItem, LPCTSTR szItemText, int nImageIndex,const Media* media)
{
	if(nImageIndex <0 || nImageIndex > 11)
		nImageIndex = 11;
	nItem = CListCtrl::InsertItem(nItem,szItemText,nImageIndex);
	if(media)
	{		
		Media *m = new Media(*media);
		CListCtrl::SetItemData(nItem,(DWORD)m);
	}
	return nItem;
}

BOOL CZPlayListCtrl::DeleteItem(int nItem)
{
	Media *p = (Media *)CListCtrl::GetItemData(nItem);  
	if(p)
		delete p;
	return CListCtrl::DeleteItem(nItem);
}

BOOL CZPlayListCtrl::DeleteAllItems()
{
    for(int i=0; i<GetItemCount(); i++)  
    {  
        Media *p = (Media *)CListCtrl::GetItemData(i);  
        if(p)  
		{  
            delete p;  
        }
    }  
    return CListCtrl::DeleteAllItems();  
}

void CZPlayListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	pDC->SetBkMode(TRANSPARENT);
	//pDC->SetBkColor(bkColor);
	//CRect rcItem(lpDrawItemStruct->rcItem);
	int nItem = lpDrawItemStruct->itemID;
	CImageList* pImageList;
	pDC->SetBkColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
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
	//CRect rcIconBG(rcIcon);
	//rcIconBG.right += 12;
	int glowSize =0;
	if(bHighlight)
	{
		rcHighlight.left = 0;
		rcHighlight.right = rcBounds.right;;
		glowSize = 10;
		pDC->FillSolidRect(rcHighlight,RGB(99,90,50));
	}

	/*if(cleaBk)
	{
		Graphics * graph = new Graphics(pDC->m_hDC);
		graph->Clear(Color(0,0,0));
		delete graph;
		cleaBk = false;
	}*/

	
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
	BSTR bstr;
	bstr = sLabel.AllocSysString();
	zg.DrawGlowingText(pDC->GetSafeHdc(),bstr,rcLabel,*(this->GetFont()),DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
		|DT_END_ELLIPSIS,glowSize);
	SysFreeString(bstr);


	Media *p = (Media*)CListCtrl::GetItemData(nItem);

	CSize size = pDC->GetTextExtent(sLabel);
	if(p != 0)
	{
		sLabel = p->aritst;

		rcLabel.top = rcLabel.top + size.cy + 3;
		rcLabel.right = rcClient.right;
	
		bstr = sLabel.AllocSysString();
		zg.DrawGlowingText(pDC->GetSafeHdc(),bstr,rcLabel,*(this->GetFont()),DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
		|DT_END_ELLIPSIS,glowSize);
		SysFreeString(bstr);

		rcLabel.top = rcLabel.top + size.cy + 3;
		rcLabel.right = rcClient.right;
		sLabel = p->amblum;
		bstr = sLabel.AllocSysString();
		zg.DrawGlowingText(pDC->GetSafeHdc(),bstr,rcLabel,*(this->GetFont()),DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
		|DT_END_ELLIPSIS,glowSize);
		SysFreeString(bstr);
	}

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

	for(int nColumn = 1; GetColumn(nColumn, &lvc); nColumn++)
	{
		rcCol.left = rcCol.right;
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
		bstr = sLabel.AllocSysString();
		zg.DrawGlowingText(pDC->GetSafeHdc(),bstr,rcLabel,*(this->GetFont()),DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP
		|DT_END_ELLIPSIS,glowSize);
		SysFreeString(bstr);
	}
	
	// Restore dc
	pDC->RestoreDC( nSavedDC );

  
}

void CZPlayListCtrl::SetBkColor(DWORD color)
{
	/*if(RGB(0,0,0) == bkColor)
		bkColor = color;
	else 
		bkColor = RGB(0,0,0);*/
	bkColor = color;
	//CListCtrl::SetBkColor(color);
}

CRect CZPlayListCtrl::GetTailIconRect(int nItem,DWORD dwServeFlag)
{
	CRect rcIcon,rcClient;
	GetItemRect(nItem,rcIcon,LVIR_ICON);
	GetClientRect(&rcClient);

	DWORD nStyle = GetItemData(nItem);
	switch(nStyle & dwServeFlag)
	{
	case TVS_CAMERA:
		{
			rcIcon.left = rcClient.right - 22;
			rcIcon.right = rcIcon.left +18;
			rcIcon.bottom = rcIcon.top +18;
		}
		break;
	case TVS_EMAIL:
		{
			rcIcon.left = rcIcon.right + 10;
			rcIcon.right = rcIcon.left +18;
			rcIcon.top = rcIcon.bottom - 20;
			rcIcon.bottom  = rcIcon.top + 18;
		}
		break;
	case TVS_MOBILE:
		{
			rcIcon.left = rcIcon.right + 10;
			if (nStyle & TVS_EMAIL)
				rcIcon.left += 18;
			rcIcon.right = rcIcon.left +18;
			rcIcon.top = rcIcon.bottom - 20;
			rcIcon.bottom  = rcIcon.top + 18;
		}
		break;
	default:
		rcIcon.SetRectEmpty();
		break;
	}
	return rcIcon;
}

void CZPlayListCtrl::OnMouseMove(UINT nFlags, CPoint point)
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


void CZPlayListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	SetColumnWidth(0 ,cx);
	RedrawWindow();
}

void CZPlayListCtrl::MeasureItem( LPMEASUREITEMSTRUCT lpMis )
{
	lpMis->itemHeight = 52;
}

void CZPlayListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	/*int nItem = HitTest(point);
	if(nItem != -1)
	{
		//CString strTip;
		CRect rcServe;
		for(DWORD dwFlag = TVS_CAMERA;dwFlag < TVS_ONLINEUSER;dwFlag++)
		{
			rcServe = GetTailIconRect(nItem,dwFlag);
			if(rcServe.PtInRect(point))
			{
				//strTip.Format(L"Î»ÖÃ:nItem=%d,dwServeFlag=%#0.8x",nItem,dwFlag);
				break;
			}
		}
		if(!strTip.IsEmpty())
			MessageBox(strTip);
	}*/
	CListCtrl::OnLButtonDown(nFlags, point);
}


BOOL CZPlayListCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	//cleaBk = true;
	return TRUE;
}


void CZPlayListCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CListCtrl::OnPaint() for painting messages

	CRect rcClient;
	GetClientRect(&rcClient);

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


void CZPlayListCtrl::OnTimer(UINT_PTR nIDEvent)
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



void CZPlayListCtrl::OnDestroy()
{
	for(int i=0; i<GetItemCount(); i++)  
    {  
        Media *p = (Media *)CListCtrl::GetItemData(i);  
        if(p)  
		{  
            delete p;  
        }
    }  
	CListCtrl::OnDestroy();
	// TODO: Add your message handler code here
}
