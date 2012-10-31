#pragma once
#include "audiodecoder.h"
#include "..\decoders\flac\include\FLAC\stream_decoder.h"
#include "..\decoders\flac\include\FLAC\metadata.h"


class FlacDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) FlacDecoder(void);
	__declspec(dllexport) ~FlacDecoder(void);
public:
	// override function
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	FLAC__StreamDecoder *decoder;
	FLAC__StreamDecoderInitStatus init_status;
	static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
	static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);
	static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data);
	long metaDataSize;
};

