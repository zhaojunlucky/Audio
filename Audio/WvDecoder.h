#pragma once
#include "audiodecoder.h"
#include "..\decoders\wavpack\wavpack.h"

#define BUFFER_SIZE 4096

class WvDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) WvDecoder(void);
	__declspec(dllexport) ~WvDecoder(void);
public:
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) ;
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) ;
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	WavpackContext *wpc;
	char error[1024];
	int numchannels;
	int bytespersample;
	unsigned int totalSamples;
};

