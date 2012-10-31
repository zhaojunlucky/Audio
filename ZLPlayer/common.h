#pragma once

enum PlayCommand
{
	MPlay,
	MPause,
	MResume,
	MStop,
	MSeek,
	MVolume,
	MMute,
	MNext,
	MPrevious,
	MNone
};

struct CZButtonStatus
{
	int tBarPreious;
	int tBarPlayPause;
	int tBarNext;

	bool previous;
	int  playPause;//0-play,1-pause,2-disabled
	bool next;
	int mes;
	int time;
};


#define WM_UPDATE_UI (WM_USER + 9000)
#define WM_UPDATE_PLAYLIST (WM_USER + 9001)
#define WM_UPDATE_UI_TIME_SILDER (WM_USER + 9002)
#define WM_LYRIC_UI_CONTROL (WM_USER + 9030)
#define WM_UPDATE_SETTING (WM_USER + 9040)


