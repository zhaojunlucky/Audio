#include "StdAfx.h"
#include "DirectoryHelper.h"
#include <io.h>
#include <sys/stat.h>
#include <malloc.h>


CDirectoryHelper::CDirectoryHelper(void)
{
}


CDirectoryHelper::~CDirectoryHelper(void)
{
	fileList.clear();
}


bool CDirectoryHelper::DirectoryExist(const wchar_t *dir)
{
	if(!dir)
		return false;
	/*char * cfile = 0;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,dir,-1,NULL,0,NULL,FALSE);
	cfile = new char[dwNum + 1];
	WideCharToMultiByte (CP_OEMCP,NULL,dir,-1,cfile,dwNum,NULL,FALSE);
	cfile[dwNum] = '\0';
	bool exist = DirectoryExist(cfile);
	delete cfile;
	return exist;*/
	if(-1 != _waccess(dir,0))
		return true;
	return false;
}
bool CDirectoryHelper::DirectoryExist(const char *dir)
{
	if(!dir)
		return false;
	if(-1 != _access(dir,0))
		return true;
	return false;
}

bool CDirectoryHelper::IsDirectory(const wchar_t* name)
{
	 if(!name)
		return false;
	char * cfile = 0;
	int len = wcslen(name);
	DWORD dwNum = WideCharToMultiByte(/*CP_OEMCP*/CP_ACP,NULL,name,len,NULL,0,NULL,FALSE);
	cfile = new char[dwNum + 1];
	WideCharToMultiByte (/*CP_OEMCP*/CP_ACP,NULL,name,len+1,cfile,dwNum+1,NULL,FALSE);
	cfile[dwNum] = '\0';
	bool dir = IsDirectory(cfile);
	delete cfile;
	return dir;
}

bool CDirectoryHelper::IsDirectory(const char* name)
{
	if(NULL == name)
		return false;
	if(!DirectoryExist(name))
		return false;

	struct stat st;
	stat(name,&st);
	if(st.st_mode & _S_IFDIR)
		return true;
	return false;
}

bool CDirectoryHelper::DirPathEndWithSepatrator(const char* dir)
{
	if(!IsDirectory(dir))
		return false;
	int len = strlen(dir);
	return '\\' == dir[len-1];
}

bool CDirectoryHelper::DirPathEndWithSepatrator(const wchar_t*dir)
{
	if(!IsDirectory(dir))
		return false;
	int len = wcslen(dir);
	return L'\\' == dir[len-1];
}

unsigned int CDirectoryHelper::FindDirectoryFiles(const wchar_t* dir)
{
	fileList.clear();

	char * cfile = 0;
	DWORD dwNum = WideCharToMultiByte(/*CP_OEMCP*/936,NULL,dir,-1,NULL,0,NULL,FALSE);
	cfile = new char[dwNum + 1];
	WideCharToMultiByte (/*CP_OEMCP*/936,NULL,dir,-1,cfile,dwNum,NULL,FALSE);
	cfile[dwNum] = '\0';
	FindDirectoryFiles(cfile);
	delete cfile;
	return fileList.size();
}

unsigned int CDirectoryHelper::FindDirectoryFiles(const char* dir)
{
	fileList.clear();
	if(!IsDirectory(dir))
		return 0;
	char path[1024];
	strcpy_s(path,1024,dir);
	if(!DirPathEndWithSepatrator(dir))
		strcat_s(path,1024,"\\");
	string s_dir(path);
	strcat_s(path,1024,"*.*");

	long handle = -1;
	struct _finddata_t fileinfo;
	handle=_findfirst(path,&fileinfo); 
    if(-1 == handle)
		return 0;
	string s_name(fileinfo.name);
	if(0 != strcmp(fileinfo.name,".") && 0 != strcmp(fileinfo.name,".."))
		fileList.push_back(s_dir+s_name);
	while(!_findnext(handle,&fileinfo))
	{
		s_name.clear();
		if(0 != strcmp(fileinfo.name,".") && 0 != strcmp(fileinfo.name,".."))
		{
			s_name = fileinfo.name;
			fileList.push_back(s_dir+s_name);
		}		
	}
	_findclose(handle); 
	s_dir.clear();
	s_name.clear();
	return fileList.size();
}

unsigned int CDirectoryHelper::GetSize()
{
	return fileList.size();
}
const char* CDirectoryHelper::GetAt(unsigned int index)
{
	if(index < 0 || index >= fileList.size())
	{
		printf("directory helper getAt\n");
		throw exception("Index out of range!");
	}
	return fileList[index].c_str();
}

CZString CDirectoryHelper::GetModuleDir(HMODULE hModule)
{
	CZString str;
	wchar_t exeFullPath[MAX_PATH];
	GetModuleFileName(hModule, exeFullPath, MAX_PATH);
	size_t len = wcslen(exeFullPath) - 1;
	while(L'\\' != exeFullPath[len])
	{
		exeFullPath[len] = L'\0';
		len--;
	}
	//wcscat(exeFullPath,L"\\");
	str.SetData(exeFullPath,wcslen(exeFullPath));
	return str;
}

bool CDirectoryHelper::CreateDirectoryD(const wchar_t *dir)
{
	DDString::CZString str(dir,wcslen(dir));

	if(L':' != dir[1] || L'\\' != dir[2])
	{
		if(L'\\' == dir[0])
			str = GetModuleDir(0) + str;
		else
		{
			str = GetModuleDir(0) +L"\\"+ str;
		}
	}

	int index = str.IndexOf(L'\\',3);
	//DDString::CZStringW str;
	DDString::CZString strTmp;
	while(-1 != index)
	{
		strTmp = str.SubString(0,index);
		if(strTmp.GetDataLength() > 2 && !DirectoryExist(strTmp.GetData())) // exclude "\" directory, etc.:"\dir\"
		{
			if(!CreateDirectory(strTmp.GetData(),0))
				return false;
		}
		index = str.IndexOf(L'\\',index + 1);
	}
	
	return true;
}

bool CDirectoryHelper::CreateDirectoryD(const char *dir)
{
	CZStringW str;
	str.SetData(dir,strlen(dir));

	return CreateDirectoryD(str.GetData());
}


