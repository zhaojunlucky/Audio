#pragma once
#include "..\ZLP\PlayerBase.h"
#include   <shlobj.h>   
#pragma   comment(lib,   "shell32.lib")  
/*
 * volume 
 * play mode
 * coverImage
 * add file play mode
 * lyric status
 */

#define ZCVolume L"volume"
#define ZCPMode L"playmode"
#define ZCCoverImg L"coverimg"
#define ZCAddFilePM L"adfilepm"
#define ZCLyric L"lyric"
#define ZCFontSize L"fontsize"
#define ZCLockCovImg L"lockcoverimage"

class CZSetting
{
public:
	CZSetting(void);
	~CZSetting(void);

	void SaveVolume(int v);
	void SavePlayMode(PlayMode pm);
	void SaveCoverImage(const CString &imgPath);
	void SaveLyricStatus(bool showLyric);
	void SaveAddFilePlayMode(bool v);
	void SaveFontSize(int fontSize);
	void SaveLockCoverImage(bool lock);

	bool ReadLockCoverImage();
	int ReadVolume(int defaultVolume = 5000);
	PlayMode ReadPlayMode(PlayMode defaultMode = PMCircle);
	void ReadCoverImage(CString &imgPath);
	bool ReadLyricStatus();
	bool ReadAddFilePlayMode();
	int ReadFontSize(int defaultSize = 25);
private:
	wchar_t path[MAX_PATH];
};

