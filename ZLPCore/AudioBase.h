#pragma once
#include "..\Utility\ZStringHelper.h"

enum AudioFormat
{
	FLAC = 0,
	APE,
	WAV,
	MP3,
	OGG,
	TTA,
	AAC,
	MP4,
	M4A,
	OFR,
	TAK,
	WV,
	CUE,
	AutoDetect = 1000
};

typedef struct WAVEINFO
{
	WORD        wFormatTag;         /* format type */
	WORD        nChannels;          /* number of channels (i.e. mono, stereo...) */
	DWORD       nSamplesPerSec;     /* sample rate */
	DWORD       nAvgBytesPerSec;    /* for buffer estimation */
	WORD        nBlockAlign;        /* block size of data */
	WORD        wBitsPerSample;     /* number of bits per sample of mono data */
	WORD        cbSize;             /* the count in bytes of the size of */
	float		durationTime;       /* the length of the media*/
	DWORD		waveSize;
	double		bitrate;
}WAVEINFO;

class CAudioBase
{
public:
	__declspec(dllexport) CAudioBase()
	{
	}
	__declspec(dllexport) ~CAudioBase()
	{
	}
	__declspec(dllexport) AudioFormat static IsSupportMedia(const char* file)
	{
		int wcsLen = ::MultiByteToWideChar(CP_ACP, NULL, file, strlen(file), NULL, 0);  
		wchar_t *wFile = new wchar_t[wcsLen + 1];
		MultiByteToWideChar(CP_ACP, NULL, file, strlen(file), wFile, wcsLen); 
		wFile[wcsLen] = L'\0';

		AudioFormat af = IsSupportMedia(wFile);
		delete wFile;
		return af;
	}

	__declspec(dllexport) AudioFormat static IsSupportMedia(const wchar_t* file)
	{
		int len = ZStringHelper::CZStringHelper::GetPtrSize(file);
		int index = ZStringHelper::CZStringHelper::LastIndexOf(file,L'.');
		AudioFormat af = AutoDetect;
		if(index)
		{
			int lenToSub = len - index;
			wchar_t *extension = new wchar_t[lenToSub];
			ZStringHelper::CZStringHelper::SubString(file,extension,index + 1);
			extension[lenToSub - 1] = L'\0';
			if(0 == _wcsicmp(extension,L"flac"))
			{
				af = FLAC;
			}
			else if(0 == _wcsicmp(extension,L"ape"))
			{
				af = APE;
			}
			else if(0 == _wcsicmp(extension,L"mp3"))
			{
				af = MP3;
			}
			else if(0 == _wcsicmp(extension,L"aac"))
			{
				af = AAC;
			}
			else if(0 == _wcsicmp(extension,L"ogg"))
			{
				af = OGG;
			}
			else if(0 == _wcsicmp(extension,L"tta"))
			{
				af = TTA;
			}
			else if(0 == _wcsicmp(extension,L"m4a") || 0 == _wcsicmp(extension,L"mp4"))
			{
				af = MP4;
			}
			else if(0 == _wcsicmp(extension,L"wav"))
			{
				af = WAV;
			}
			else if(0 == _wcsicmp(extension,L"ofr"))
			{
				af = OFR;
			}
			else if(0 == _wcsicmp(extension,L"tak"))
			{
				af = TAK;
			}
			else if(0 == _wcsicmp(extension,L"wv"))
			{
				af = WV;
			}
			else if(0 == _wcsicmp(extension,L"cue"))
			{
				af =  CUE;
			}
			delete extension;
		}
		return af;
	}
};