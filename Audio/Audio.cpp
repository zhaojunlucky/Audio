#include "Audio.h"
#include <stdio.h>

ZLP_HANDLE  EXPORT_Func::CreateZLP()
{
	ZLP * zlp = new ZLP;
	return (ZLP_HANDLE)zlp;
}
void  EXPORT_Func::CloseZLP(ZLP_HANDLE hwnd)
{
	if(hwnd)
	{
		ZLP * zlp = (ZLP*)hwnd;
		delete zlp;
	}
}
int  EXPORT_Func::OpenFile(ZLP_HANDLE hwnd,wchar_t *file)
{
	if(!file || !hwnd)
		return 1;
	ZLP *zlp = (ZLP*)hwnd;
	if(zlp->title)
		delete zlp->title;
	zlp->title = 0;

	if(zlp->artist)
		delete zlp->artist;
	zlp->artist = 0;
	wchar_t ext[5];
	int length = wcslen(file);
	if(length < 5)
		return 2;

	if(L'.' == file[length - 4])//.wav,.aac,.mp3,....
	{
		for(int i = 2; i >= 0;i--)
			ext[2-i] = file[length - i - 1];
		ext[3] = L'\0';
	}
	else if(L'.' == file[length - 5])//.flac
	{
		for(int i = 3; i >= 0;i--)
			ext[3-i] = file[length - i - 1];
		ext[4] = L'\0';
	}
	else if(L'.' == file[length - 2])//wv
	{
		ext[0] = file[length - 2];
		ext[1] = file[length - 1];
		ext[2] = L'\0';
	}
	else
		return 3;

	if(zlp && zlp->decoder)
		delete zlp->decoder;
	if(0 == _wcsicmp(ext,L"aac"))
	{
		zlp->decoder = new AacDecoder;
	}
	else if(0 == _wcsicmp (ext,L"ape"))
	{
		zlp->decoder = new ApeDecoder;
	}
	else if(0 == _wcsicmp (ext,L"flac"))
	{
		zlp->decoder = new FlacDecoder;
	}
	else if(0 == _wcsicmp (ext,L"mp3"))
	{
		zlp->decoder = new Mp3Decoder;
	}
	else if(0 == _wcsicmp (ext,L"mp4"))
	{
		return 2;
	}
	else if(0 == _wcsicmp (ext,L"ofr"))
	{
		zlp->decoder = new OfrDecoder;
	}
	else if(0 == _wcsicmp (ext,L"ogg"))
	{
		zlp->decoder = new OggDecoder;
	}
	else if(0 == _wcsicmp (ext,L"tak"))
	{
		zlp->decoder = new TakDecoder;
	}
	else if(0 == _wcsicmp (ext,L"tta"))
	{
		zlp->decoder = new TtaDecoder;
	}
	else if(0 == _wcsicmp (ext,L"wav"))
	{
		zlp->decoder = new WaveDecoder;
	}
	else if(0 == _wcsicmp (ext,L"wv"))
	{
		zlp->decoder = new WvDecoder;
	}
	else
		return 2;
	if(FAILED(zlp->decoder->Open(file,0,WAVEFILE_READ)))
		return 3;

	if(zlp->decoder->GetID3Info()->title.GetDataLength() > 0)
	{
		zlp->title = new wchar_t[zlp->decoder->GetID3Info()->title.GetDataLength() + 1];
		wcscpy(zlp->title,zlp->decoder->GetID3Info()->title.GetData());
		zlp->title[zlp->decoder->GetID3Info()->title.GetDataLength()] = L'\0';
	}
	if(zlp->decoder->GetID3Info()->artist.GetDataLength() > 0)
	{
		zlp->artist = new wchar_t[zlp->decoder->GetID3Info()->artist.GetDataLength() + 1];
		wcscpy(zlp->artist,zlp->decoder->GetID3Info()->artist.GetData());
		zlp->artist[zlp->decoder->GetID3Info()->artist.GetDataLength()] = L'\0';
	}
	delete zlp->decoder;
	zlp->decoder = 0;
	return 0;
}
int  EXPORT_Func::GetTitle(ZLP_HANDLE hwnd,wchar_t *title,DWORD size)
{
	//FILE * f = fopen("d:\\1.txt","w+");
	//fputs("a:\n",f);
	//fclose(f);
	if(hwnd)
	{
		//fputs("b:\n",f);
		ZLP * zlp = (ZLP*)hwnd;
		//fputs("c:\n",f);
		if(wcslen(zlp->title) < size)
		{
			//fputs("d:\n",f);
			//fflush(f);
			wcscpy(title , zlp->title);
			//fputs("e:\n",f);
			
			return 0;
		}
	}
	//fclose(f);
	return 1;
}
int  EXPORT_Func::GetArtist(ZLP_HANDLE hwnd,wchar_t *artist,DWORD size)
{
	if(hwnd)
	{
		ZLP * zlp = (ZLP*)hwnd;
		if(wcslen(zlp->artist) < size)
		{
			wcscpy(artist , zlp->artist);
			return 0;
		}
	}
	return 1;
}