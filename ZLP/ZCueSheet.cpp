#include "StdAfx.h"
#include "ZCueSheet.h"
#include "..\ZLPCore\AudioBase.h"
#include "..\Utility\FileHelper.h"
#include <MMSystem.h>
#include "..\ZLPCore\Decoder.h"
#include "..\Utility\CharWCharHelper.h"

CZCueSheet::CZCueSheet(void)
{
}


CZCueSheet::~CZCueSheet(void)
{
	medias.clear();
}

unsigned int CZCueSheet::GetSize()
{
	return medias.size();
}

const Media CZCueSheet::GetAt(unsigned int  index)
{
	assert(index >=0 && index < medias.size());
	return medias[index];
}

bool CZCueSheet::PaseSheet(const wchar_t * fileName) 
{
	medias.clear();
	int findIndex = ZStringHelper::CZStringHelper::LastIndexOf(fileName,L'\\');
	if(!findIndex)
		return false;

	wchar_t *p = new wchar_t[findIndex + 2];
	ZStringHelper::CZStringHelper::SubString(fileName,p,0,findIndex);
	p[findIndex+1] = '\0';
	CString cueDir(p);
	delete p;

	
	FILE *file;

	char *filePath = 0;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,fileName,-1,NULL,0,NULL,FALSE);
	filePath = new char[dwNum + 1];
	WideCharToMultiByte (CP_OEMCP,NULL,fileName,-1,filePath,dwNum,NULL,FALSE);

	fopen_s(&file,filePath,"r");
	if(!file)
	{
		delete filePath;
		return false;
	}
	string cueDirA ;

	delete filePath;

	

	Media m;
	EmptyMedia(m);
	char content[1024];
	char tmp[1024];
	int fileNum = 0;
	int trackNum = 0;
	m.isSingleFile = false;
	m.isCue = true;

	int min,sec,frame;
	int lastmin = 0,lastsec = 0;
	// detect code
	bool ascii = true;
	fread(tmp,10,1,file);
	if((int)(unsigned char)tmp[0]==0xEF &&
		(int)(unsigned char)tmp[1]==0xBB &&
		(int)(unsigned char)tmp[2]==0xBF)
	{
		ascii = false;
	}
	fseek(file,0,SEEK_SET);
	CFileHelper fileHelper;
	wchar_t b[512];
	CCharWCharHelper cwh;
	while(0 != fgets(content,512,file))
	{
		if(strstr(content,"PERFORMER") != NULL)
		{
			sscanf(content, "%*[^\"]\"%[^\"]", tmp);
			
			if(!ascii)
			{
				int n=MultiByteToWideChar(CP_UTF8,0,tmp,strlen(tmp)+1,NULL,0);
				wchar_t* pWideChar=(wchar_t*)calloc(n+1,sizeof(wchar_t));
				MultiByteToWideChar(CP_UTF8,0,tmp,strlen(tmp)+1,pWideChar,n);
				m.aritst = pWideChar;
				free(pWideChar);
			}
			else
			{
				cwh.ZMultiByteToWideChar(tmp);
				m.aritst = cwh.GetWideChar();
			}
		}
		else if (strstr(content, "TITLE") != NULL)
		{
			sscanf(content, "%*[^\"]\"%[^\"]", tmp);
			if(!ascii)
			{
				int n=MultiByteToWideChar(CP_UTF8,0,tmp,strlen(tmp)+1,NULL,0);
				wchar_t* pWideChar=(wchar_t*)calloc(n+1,sizeof(wchar_t));
				MultiByteToWideChar(CP_UTF8,0,tmp,strlen(tmp)+1,pWideChar,n);
				m.title = pWideChar;
				free(pWideChar);
			}
			else
			{
				//m.title = tmp;
				cwh.ZMultiByteToWideChar(tmp);
				m.title = cwh.GetWideChar();
			}
		}
		else if (strstr(content, "FILE") != NULL)
		{
			sscanf(content, "%*[^\"]\"%[^\"]", tmp);
			AudioFormat af = AutoDetect;
			CString name  ;
			if(!ascii)
			{
				int n=MultiByteToWideChar(CP_UTF8,0,tmp,strlen(tmp)+1,NULL,0);
				wchar_t* pWideChar=(wchar_t*)calloc(n+1,sizeof(wchar_t));
				MultiByteToWideChar(CP_UTF8,0,tmp,strlen(tmp)+1,pWideChar,n);
				pWideChar[n] = L'\0';
				af = CAudioBase::IsSupportMedia(pWideChar);				
				name = pWideChar;
				//setlocale(LC_ALL, ".936");
				//wcstombs(tmp,pWideChar,1023);
				cwh.ZWideCharToMultiByte(pWideChar);
				strcpy_s(tmp,1024,cwh.GetMultiChar());
				free(pWideChar);
			}
			else
			{
				cwh.ZMultiByteToWideChar(tmp);
				name = cwh.GetWideChar();
				af = CAudioBase::IsSupportMedia(name);				
			}
			if(AutoDetect == af || CUE == m.mediaFormat)//unsupported file
			{
				break;
			}
			
			// if find file tag twice,it means multi files in cue
			fileNum++;
			if(2 == fileNum)
			{
				fileNum = 1;
				m.isSingleFile = true;
				if(fileHelper.FileExist(m.fileName))
					medias.push_back(m);
				EmptyMedia(m);
			}
			
			m.fileName = cueDir + name;
			name.Empty();
			
			//if(!fileHelper.FileExist(m.fileName))
			//{				
			//	break;
			//}
		}
		else if (strstr(content, "TRACK") != NULL)
		{
			if(AutoDetect == m.mediaFormat)
			{
				m.mediaFormat = CAudioBase::IsSupportMedia(tmp);
				if(AutoDetect == m.mediaFormat || CUE == m.mediaFormat )//unsupported file
				{
					break;
				}
			}
			trackNum++;
			if(2 == trackNum)
			{
				trackNum = 1;
				if(!m.isSingleFile && fileHelper.FileExist(m.fileName))
				{
					medias.push_back(m);
					EmptyMedia(m);
				}
			}
		}
		else if(strstr(content, "INDEX 00") != NULL && !m.isSingleFile)// update last media's end time
		{
			sscanf(content, "    INDEX 00 %d:%d:%d", &lastmin, &lastsec, &frame);
			if(medias.size() > 0)
				medias[medias.size()-1].end = lastmin * 60 + lastsec;
		}
		else if (strstr(content, "INDEX 01") != NULL && !m.isSingleFile)// update current media start time
		{				
			sscanf(content, "    INDEX 01 %d:%d:%d", &min, &sec, &frame);
			m.start = min * 60 + sec;
			if(medias.size() > 0 && -1 == medias[medias.size()-1].end)
			{
				medias[medias.size()-1].end = m.start -2 ;// offerset 2 seconds
			}
		}
	}
	// update the last media end time which is -1 a special tag
	if(fileHelper.FileExist(m.fileName))
		medias.push_back(m);
	if(medias.size() > 0)
	{
		//medias[medias.size()-1].end = -1;
		CDecoder decoder;
		BSTR bstr = medias[medias.size()-1].fileName.AllocSysString();
		decoder.Open(bstr,m.mediaFormat);
		medias[medias.size()-1].end = decoder.GetWaveInfo()->durationTime;
		SysFreeString(bstr);
	}
	EmptyMedia(m,true);
	return true;
}

