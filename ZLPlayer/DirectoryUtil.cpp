#include "stdafx.h"
#include "DirectoryUtil.h"


CDirectoryUtil::CDirectoryUtil(void)
{
}


CDirectoryUtil::~CDirectoryUtil(void)
{
}

int CDirectoryUtil::FindFiles(const wchar_t* directory)
{
	m_files.RemoveAll();
	CFileFind fileFinder;
	CString filePath(directory);
	if(L'\\' != directory[wcslen(directory) - 1])
	{
		filePath += L"\\";
	}
	filePath += "*.*";
	BOOL hasFile = fileFinder.FindFile(filePath);

	while(hasFile)
	{
		hasFile = fileFinder.FindNextFileW();
		if(!fileFinder.IsDirectory() && !fileFinder.IsDots())
		{
			m_files.AddTail(fileFinder.GetFilePath());
		}
	}

	return m_files.GetCount();
}

int CDirectoryUtil::GetFileCount()
{
	return m_files.GetCount();
}

const wchar_t* CDirectoryUtil::GetFilePath(int index)
{
	assert(index >= 0 && index < GetFileCount());

	POSITION pos = m_files.FindIndex(index);

	return m_files.GetAt(pos);
}