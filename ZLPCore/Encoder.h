#pragma once

#include "Decoder.h"
#include "..\Utility\ZString.h"
using namespace DDString;

typedef void (__stdcall * ZDProcCallbackFunction)(void* instance, int processType, DWORD totalBytes,DWORD proccessedbytes,void *client);

typedef struct ZDProcCallback
{
	void *client;
	ZDProcCallbackFunction zDProcCallbackfuc;
}ZDProcCallback;

class CEncoder
{
public:
	__declspec(dllexport) CEncoder(void);
	__declspec(dllexport) virtual ~CEncoder(void);
	__declspec(dllexport) virtual int Open(const wchar_t *,AudioFormat af = AutoDetect)=0 ;
	__declspec(dllexport) virtual int Save(const wchar_t *) =0;

	__declspec(dllexport) void SetProccessCallback(ZDProcCallbackFunction zDProcCallbackfuc,void *client);
public:
	public:
	CZStringA title;
	CZStringA artist;
	CZStringA album;
	CZStringA year;
	CZStringA comment;
	CZStringA genre;
	CZStringA track;
protected:
	ZDProcCallback zDProcCallback;
	CDecoder *decoder;
	float start;
	float end;
};

static bool write_little_endian_uint16(FILE *f, unsigned short x)
{
	return
		fputc(x, f) != EOF &&
		fputc(x >> 8, f) != EOF
	;
}

static bool write_little_endian_int16(FILE *f, short x)
{
	return write_little_endian_uint16(f, (unsigned short)x);
}

static bool write_little_endian_uint32(FILE *f, unsigned int x)
{
	return
		fputc(x, f) != EOF &&
		fputc(x >> 8, f) != EOF &&
		fputc(x >> 16, f) != EOF &&
		fputc(x >> 24, f) != EOF
	;
}
