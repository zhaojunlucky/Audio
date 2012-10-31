#include "StdAfx.h"
#include "CharWcharHelper.h"
#include <locale.h>
#include <Windows.h>


CCharWCharHelper::CCharWCharHelper()
{
	multiChar = new char[10];
	wideChar = new wchar_t[10];
	multiCharLength = wideCharLength = 0;
	multiCharAlloc = wideCharAlloc = 10;

}

CCharWCharHelper::CCharWCharHelper(const char *src)
{
	multiCharLength = wideCharLength = 0;
	multiCharAlloc = wideCharAlloc = 0;
	multiChar = 0;
	wideChar = 0;
	ZMultiByteToWideChar(src);
}

CCharWCharHelper::CCharWCharHelper(const wchar_t *src)
{
	multiCharLength = wideCharLength = 0;
	multiCharAlloc = wideCharAlloc = 0;
	multiChar = 0;
	wideChar = 0;
	ZWideCharToMultiByte(src);
}

CCharWCharHelper::~CCharWCharHelper(void)
{
	if(multiChar)
		delete multiChar;
	if(wideChar)
		delete wideChar;
}


size_t CCharWCharHelper::ZMultiByteToWideChar(const char*src,ZTextEncoding enc)
{
	if(0 == src)
		return 0;
	multiCharLength = 0;
	int code = CP_OEMCP;
	if(ZUTF8 == enc)
		code = CP_UTF8;
	wideCharLength = MultiByteToWideChar(code , 0, src, -1, NULL, 0);
	if(wideCharLength >= wideCharAlloc)
	{
		if(wideChar)
			delete wideChar;

		wideCharAlloc = wideCharLength +8;
		wideChar = new wchar_t[wideCharAlloc];
	}

	MultiByteToWideChar(code , 0, src, -1, wideChar, wideCharLength);
	wideChar[wideCharLength] = L'\0';

	return wideCharLength;
}

size_t CCharWCharHelper::ZWideCharToMultiByte(const wchar_t* src)
{
	wideCharLength = 0;
	if(0 == src)
		return 0;

	multiCharLength = WideCharToMultiByte(/*CP_OEMCP CP_ACP 936*/CP_ACP,NULL,src,wcslen(src),NULL,0,NULL,FALSE);
	if(multiCharLength >= multiCharAlloc)
	{
		if(multiChar)
			delete multiChar;	

		multiCharAlloc = multiCharLength +8;
		multiChar = new char[multiCharAlloc];
		
	}

	WideCharToMultiByte (/*CP_OEMCP CP_ACP 936*/CP_ACP,NULL,src,wcslen(src) ,multiChar,multiCharLength,NULL,FALSE);
	multiChar[multiCharLength] = '\0';
	return multiCharLength;
}

size_t CCharWCharHelper::GetMultiByteLength()
{
	return multiCharLength;
}

size_t CCharWCharHelper::GetWideCharLength()
{
	return wideCharLength;
}

const char *CCharWCharHelper::GetMultiChar()
{
	if(0 == multiCharLength)
		return 0;
	return multiChar;
}

const wchar_t*CCharWCharHelper::GetWideChar()
{
	if(0 == wideCharLength)
		return 0;
	return wideChar;
}