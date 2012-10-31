#pragma once
#include "Base.h"
class CCharWCharHelper
{
public:
	__declspec(dllexport)  CCharWCharHelper(const char *);
	__declspec(dllexport)  CCharWCharHelper(const wchar_t*);
	__declspec(dllexport)  CCharWCharHelper();
	__declspec(dllexport)  ~CCharWCharHelper(void);

	__declspec(dllexport)  size_t ZMultiByteToWideChar(const char*,ZTextEncoding = ZASCII);
	__declspec(dllexport)  size_t ZWideCharToMultiByte(const wchar_t *);

	__declspec(dllexport)  size_t GetMultiByteLength();
	__declspec(dllexport)  size_t GetWideCharLength();
	__declspec(dllexport)  const char *GetMultiChar();
	__declspec(dllexport)  const wchar_t *GetWideChar();

	
private:
	char *multiChar;
	wchar_t *wideChar;

	size_t multiCharLength;
	size_t wideCharLength;

	size_t multiCharAlloc;
	size_t wideCharAlloc;
};

