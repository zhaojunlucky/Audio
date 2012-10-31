#include "StdAfx.h"
#include "ZParseCommandLine.h"

#include "..\Utility\DirectoryHelper.h"

CZParseCommandLine::CZParseCommandLine(void)
{
}


CZParseCommandLine::~CZParseCommandLine(void)
{
	fileList.RemoveAll();
}

unsigned int CZParseCommandLine::GetCount()
{
	return fileList.GetCount();
}

const CString CZParseCommandLine::GetAt(unsigned int index)
{
	assert(index >=0 && index < fileList.GetCount());
	return fileList.GetAt(index);
}

void CZParseCommandLine::ParseCommand()
{
	int nNumArgs = 0;
	PWSTR *ppArgv = CommandLineToArgvW(::GetCommandLineW(),&nNumArgs);

	if(nNumArgs > 1)
	{
		CDirectoryHelper dirHelper;
		for(int i = 1;i<nNumArgs;i++)
		{
			CString file(ppArgv[i]);
			if(!file.IsEmpty())
			{
				if(!dirHelper.IsDirectory(ppArgv[i]))
				{
					fileList.Add(file);
				}
				else
				{
					unsigned int count = dirHelper.FindDirectoryFiles(ppArgv[i]);
					for(unsigned int i = 0; i < count;i++)
					{
						file.Empty();
						file = dirHelper.GetAt(i);
						fileList.Add(file);
					}
				}
				file.Empty();
			}
		}
	}
}