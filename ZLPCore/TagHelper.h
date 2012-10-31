#pragma once

#include "..\TagLibrary\Tags.h"
#include "TagBase.h"



class CTagHelper
{
public:
	__declspec(dllexport) CTagHelper(void);
	__declspec(dllexport) ~CTagHelper(void);
public:
	__declspec(dllexport) int Link(const wchar_t *file);
	__declspec(dllexport) int GetTextTag(const wchar_t *tagName,wchar_t *tag);
	__declspec(dllexport) const wchar_t* GetTextTag(const wchar_t *tagName);
	__declspec(dllexport) void Destory();
	__declspec(dllexport) const HBITMAP GetBitmap(int *w,int *h);
	__declspec(dllexport) bool HasImage();
private:
	void Init();
	
	int CopyTagData(wchar_t *src,wchar_t **des,int);
private:
	wchar_t *m_title;
	wchar_t *m_artist;
	wchar_t *m_album;
	wchar_t *m_year;
	wchar_t *m_track;
	wchar_t	*m_genre;
	wchar_t *m_fileName;
	wchar_t *m_comments;
	wchar_t *m_uslt;

	Tags *tag;

	BMPSIZE m_bmpSize;
	HBITMAP hbmp;
};



