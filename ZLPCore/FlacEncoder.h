#pragma once
#include "encoder.h"

#if HAVE_CONFIG_H
#  include <config.h>
#endif
#include "..\decoders\flac\include\FLAC\metadata.h"
#include "..\decoders\flac\include\FLAC\stream_encoder.h"

// simple processing 
#define READSIZE 1024


class CFlacEncoder :
	public CEncoder
{
public:
	__declspec(dllexport) CFlacEncoder(void);
	__declspec(dllexport) ~CFlacEncoder(void);
	__declspec(dllexport) int Open(const wchar_t *,AudioFormat af = AutoDetect);
	__declspec(dllexport) int SetTimes(float s,float e);
	__declspec(dllexport) int SetCompressionLevel(unsigned int compressionLevel);
	__declspec(dllexport) int Save(const wchar_t *);
	static void progress_callback(const FLAC__StreamEncoder *encoder, FLAC__uint64 bytes_written, FLAC__uint64 samples_written, unsigned frames_written, unsigned total_frames_estimate, void *client_data);
private:
	unsigned int total_samples;
	unsigned int compLevel;
    FLAC__byte buffer[READSIZE/*samples*/ * 2/*bytes_per_sample*/ * 2/*channels*/]; /* we read the WAVE data into here */
    FLAC__int32 pcm[READSIZE/*samples*/ * 2/*channels*/];
};

