#pragma once
#include "common.h"

#define ZPREVIOUS_COMMAND 10000
#define ZPLAY_PAUSE_COMMAND 10001
#define ZNEXT_COMMAND 10002
class CZTaskBarHelper
{
public:
	CZTaskBarHelper(UINT preDisable,UINT pre,UINT ppDisable,UINT ppPlay,UINT ppPause,UINT nextDisable,UINT next);
	~CZTaskBarHelper(void);

	bool AddThumbToolBar(int pre=0,int pp=0,int next=0);
	bool UpdateThumbToolBar(int pre=0,int pp=0,int next=0);

	PlayCommand OnCommand(WPARAM wParam, LPARAM lParam);
private:
	HICON m_previous[2];
	HICON m_playPause[3];
	HICON m_next[2];

	ITaskbarList4 *pTaskbar;
};

