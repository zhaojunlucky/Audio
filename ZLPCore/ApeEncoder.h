#pragma once
#include "encoder.h"

#include "..\decoders\MAC_SDK\Shared\All.h"
#include "..\decoders\MAC_SDK\Shared\MACLib.h"

class CApeEncoder :
	public CEncoder
{
public:
	__declspec(dllexport) CApeEncoder(void);
	__declspec(dllexport) virtual ~CApeEncoder(void);
	__declspec(dllexport) int Open(const wchar_t *,AudioFormat af = AutoDetect);
	__declspec(dllexport) int SetTimes(float s,float e);
	__declspec(dllexport) int SetCompressionLevel(int compressionLevel);
	__declspec(dllexport) int Save(const wchar_t *);
private:
	int compLevel;
};

