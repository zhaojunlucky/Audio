#include "StdAfx.h"
#include "ZFileVersion.h"


CZFileVersion::CZFileVersion(void)
{
}


CZFileVersion::~CZFileVersion(void)
{
}

void CZFileVersion::GetVersionInfo(const wchar_t *pszFilePath,CZVersionInfo *verInfo)
{
	memset(verInfo,0,sizeof(CZVersionInfo));
	int nRet = _waccess(pszFilePath,0);
	if(nRet != -1)
	{
		DWORD               dwSize              = 0;
		BYTE                *pbVersionInfo      = NULL;
		VS_FIXEDFILEINFO    *pFileInfo          = NULL;
		UINT                puLenFileInfo       = 0;

		// get the version info for the file requested
		dwSize = GetFileVersionInfoSize( pszFilePath, NULL );
		if ( dwSize == 0 )
		{				
			return;
		}

		pbVersionInfo = new BYTE[ dwSize ];

		if ( !GetFileVersionInfo( pszFilePath, 0, dwSize, pbVersionInfo ) )
		{
			delete[] pbVersionInfo;
			return;
		}

		if ( !VerQueryValue( pbVersionInfo, TEXT("\\"), (LPVOID*) &pFileInfo, &puLenFileInfo ) )
		{
			delete[] pbVersionInfo;
			return;
		}

		verInfo->fileLeftMost =  HIWORD( pFileInfo->dwFileVersionMS);
		verInfo->fileSecondLeft = LOWORD( pFileInfo->dwFileVersionMS);
		verInfo->fileSecondRight = HIWORD( pFileInfo->dwFileVersionLS);
		verInfo->fileRightMost = LOWORD( pFileInfo->dwFileVersionLS);

		verInfo->productLeftMost =  HIWORD( pFileInfo->dwProductVersionMS);
		verInfo->productSecondLeft = LOWORD( pFileInfo->dwProductVersionMS);
		verInfo->productSecondRight = HIWORD( pFileInfo->dwProductVersionLS);
		verInfo->productRightMost = LOWORD( pFileInfo->dwProductVersionLS);
		delete[] pbVersionInfo;
	}
}

void CZFileVersion::GetVersionInfo(const wchar_t fileDir[MAX_PATH],const wchar_t fileName[MAX_PATH],CZVersionInfo *zverInfo)
{
	wchar_t filePath[MAX_PATH*2];
	wcscpy_s(filePath,MAX_PATH,fileDir);
	wcscat_s(filePath,MAX_PATH,fileName);

	GetVersionInfo(filePath,zverInfo);
}