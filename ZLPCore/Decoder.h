#pragma once

#include "..\Audio\AudioDecoder.h"
#include "..\Audio\WaveDecoder.h"
#include "..\Audio\FlacDecoder.h"
#include "..\Audio\Mp3Decoder.h"
#include "..\Audio\ApeDecoder.h"
#include "..\Audio\OggDecoder.h"
#include "..\Audio\AacDecoder.h"
#include "..\Audio\Mp4Decoder.h"
#include "..\Audio\TtaDecoder.h"
#include "..\Audio\OfrDecoder.h"
#include "..\Audio\WvDecoder.h"
#include "..\Audio\TakDecoder.h"
#include "AudioBase.h"

class CDecoder
{
public:
	__declspec(dllexport) CDecoder(void);
	__declspec(dllexport) virtual ~CDecoder(void);
	__declspec(dllexport) int Open(const wchar_t *,AudioFormat af = AutoDetect);
	__declspec(dllexport) int Read(BYTE *,DWORD ,DWORD *);
	__declspec(dllexport) int ResetFile();
	__declspec(dllexport) int Close();
	__declspec(dllexport) int Seek(DWORD ,DWORD *);
	__declspec(dllexport) WAVEINFO *GetWaveInfo();
	__declspec(dllexport) DWORD CalcaulateWaveSize(float);
	__declspec(dllexport) float CalcalateTime(DWORD );
	__declspec(dllexport) bool IsValidDecoder();
	__declspec(dllexport) ID3Info *GetID3Info(); 
private:
	AudioDecoder *aDecoder;
	wchar_t *file;
	WAVEINFO waveInfo;
	bool isValid;
};

