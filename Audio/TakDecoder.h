#pragma once
#include "audiodecoder.h"
#include "..\decoders\TAK\SDK\Header\tak_deco_lib.h"

class TakDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) TakDecoder(void);
	__declspec(dllexport) ~TakDecoder(void);
public:
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) ;
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) ;
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	TtakSSDOptions                Options;
	TtakSeekableStreamDecoder     Decoder;
	TtakInt32  ReadNum;
	TtakInt32  SamplesPerBuf ;
};

