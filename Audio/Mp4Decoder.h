#pragma once
#include "audiodecoder.h"
#include "..\decoders\faad\include\neaacdec.h"
#include "..\decoders\faad\common\mp4ff\mp4ff.h"

#define SAMPLE_SIZE 16

class Mp4Decoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) Mp4Decoder(void);
	__declspec(dllexport) ~Mp4Decoder(void);

public:
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) ;
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) ;
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition );
public:
	static uint32_t read_callback(void *user_data, void *buffer, uint32_t length);
	static uint32_t seek_callback(void *user_data, uint64_t position);
	int GetAACTrack(mp4ff_t *infile);
private:
	mp4ff_t *infile;
    FILE *mp4File;
	long m_ntotalSamples;
	long m_currentSample;
	mp4ff_callback_t *mp4cb;
	unsigned char *buffer;
	unsigned int buffer_size;
	int track;
	unsigned int *m_pSb;
	BOOL m_seek;
	unsigned int m_seekLeftBytes;

	NeAACDecHandle hDecoder;
    NeAACDecFrameInfo frameInfo;
	NeAACDecConfigurationPtr config;
};

