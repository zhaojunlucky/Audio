#include "StdAfx.h"
#include "FileHelper.h"
#include <io.h>

CFileHelper::CFileHelper(void)
{
}


CFileHelper::~CFileHelper(void)
{
}

bool CFileHelper::FileExist(const wchar_t *file)
{
	if(!file)
		return false;
	/*char * cfile = 0;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,file,-1,NULL,0,NULL,FALSE);
	cfile = new char[dwNum + 1];
	WideCharToMultiByte (CP_OEMCP,NULL,file,-1,cfile,dwNum,NULL,FALSE);
	cfile[dwNum] = '\0';
	bool exist = FileExist(cfile);
	delete cfile;
	return exist;*/
	if(-1 != _waccess(file,0))
		return true;
	return false;
}
bool CFileHelper::FileExist(const char *file)
{
	if(!file)
		return false;
	if(-1 != _access(file,0))
		return true;
	return false;
}


int CFileHelper::Move(const wchar_t *srcFile,const wchar_t *desFile)
{
	if(MoveFile(srcFile,desFile))
		return 0;
	return 1;
}
int CFileHelper::Move(const char *srcFile,const char *desFile)
{
	if(MoveFileA(srcFile,desFile))
		return 0;
	return 0;
}

