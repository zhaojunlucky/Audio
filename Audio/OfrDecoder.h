#pragma once
#include "audiodecoder.h"
#include "..\decoders\OptimFROG\OptimFROGDecoder.h"

class OfrDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) OfrDecoder(void);
	__declspec(dllexport) ~OfrDecoder(void);
public:
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) ;
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) ;
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	OptimFROGDecoder *decoder;
};

