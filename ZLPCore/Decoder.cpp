#include "Decoder.h"


CDecoder::CDecoder(void)
{
	aDecoder = NULL;
	file = new wchar_t[10];
	file[0] = L'\0';
	memset(&waveInfo,0,sizeof(WAVEINFO));
	isValid = false;
}


CDecoder::~CDecoder(void)
{
	if(aDecoder)
	{
		aDecoder->Close();
		delete aDecoder;
	}
	if(file)
		delete file;
}

// return code: 0  success
//				-1  unsupport file
//				1 fail to open file

int CDecoder::Open(const wchar_t * fileName,AudioFormat af )
{
	isValid = false;
	memset(&waveInfo,0,sizeof(WAVEINFO));
	int inputLength = wcslen(fileName);
	int alloctedLength = wcslen(file);
	if(alloctedLength <= inputLength)
	{
		if(file)
			delete file;

		file = new wchar_t[inputLength + 1];		
	}
	wcscpy(file,fileName);
	file[inputLength] = L'\0';
	if(aDecoder)
	{
		aDecoder->Close();
		delete aDecoder;
	}

	if(AutoDetect == af)// try to get the audio file format
		af = CAudioBase::IsSupportMedia(file);

	if(FLAC == af)
		aDecoder = new FlacDecoder;
	else if(APE == af)
		aDecoder = new ApeDecoder;
	else if(WAV == af )
		aDecoder = new WaveDecoder;
	else if(MP3 == af)
		aDecoder = new Mp3Decoder;
	else if(OGG == af)
		aDecoder = new OggDecoder;
	else if(TTA == af)
		aDecoder = new TtaDecoder;
	else if(AAC == af)
		aDecoder = new AacDecoder;
	else if(MP4 == af)
		aDecoder = new Mp4Decoder;
	else if(OFR == af)
		aDecoder = new OfrDecoder;
	else if(TAK == af)
		aDecoder = new TakDecoder;
	else if(WV == af)
		aDecoder = new WvDecoder;
	else 
		return -1;

	if(FAILED(aDecoder->Open(file,0,WAVEFILE_READ)))
		return 1;
	// set wave header information
	WAVEFORMATEX* pwfx = aDecoder->GetFormat();
	waveInfo.cbSize = pwfx->cbSize;
	waveInfo.durationTime = aDecoder->GetID3Info()->duration_times;
	waveInfo.nAvgBytesPerSec = pwfx->nAvgBytesPerSec;
	waveInfo.nBlockAlign = pwfx->nBlockAlign;
	waveInfo.nChannels = pwfx->nChannels;
	waveInfo.nSamplesPerSec = pwfx->nSamplesPerSec;
	waveInfo.waveSize = aDecoder->GetSize();
	waveInfo.wBitsPerSample = pwfx->wBitsPerSample;
	waveInfo.wFormatTag = pwfx->wFormatTag;
	waveInfo.bitrate = aDecoder->GetID3Info()->bitrate;
	isValid = true;
	return 0;
}

bool CDecoder::IsValidDecoder()
{
	return isValid;
}

int CDecoder::Close()
{
	if(FAILED(aDecoder->Close()))
		return -1;
	return 0;
}

int CDecoder::Read(BYTE *buffer,DWORD sizeToRead,DWORD *sizeReaded)
{
	if(FAILED(aDecoder->Read(buffer,sizeToRead,sizeReaded)))
		return -1;
	return 0;
}

int CDecoder::ResetFile()
{
	if(FAILED(aDecoder->ResetFile()))
		return -1;
	return 0;
}

int CDecoder::Seek(DWORD seekDuration,DWORD *newPosition)
{
	if(FAILED(aDecoder->Seek(seekDuration,newPosition)))
		return -1;
	return 0;
}
ID3Info *CDecoder::GetID3Info()
{
	return aDecoder->GetID3Info();
}
// check if the audio file is supported by the decoder,
// return AutoDetect means that the file is not supported
//AudioFormat CDecoder::IsSupportMedia(const wchar_t* file)
//{
//	int length = wcslen(file);
//	int i = 0,j=0;
//	for(i = length - 1;i > 3;i--)
//		if(L'.' == file[i])
//			break;
//	wchar_t extension[25];
//	for(j = i+1;j < length ; j++)
//	{
//		if(file[j] >= L'a' || file[j] <= L'z')
//			extension[j - i - 1] = file[j] - 32;
//		else
//			extension[j - i - 1] = file[j];
//	}
//	extension[j] = L'\0';
//	if (0 == wcscmp(extension,L"FLAC"))
//	{
//		return FLAC;
//	}
//	else if (0 == wcscmp(extension,L"APE"))
//	{
//		return APE;
//	}
//	else if (0 == wcscmp(extension,L"WAV"))
//	{
//		return WAV;
//	}
//	else if (0 == wcscmp(extension,L"CUE"))
//	{
//		return CUE;
//	}
//	else if (0 == wcscmp(extension,L"MP3"))
//	{
//		return MP3;
//	}
//	else if (0 == wcscmp(extension,L"OGG"))
//	{
//		return OGG;
//	}
//	else if (0 == wcscmp(extension,L"TTA"))
//	{
//		return TTA;
//	}
//	else if (0 == wcscmp(extension,L"AAC"))
//	{
//		return AAC;
//	}
//	else if (0 == wcscmp(extension,L"MP4"))
//	{
//		return MP4;
//	}
//	else if (0 == wcscmp(extension,L"M4A"))
//	{
//		return M4A;
//	}
//	else if (0 == wcscmp(extension,L"OFR"))
//	{
//		return OFR;
//	}
//	else if (0 == wcscmp(extension,L"TAK"))
//	{
//		return TAK;
//	}
//	else if (0 == wcscmp(extension,L"WV"))
//	{
//		return WV;
//	}
//	return AutoDetect;
//}

WAVEINFO *CDecoder::GetWaveInfo()
{
	return &this->waveInfo;
}

DWORD CDecoder::CalcaulateWaveSize(float d)
{
	return 0==(int)waveInfo.durationTime?0:(DWORD)((float)waveInfo.waveSize * d / waveInfo.durationTime);
}

float CDecoder::CalcalateTime(DWORD l)
{
	return 0==l?0:(float)((double)l * waveInfo.durationTime/ (double) waveInfo.waveSize);
}