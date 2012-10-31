#pragma once
#include "audiodecoder.h"
#include <MMSystem.h>

class WaveDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) WaveDecoder(void);
	__declspec(dllexport) ~WaveDecoder(void);

public:
	// overrid function
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration,DWORD *newPosition ) ;
private:
	HMMIO m_hmmio;       // MM I/O handle for the WAVE
    MMCKINFO m_ck;          // Multimedia RIFF chunk
    MMCKINFO m_ckRiff;      // Use in opening a WAVE file
	MMIOINFO m_mmioinfoOut;
	BYTE* m_pbData;
    BYTE* m_pbDataCur;
    ULONG m_ulDataSize;
    CHAR* m_pResourceBuffer;

private:
	HRESULT ReadMMIO();
    HRESULT WriteMMIO( WAVEFORMATEX* pwfxDest );
};

