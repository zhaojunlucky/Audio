#pragma once
#include "encoder.h"

#define STREAMING_BUFFER_SIZE 65536
static BYTE buffer[STREAMING_BUFFER_SIZE];

class CWaveEncoder :
	public CEncoder
{
public:
	__declspec(dllexport) CWaveEncoder(void);
	__declspec(dllexport) virtual ~CWaveEncoder(void);

	__declspec(dllexport) int Open(const wchar_t *,AudioFormat af = AutoDetect);
	__declspec(dllexport) int SetTimes(float s,float e);
	__declspec(dllexport) int Save(const wchar_t *);
private:
	int WriteWaveFile(const wchar_t *);
};

