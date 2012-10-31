#include "StdAfx.h"
#include "ZSetting.h"
#include "..\Utility\DirectoryHelper.h"

CZSetting::CZSetting(void)
{
	SHGetSpecialFolderPath(NULL,path,CSIDL_APPDATA,FALSE);
	wcscat(path,L"\\DDJJ\\ZLPlayer\\");
	CDirectoryHelper dir;
	if(!dir.DirectoryExist(path))
		dir.CreateDirectoryD(path);
	wcscat(path,L"zlpddjj.dd");
}


CZSetting::~CZSetting(void)
{
}


void CZSetting::SaveVolume(int v)
{
	wchar_t buf[20];
	wsprintf(buf,L"%d",v);
	WritePrivateProfileString(L"ZLPlayer",ZCVolume,buf,path);
}
void CZSetting::SaveFontSize(int fontSize)
{
	wchar_t buf[20];
	wsprintf(buf,L"%d",fontSize);
	WritePrivateProfileString(L"ZLPlayer",ZCFontSize,buf,path);
}
void CZSetting::SavePlayMode(PlayMode pm)
{
	wchar_t buf[20];
	wsprintf(buf,L"%d",(int)pm);
	WritePrivateProfileString(L"ZLPlayer",ZCPMode,buf,path);
}
void CZSetting::SaveCoverImage(const CString &imgPath)
{
	WritePrivateProfileString(L"ZLPlayer",ZCCoverImg,imgPath,path);
}
void CZSetting::SaveLyricStatus(bool showLyric)
{
	WritePrivateProfileString(L"ZLPlayer",ZCLyric,showLyric?L"1":L"0",path);
}
void CZSetting::SaveAddFilePlayMode(bool v)
{
	WritePrivateProfileString(L"ZLPlayer",ZCAddFilePM,v?L"1":L"0",path);
}

void CZSetting::SaveLockCoverImage(bool lock)
{
	WritePrivateProfileString(L"ZLPlayer",ZCLockCovImg,lock?L"1":L"0",path);
}

bool CZSetting::ReadLockCoverImage()
{
	return( GetPrivateProfileInt(L"ZLPlayer",ZCLockCovImg,1,path) != 0 );
}

int CZSetting::ReadVolume(int defaultVolume)
{
	return GetPrivateProfileInt(L"ZLPlayer",ZCVolume,defaultVolume,path);
}
PlayMode CZSetting::ReadPlayMode(PlayMode defaultMode )
{
	int v = GetPrivateProfileInt(L"ZLPlayer",ZCPMode,2,path);
	if(v >= 0 && v <= 3)
		return (PlayMode)v;
	return defaultMode;
}
void CZSetting::ReadCoverImage(CString &imgPath)
{
	imgPath = "";
	wchar_t imgPaths[MAX_PATH];
	wcscpy(imgPaths,L"");
	GetPrivateProfileString(L"ZLPlayer",ZCCoverImg,L"",imgPaths,MAX_PATH,path);
	imgPath.Format(L"%s",imgPaths);
}
bool CZSetting::ReadLyricStatus()
{
	return( GetPrivateProfileInt(L"ZLPlayer",ZCLyric,1,path) != 0 );
}

bool CZSetting::ReadAddFilePlayMode()
{
	return( GetPrivateProfileInt(L"ZLPlayer",ZCAddFilePM,1,path) != 0 );
}

int CZSetting::ReadFontSize(int defaultSize)
{
	int fSize = GetPrivateProfileInt(L"ZLPlayer",ZCFontSize,defaultSize,path);
	if(fSize < 20 || fSize > 42)
		return defaultSize;
	return fSize;
}