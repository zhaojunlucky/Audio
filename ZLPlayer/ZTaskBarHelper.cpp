#include "StdAfx.h"
#include "ZTaskBarHelper.h"
#include <strsafe.h>

CZTaskBarHelper::CZTaskBarHelper(UINT preDisable,UINT pre,UINT ppDisable,UINT ppPlay,UINT ppPause,UINT nextDisable,UINT next)
{
	m_previous[0] =  AfxGetApp()->LoadIcon(preDisable);
	m_previous[1] =  AfxGetApp()->LoadIcon(pre);

	m_playPause[0] =  AfxGetApp()->LoadIcon(ppDisable);
	m_playPause[1] =  AfxGetApp()->LoadIcon(ppPlay);
	m_playPause[2] =  AfxGetApp()->LoadIcon(ppPause);

	m_next[0] =  AfxGetApp()->LoadIcon(nextDisable);
	m_next[1] =  AfxGetApp()->LoadIcon(next);

	pTaskbar = 0;
}


CZTaskBarHelper::~CZTaskBarHelper(void)
{
	::DestroyIcon(m_previous[0]);
	::DestroyIcon(m_previous[1]);

	::DestroyIcon(m_playPause[0]);
	::DestroyIcon(m_playPause[1]);
	::DestroyIcon(m_playPause[2]);

	::DestroyIcon(m_next[0]);
	::DestroyIcon(m_next[1]);
}

bool CZTaskBarHelper::AddThumbToolBar(int pre,int pp,int next)
{
	if(NULL == pTaskbar)
	{
		if(FAILED(CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pTaskbar))))
			return false;
	}

	THUMBBUTTONMASK dwMask = THB_ICON | THB_TOOLTIP |THB_FLAGS;
	THUMBBUTTON thbButtons[3];
	thbButtons[0].dwMask = dwMask;
	thbButtons[0].iId = ZPREVIOUS_COMMAND;
	thbButtons[0].dwFlags = (pre==0?THBF_DISABLED:THBF_ENABLED);
	thbButtons[0].hIcon = m_previous[pre];
	StringCbCopy(thbButtons[0].szTip, sizeof(thbButtons[0].szTip), TEXT("Previous"));

	thbButtons[1].dwMask = dwMask;
	thbButtons[1].iId = ZPLAY_PAUSE_COMMAND;
	thbButtons[1].dwFlags = (pp==0?THBF_DISABLED:THBF_ENABLED);
	thbButtons[1].hIcon = m_playPause[pp];
	StringCbCopy(thbButtons[1].szTip, sizeof(thbButtons[0].szTip), 2==pp?TEXT("Pause"):TEXT("Play"));

	thbButtons[2].dwMask = dwMask;
	thbButtons[2].iId = ZNEXT_COMMAND;
	thbButtons[2].dwFlags = (next==0?THBF_DISABLED:THBF_ENABLED);;
	thbButtons[2].hIcon = m_next[next];
	StringCbCopy(thbButtons[2].szTip, sizeof(thbButtons[0].szTip), TEXT("Next"));

	if(FAILED(pTaskbar->HrInit()))
		return false;
	if(FAILED(pTaskbar->ThumbBarAddButtons(::AfxGetApp()->GetMainWnd()->GetSafeHwnd(),ARRAYSIZE(thbButtons),thbButtons)))
		return false;
	return true;
}

bool CZTaskBarHelper::UpdateThumbToolBar(int pre,int pp,int next)
{
	if(NULL == pTaskbar)
		return false;
	THUMBBUTTONMASK dwMask = THB_ICON | THB_TOOLTIP |THB_FLAGS;
	THUMBBUTTON thbButtons[3];
	thbButtons[0].dwMask = dwMask;
	thbButtons[0].iId = ZPREVIOUS_COMMAND;
	thbButtons[0].dwFlags = (pre==0?THBF_DISABLED:THBF_ENABLED);
	thbButtons[0].hIcon = m_previous[pre];
	StringCbCopy(thbButtons[0].szTip, sizeof(thbButtons[0].szTip), TEXT("Previous"));

	thbButtons[1].dwMask = dwMask;
	thbButtons[1].iId = ZPLAY_PAUSE_COMMAND;
	thbButtons[1].dwFlags = (pp==0?THBF_DISABLED:THBF_ENABLED);
	thbButtons[1].hIcon = m_playPause[pp];
	StringCbCopy(thbButtons[1].szTip, sizeof(thbButtons[0].szTip), 2==pp?TEXT("Pause"):TEXT("Play"));

	thbButtons[2].dwMask = dwMask;
	thbButtons[2].iId = ZNEXT_COMMAND;
	thbButtons[2].dwFlags = (next==0?THBF_DISABLED:THBF_ENABLED);;
	thbButtons[2].hIcon = m_next[next];
	StringCbCopy(thbButtons[2].szTip, sizeof(thbButtons[0].szTip), TEXT("Next"));

	if(FAILED(pTaskbar->ThumbBarUpdateButtons( ::AfxGetApp()->GetMainWnd()->GetSafeHwnd(), ARRAYSIZE(thbButtons), thbButtons )))
		return false;
	return true;
}

PlayCommand CZTaskBarHelper::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT CommandID = LOWORD( wParam );
	if(ZPREVIOUS_COMMAND == CommandID)//previous
	{
		return MPrevious;
	}
	else if(ZPLAY_PAUSE_COMMAND == CommandID)// play or pause
	{
		//dlg->PlayBtn();
		return MPlay;
	}
	else if(ZNEXT_COMMAND == CommandID)//next
	{
		//dlg->OnBnClickedNext();
		return MNext;
	}
	return MNone;
}