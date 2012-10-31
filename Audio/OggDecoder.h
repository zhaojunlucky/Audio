#pragma once
#include <io.h>
#include <fcntl.h>

#include "audiodecoder.h"
#include "..\decoders\libvorbis\include\vorbis\vorbisfile.h"

#define OGG_BUFFER_SIZE 4096

class OggDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) OggDecoder(void);
	__declspec(dllexport) ~OggDecoder(void);
public:
	// overrid function
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	OggVorbis_File vf; // Create the vorbis file
	FILE *infile; // Opens Ogg file
	int eof ;
	int current_section;
};

