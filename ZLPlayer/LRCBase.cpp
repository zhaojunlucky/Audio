#include "stdafx.h"
#include "LRCBase.h"
#include "..\Utility\ZStringHelper.h"
#include "..\Utility\FileHelper.h"
#include <algorithm>
#include "StdioFileEx.h"

long StrToSS(LPWSTR lpszTag)
{
	int paramNums = 0;
	int minutes;
	float seconds;
	paramNums = _stscanf(lpszTag, L"%d:%f", &minutes, &seconds);
	if(paramNums != 2)
	{
		//ASSERT(0);
		return -1;
	}
	return 60*minutes*1000 + seconds*1000;
}
int CompareTime(LRCTagTime& tag1, LRCTagTime& tag2)
{
	if(tag1.time > tag2.time)
		return 0;
	else
		return 1;
}

CZLRC::CZLRC()
{
	wchar_t exeFullPath[MAX_PATH];
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	/*int index = wcslen(exeFullPath) - 1;
	while(L'\\' != exeFullPath[index])
		index--;
	exeFullPath[index + 1] = L'\0';*/
	PathRemoveFileSpec(exeFullPath);
	wcscat(exeFullPath,L"\\ZDLRC\\");
	wcscpy(lrcDir,exeFullPath);
}

CZLRC::~CZLRC()
{
	Clear();
}

int CZLRC::ReadLRCFile(wchar_t *lrcFile)
{
	Clear();
	offset = 0;
	if(!ZStringHelper::CZStringHelper::EndWith(lrcFile,L"lrc"))
		return -2;
	CStdioFileEx file;
	
	if(!file.Open(lrcFile,CFile::modeRead|CFile::typeText))
		return -1;
	CString line;
	int num = 0;
	while(file.ReadString(line) != FALSE)
	{
		if(!line.IsEmpty())
		{
			//char *szBuf = new char[line.GetLength() + 1];
			//

			//int i;
			//for ( i = 0 ; i < line.GetLength(); i++)
			//{
			//	szBuf[i] = (char)line.GetAt(i);
			//}
			//szBuf[i] = '\0';

			//int nLen;
			//wchar_t *ptch;
			//CString strOut;
			//if(szBuf == NULL)
			//{
			//	continue;
			//}
			//nLen = MultiByteToWideChar(CP_OEMCP, 0, szBuf, -1, NULL, 0);//获得需要的宽字符字节数
			//ptch = new WCHAR[nLen];
			//memset(ptch, '\0', nLen);
			//MultiByteToWideChar(CP_ACP, 0, szBuf, -1, ptch, nLen);
			//line.Format(_T("%s"), ptch);

			//if(NULL != ptch)
			//delete [] ptch;
			//ptch = NULL;

			//if(NULL != szBuf)
			//delete []szBuf;
			//szBuf = NULL;

			BSTR bstr = line.AllocSysString();
			if(ParseLine(bstr,num))
				num++;
			SysFreeString(bstr);
			line.Empty();
		}
	}
	sort(tagTimes.begin(),tagTimes.end(),CompareTime);
	return 0;
}

int CZLRC::ReadLRCFile(const Media &m)
{
	Clear();
	CString lrcFile;
	CFileHelper fileHelper;
	if(!m.isCue || m.isSingleFile)
	{
		lrcFile.Format(L"%s%s",m.fileName.Left(m.fileName.ReverseFind(L'.')+1),L"lrc");
	}
	else
	{
		lrcFile.Format(L"%s%s%s",m.fileName.Left(m.fileName.ReverseFind(L'\\')+1),m.title,L".lrc");
	}
	
	if(!fileHelper.FileExist(lrcFile) && (!m.isCue || m.isSingleFile))
	{
		lrcFile.Format(L"%s%s%s",lrcFile.Left(lrcFile.ReverseFind(L'\\')+1),m.title,L".lrc");
	}

	if(!fileHelper.FileExist(lrcFile))
	{		
		if(!m.isCue || m.isSingleFile)
		{
			lrcFile.Format(L"%s%s%s",lrcDir,m.title,L".lrc");
		}
	}

	if(!fileHelper.FileExist(lrcFile))
	{
		if(!m.isCue || m.isSingleFile)
		{
			int s = m.fileName.ReverseFind('\\')+1;
			int c = m.fileName.ReverseFind('.') - s;
			lrcFile.Format(L"%s%s%s",lrcDir,m.fileName.Mid(s,c),L".lrc");
		}
		else
		{
			lrcFile.Format(L"%s%s%s",lrcDir,m.title,L".lrc");
		}
	}
	if(!fileHelper.FileExist(lrcFile))
		lrcFile.Empty();
	
	if(!lrcFile.IsEmpty())
	{
		BSTR bstr = lrcFile.AllocSysString();
		int i= ReadLRCFile(bstr);
		SysFreeString(bstr);
		lrcFile.Empty();
		return i;
	}
	
	return -1;
}

bool CZLRC::ParseLine(wchar_t*  line,int &lineNum)
{
	CString str(line);
	str.Trim();
	CString strHead;
	int pos = str.Find(L"[");
	if(-1 == pos)
		return false;
	while(-1 != pos)
	{
		str = str.Mid(pos+1);
		pos = str.Find(L"]");
		if(-1 != pos)
		{
			strHead = str.Left(pos);
			
			if(lineNum < 6 && -1 != strHead.Find(L":"))
			{
				strHead = strHead.Left(strHead.Find(L":"));
				if("ti" == strHead  || "ar" == strHead  ||"by" == strHead || "al" == strHead )
				{	
					str = str.Left(pos);
					BSTR bstr = str.Right(str.GetLength() - 3).AllocSysString();
					AddLRCWord(bstr);
					SysFreeString(bstr);
					str.Empty();
					strHead.Empty();
					return true;
				}
				else if("offset" == strHead)
				{
					if(1 != _stscanf(str.Right(str.GetLength()-7), L"%d", &offset))
						offset = 0;
					str.Empty();
					strHead.Empty();
					return false;
				}
				else if("la" == strHead)
				{
					str.Empty();
					strHead.Empty();
					return false;
				}
				strHead = str.Left(pos);
			}
			str = str.Mid(pos+1);
			str = str.Trim();
			BSTR bstr = strHead.AllocSysString();
			AddLRCTag(bstr,lineNum);
			SysFreeString(bstr);
			pos = str.Find(L"[");
		}
	}
	str = str.Trim();
	AddLRCWord(str.AllocSysString());
	strHead.Empty();
	str.Empty();
	return true;
}

void CZLRC::AddLRCTag(wchar_t* tag,int &lineNum)
{
	LRCTagTime lrcTag;
	lrcTag.lineNum = lineNum;
	lrcTag.time = StrToSS(tag);
	if(-1 != lrcTag.time)
		tagTimes.push_back(lrcTag);
}

void CZLRC::AddLRCWord(wchar_t* lrcWord)
{
	LRCWord l;
	l.lrcWord = lrcWord;
	lrcWords.push_back(l);
}

void CZLRC::Clear()
{
	tagTimes.clear();
	//lrcWords.clear();
	for(int i = lrcWords.size()-1;i >=0;i--)
	{
		lrcWords[i].lrcWord.Empty();
		lrcWords.erase(lrcWords.begin()+i);
	}
}