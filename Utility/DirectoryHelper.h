#pragma once

#include <vector>
#include <string>
#include <exception>
#include "ZString.h"
using namespace DDString;
using namespace std;

class CDirectoryHelper
{
public:
	__declspec(dllexport) CDirectoryHelper(void);
	__declspec(dllexport) ~CDirectoryHelper(void);
	__declspec(dllexport)  CZString GetModuleDir(HMODULE hModule);
	__declspec(dllexport)  bool CreateDirectoryD(const wchar_t *dir);
	__declspec(dllexport)  bool CreateDirectoryD(const char *dir);
	__declspec(dllexport)  bool DirectoryExist(const wchar_t *);
	__declspec(dllexport)  bool DirectoryExist(const char *);
	__declspec(dllexport)  bool IsDirectory(const wchar_t*);
	__declspec(dllexport)  bool IsDirectory(const char*);
	__declspec(dllexport) unsigned int FindDirectoryFiles(const char*);
	__declspec(dllexport) unsigned int FindDirectoryFiles(const wchar_t*);
	__declspec(dllexport) bool DirPathEndWithSepatrator(const char*);
	__declspec(dllexport) bool DirPathEndWithSepatrator(const wchar_t*);
	__declspec(dllexport) unsigned int GetSize();
	__declspec(dllexport) const char* GetAt(unsigned int);
	
private:
	vector<string> fileList;
};

