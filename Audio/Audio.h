#pragma once
#include "..\Audio\AudioDecoder.h"
#include "..\Audio\WaveDecoder.h"
#include "..\Audio\FlacDecoder.h"
#include "..\Audio\Mp3Decoder.h"
#include "..\Audio\ApeDecoder.h"
#include "..\Audio\OggDecoder.h"
#include "..\Audio\AacDecoder.h"
#include "..\Audio\Mp4Decoder.h"
#include "..\Audio\TtaDecoder.h"
#include "..\Audio\OfrDecoder.h"
#include "..\Audio\WvDecoder.h"
#include "..\Audio\TakDecoder.h"
#define ZCWRAP __declspec(dllexport)



#define ZLP_HANDLE void*


class ZLP
{
public:
	ZLP()
	{
		decoder = 0;
		title = 0;
		artist = 0;
	}
	~ZLP()
	{
		if(decoder)
			delete decoder;
		if(title)
			delete title;
		if(artist)
			delete artist;
	}
public:
	AudioDecoder *decoder;
	wchar_t *title;
	wchar_t *artist;
};

/*
ZCWRAP void __stdcall CloseZLP(ZLP_HANDLE hwnd);
ZCWRAP int	__stdcall OpenFile(ZLP_HANDLE hwnd,wchar_t *file);
ZCWRAP int __stdcall GetTitle(ZLP_HANDLE hwnd,wchar_t *title,DWORD size);
ZCWRAP int __stdcall GetArtist(ZLP_HANDLE hwnd,wchar_t *artist,DWORD size);
*/

class EXPORT_Func
{
public:
    static  __declspec(dllexport) ZLP_HANDLE  CreateZLP();
	static  __declspec(dllexport)  void  CloseZLP(ZLP_HANDLE hwnd);
	static  __declspec(dllexport)  int	 OpenFile(ZLP_HANDLE hwnd,wchar_t *file);
	static  __declspec(dllexport)  int  GetTitle(ZLP_HANDLE hwnd,wchar_t *title,DWORD size);
	static  __declspec(dllexport)  int  GetArtist(ZLP_HANDLE hwnd,wchar_t *artist,DWORD size);
};


