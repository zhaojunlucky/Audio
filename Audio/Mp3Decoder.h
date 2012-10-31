#pragma once
#include "audiodecoder.h"
#include "..\include\mpg123.h"

class Mp3Decoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) Mp3Decoder(void);
	__declspec(dllexport) ~Mp3Decoder(void);
public:
	// overrid function
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	struct mpg123_handle_struct* _handle;
};

