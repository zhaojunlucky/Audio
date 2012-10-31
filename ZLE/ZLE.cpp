// ZLE.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdlib.h>
#include <afx.h>
#include <MMSystem.h>
#include "..\Utility\FileHelper.h"
#include "..\Utility\ZStringHelper.h"
#include "..\ZLPCore\AudioBase.h"
#include "..\Audio\AudioDecoder.h"
#include "..\ZLPCore\FlacEncoder.h"
#include "..\ZLPCore\ApeEncoder.h"
#include "..\ZLPCore\Mp3Encoder.h"
#include "..\ZLPCore\WaveEncoder.h"
#include "..\ZLPCore\Encoder.h"
#include "..\ZLP\ZCueSheet.h"
#include <vector>
using namespace std;
using namespace ZStringHelper;


void CheckPNum(int &cIndex,int & total);
void ConvertToFlac(CString &sFile,CString &dFile,int cL,float ss = -1.0f,float ee = -1.0f);
void ConvertToApe(CString &sFile,CString &dFile,int cL,float ss = -1.0f, float = -1.0f);
void ConvertToWav(CString &sFile,CString &dFile,float ss = -1.0f,float ee = -1.0f );
void ConvertToMp3(CString &sFile,CString &dFile,int bitrate,int quality,float ss = -1.0f,float ee = -1.0f);
void ConvertFromCue(CString &sFile,AudioFormat af,vector<unsigned int> aIndex,int cL,int bitrate,int quality);


void  static __stdcall Process(void* instance, int processType, DWORD totalBytes,DWORD proccessedbytes,void *client);
bool first = true;
int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_CTYPE,"chs");  
	if (argc < 2)
	{
		printf("Please input the source audio file...\n");
	}
	else if(argc == 2)
	{
		CFileHelper fileHelper;
		if(!fileHelper.FileExist(argv[1]))
			printf("The source file is not exist...\n");
		// convert the audio file into flac and save in default directory
		else
		{
			CString sFile(argv[1]);
			CString dFile = sFile.Left(sFile.ReverseFind(L'.')+1)+"flac";
			if(sFile != dFile)
				ConvertToFlac(sFile,dFile,-1000);
			else
				printf("Input file format is flac,can't convert to default format flac...\n");
		}

	}
	else
	{
		// strict mode
		// -s source file
		// -d save file
		// -FLAC flac file,-l compression level
		// -APE APE file,-l compression level
		// -MP3 MP3 file,-q quality,-m MPEG Mode(not recommend),-b bitrate
		// -WAV WAV file
		// CUE file,-split split single audio into multi files by cue index
		//               -index the number of the audio index to be converted,started from 0
		//					e.g. 0,3,5 will convert the 0,3,5 track if have.
		// notice:the two tag below only work except CUE
		// -ss start time
		// -ee end time 
		CString sFile;
		CString dFile;
		int l = -1000;
		int q = -1000;
		int m = -1000;
		int b = 320;
		int split = 0;
		//int index;
		vector<unsigned int> aIndex;
		float ss;
		float ee;
		AudioFormat af = FLAC;
		for(int i = 1;i < argc;i++)
		{
			if(CZStringHelper::StartWith(argv[i],L"-s"))
			{
				i++;
				CheckPNum(i,argc);
				sFile.Format(L"%s",argv[i]);			
			}
			else if(CZStringHelper::StartWith(argv[i],L"-d"))
			{
				i++;
				CheckPNum(i,argc);
				dFile.Format(L"%s",argv[i]);
			}
			else if(CZStringHelper::StartWith(argv[i],L"-l"))
			{
				i++;
				CheckPNum(i,argc);
				if(1 != wscanf_s(L"%d",argv[i],&l))
				{
					printf("-l parameter error...");
					exit(0);
				}
			}
			else if(CZStringHelper::StartWith(argv[i],L"-q"))
			{
				i++;
				CheckPNum(i,argc);
				if(1 != wscanf_s(L"%d",argv[i],&q))
				{
					printf("-q parameter error...");
					exit(0);
				}
			}
			else if(CZStringHelper::StartWith(argv[i],L"-b"))
			{
				i++;
				CheckPNum(i,argc);
				if(1 != wscanf_s(L"%d",argv[i],&b))
				{
					printf("-b parameter error...");
					exit(0);
				}
			}
			else if(CZStringHelper::StartWith(argv[i],L"-ss"))
			{
				i++;
				CheckPNum(i,argc);
				if(1 != wscanf_s(L"%f",argv[i],&ss))
				{
					printf("-ss parameter error...");
					exit(0);
				}
			}
			else if(CZStringHelper::StartWith(argv[i],L"-ee"))
			{
				i++;
				CheckPNum(i,argc);
				if(1 != wscanf_s(L"%f",argv[i],&ee))
				{
					printf("-ee parameter error...");
					exit(0);
				}
			}
			else if(CZStringHelper::StartWith(argv[i],L"-split"))
			{
				split = 1;
			}
			else if(CZStringHelper::StartWith(argv[i],L"-index"))// not support in this version
			{
				i++;
				CheckPNum(i,argc);
				
				CString indexStr(argv[i]);
				int iOfIndex = indexStr.Find(',');
				int indexNum;
				
				while(-1 != iOfIndex)
				{
					CString tmp = indexStr.Left(iOfIndex);// may be index num
					if(1 ==  wscanf_s(L"%f",tmp,&indexNum) && indexNum >= 0)
						aIndex.push_back(indexNum);

					indexStr = indexStr.Right(indexStr.GetLength() - iOfIndex -1);
					iOfIndex = indexStr.Find(',');
				}
			}
			else if(CZStringHelper::StartWith(argv[i],L"-MP3"))
			{
				af = MP3;
			}
			else if(CZStringHelper::StartWith(argv[i],L"-FLAC"))
			{
				af = FLAC;
			}
			else if(CZStringHelper::StartWith(argv[i],L"-APE"))
			{
				af = APE;
			}
			else if(CZStringHelper::StartWith(argv[i],L"-WAV"))
			{
				af = WAV;
			}
			else
			{
				printf("Error command...\n");
				exit(0);
			}
		}


		if(sFile.IsEmpty())
		{
			printf("No input file specfied...\n");
			exit(0);
		}
		
		if(L"cue" == sFile.Right(3).MakeLower())
		{
			ConvertFromCue(sFile,af,aIndex,l,b,q);
		}
		else if(FLAC == af)
		{
			if(dFile.IsEmpty() || 0 != wcscmp(dFile.Right(4).MakeLower(), L"flac"))
				dFile = sFile.Left(sFile.ReverseFind(L'.')+1)+"flac";
			ConvertToFlac(sFile,dFile,l);
		}
		else if(APE == af)
		{
			if(dFile.IsEmpty() || 0 != wcscmp(dFile.Right(3).MakeLower(), L"ape"))
				dFile = sFile.Left(sFile.ReverseFind(L'.')+1)+"ape";
			ConvertToApe(sFile,dFile,l);
		}
		else if(MP3 == af)
		{
			if(dFile.IsEmpty() || 0 != wcscmp(dFile.Right(3).MakeLower(), L"mp3"))
				dFile = sFile.Left(sFile.ReverseFind(L'.')+1)+"mp3";
			ConvertToMp3(sFile,dFile,b,q);
		}
		else if(WAV == af)
		{
			if(dFile.IsEmpty() || 0 != wcscmp(dFile.Right(3).MakeLower(), L"wav"))
				dFile = sFile.Left(sFile.ReverseFind(L'.')+1)+"wav";
			ConvertToWav(sFile,dFile);
		}

	}
	system("PAUSE");
	return 0;
}

void CheckPNum(int &cIndex,int & total)
{
	if(cIndex >= total)
	{
		printf("The number of parameters is not right...\n");
		exit(0);
	}
}


void ConvertToFlac(CString &sFile,CString &dFile,int cL,float ss ,float ee)
{
	CFlacEncoder *flacEncoder = new CFlacEncoder;
	if(!flacEncoder || 0!= flacEncoder->Open(sFile))
	{
		printf("Open file error...");
		delete flacEncoder;
		exit(0);
	}
	if(-1.0f != ss && -1.0f != ee && ss < ee)
	{
		flacEncoder->SetTimes(ss,ee);
	}
	wprintf(L"File save location:%s\n",dFile);
	flacEncoder->SetProccessCallback(Process,0);
	flacEncoder->SetCompressionLevel(cL);
	flacEncoder->Save(dFile);
	puts("\nCompleted...");
	delete flacEncoder;
}
void ConvertToApe(CString &sFile,CString &dFile,int cL,float ss,float ee)
{
	CApeEncoder *apeDecoder = new CApeEncoder;
	if(!apeDecoder || 0!= apeDecoder->Open(sFile))
	{
		printf("Open file error...");
		delete apeDecoder;
		exit(0);
	}
	if(-1.0f != ss && -1.0f != ee && ss < ee)
	{
		apeDecoder->SetTimes(ss,ee);
	}
	wprintf(L"File save location:%s\n",dFile);
	apeDecoder->SetProccessCallback(Process,0);
	apeDecoder->SetCompressionLevel(cL);
	apeDecoder->Save(dFile);
	puts("\nCompleted...");
	delete apeDecoder;
}
void ConvertToWav(CString &sFile,CString &dFile,float ss,float ee)
{
	CWaveEncoder *wav = new CWaveEncoder;
	if(!wav || 0 != wav->Open(sFile))
	{
		printf("Open file error...");
		delete wav;
		exit(0);
	}
	if(-1.0f != ss && -1.0f != ee && ss < ee)
	{
		wav->SetTimes(ss,ee);
	}
	wprintf(L"File save location:%s\n",dFile);
	wav->SetProccessCallback(Process,0);
	wav->Save(dFile);
	puts("\nCompleted...");
	delete wav;
}
void ConvertToMp3(CString &sFile,CString &dFile,int bitrate,int quality,float ss,float ee )
{
	CMP3Encoder *mp3 = new CMP3Encoder;
	if(!mp3 || 0 != mp3->Open(sFile))
	{
		printf("Open file error...");
		delete mp3;
		exit(0);
	}
	if(-1.0f != ss && -1.0f != ee && ss < ee)
	{
		mp3->SetTimes(ss,ee);
	}

	wprintf(L"File save location:%s\n",dFile);
	mp3->SetProccessCallback(Process,0);
	mp3->SetBitrate(bitrate);
	mp3->SetQuality(quality);
	mp3->Save(dFile);
	puts("\nCompleted...");
	delete mp3;
}

void  static __stdcall Process(void* instance, int processType, DWORD totalBytes,DWORD proccessedbytes,void *client)
{
	if(1 == processType)
	{
		if(!first)
		{
			
			float p = (float)((double)proccessedbytes/(double)totalBytes*100);
			printf("\b\b\b\b\b\b\b");
			printf("%6.2f%%",p) ;
			if(100 == (int)p)
				puts("\n");
		}
		else
		{
			printf("\nProcessing:");
			float p = (float)((double)proccessedbytes/(double)totalBytes*100);
			first = false;
			printf("%6.2f%%",p);
		}

	}
}

void ConvertFromCue(CString &sFile,AudioFormat af,vector<unsigned int> aIndex,int cL,int bitrate,int quality)
{
	CZCueSheet cue;
	cue.PaseSheet(sFile);
	if(cue.GetSize() <=0 )
	{
		printf("Parse Cue sheet error...\n");
		exit(0);
	}
	if(cue.GetAt(0).isSingleFile && cue.GetAt(0).mediaFormat == af)
	{
		printf("Source file format error...\n");
		exit(0);
	}
	CString dir = sFile.Left(sFile.Find(L'\\')+1);
	if(aIndex.size() > 0)
	{
		for(size_t i = 0;i < aIndex.size();i++)
		{
			if(aIndex[i] < cue.GetSize())
			{
				Media m = cue.GetAt(aIndex[i]);
				if(MP3 == af)
				{
					CString dFile = dir + m.title+".mp3";
					puts("\n");
					ConvertToMp3(m.fileName,dFile,bitrate,quality,(float)m.start,(float)m.end);
				}
				else if(FLAC ==af)
				{
					CString dFile = dir + m.title+".flac";
					puts("\n");
					ConvertToFlac(m.fileName,dFile,cL,(float)m.start,(float)m.end);
				}
				else if(WAV == af)
				{
					CString dFile = dir + m.title+".wav";
					puts("\n");
					ConvertToWav(m.fileName,dFile,(float)m.start,(float)m.end);
				}
				else if(APE == af)
				{
					CString dFile = dir + m.title+".ape";
					puts("\n");
					ConvertToApe(m.fileName,dFile,cL,(float)m.start,(float)m.end);
				}
			}
		}
	}
	else
	{
		for(size_t i = 0;i < cue.GetSize();i++)
		{
			Media m = cue.GetAt(i);
			if(MP3 == af)
			{
				CString dFile = dir + m.title+".mp3";
				puts("\n");
				ConvertToMp3(m.fileName,dFile,bitrate,quality,(float)m.start,(float)m.end);
			}
			else if(FLAC ==af)
			{
				CString dFile = dir + m.title+".flac";
				puts("\n");
				ConvertToFlac(m.fileName,dFile,cL,(float)m.start,(float)m.end);
			}
			else if(WAV == af)
			{
				CString dFile = dir + m.title+".wav";
				puts("\n");
				ConvertToWav(m.fileName,dFile,(float)m.start,(float)m.end);
			}
			else if(APE == af)
			{
				CString dFile = dir + m.title+".ape";
				puts("\n");
				ConvertToApe(m.fileName,dFile,cL,(float)m.start,(float)m.end);
			}
		}
	}
}