#pragma once
#include "encoder.h"

#include "..\decoders\lame\include\lame.h"

#define INBUFSIZE 4096
#define MP3BUFSIZE (int) (1.25 * INBUFSIZE) + 7200

class CMP3Encoder :
	public CEncoder
{
public:
	__declspec(dllexport) CMP3Encoder(void);
	__declspec(dllexport) virtual ~CMP3Encoder(void);
	__declspec(dllexport) int Open(const wchar_t *,AudioFormat af = AutoDetect);
	__declspec(dllexport) int SetTimes(float s,float e);
	__declspec(dllexport) int Save(const wchar_t *);
	__declspec(dllexport) int SetQuality(int);
	__declspec(dllexport) int SetMPEG_Mode(int);
	__declspec(dllexport) int SetBitrate(int);
private:
	int quality;
	MPEG_mode mpegMode;
	int bitrate;

	short buffer[INBUFSIZE*2];
	unsigned char mp3Buffer[MP3BUFSIZE];
};

