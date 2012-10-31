#pragma once
class CFileHelper
{
public:
	__declspec(dllexport) CFileHelper(void);
	__declspec(dllexport) ~CFileHelper(void);

	__declspec(dllexport) bool FileExist(const wchar_t *);
	__declspec(dllexport) bool FileExist(const char *);
	__declspec(dllexport) int Move(const wchar_t *srcFile,const wchar_t *desFile);
	__declspec(dllexport) int Move(const char *srcFile,const char *desFile);
};

