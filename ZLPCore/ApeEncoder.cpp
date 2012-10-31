#include "ApeEncoder.h"


CApeEncoder::CApeEncoder(void)
{
	compLevel = COMPRESSION_LEVEL_NORMAL;
}


CApeEncoder::~CApeEncoder(void)
{
}


int CApeEncoder::Open(const wchar_t *aFile,AudioFormat af)
{
	start = 0;
	int r =  decoder->Open(aFile,af);

	end = decoder->GetWaveInfo()->durationTime;
	return r;
}

int CApeEncoder::SetTimes(float s,float e)
{
	if(s >= 0 && s < e && e <= decoder->GetWaveInfo()->durationTime)
	{
		start = s;
		end = e;
	}
	return 0;
}

int CApeEncoder::SetCompressionLevel(int compressionLevel)
{
	if(1000 == compressionLevel || 2000 == compressionLevel
		|| 3000 == compressionLevel || 4000 == compressionLevel
		|| 5000 == compressionLevel)
		compLevel = compressionLevel;

	return 0;
}

int CApeEncoder::Save(const wchar_t *aFile)
{
	WAVEINFO waveInfo = *decoder->GetWaveInfo();
	WAVEFORMATEX wfeAudioFormat;
	wfeAudioFormat.cbSize = waveInfo.cbSize;
	wfeAudioFormat.nAvgBytesPerSec = waveInfo.nAvgBytesPerSec;
	wfeAudioFormat.nBlockAlign = waveInfo.nBlockAlign;
	wfeAudioFormat.nChannels = waveInfo.nChannels;
	wfeAudioFormat.nSamplesPerSec = waveInfo.nSamplesPerSec;
	wfeAudioFormat.wBitsPerSample = waveInfo.wBitsPerSample;
	wfeAudioFormat.wFormatTag = waveInfo.wFormatTag;

	IAPECompress * pAPECompress = CreateIAPECompress();
	unsigned int size = decoder->CalcaulateWaveSize(end)- decoder->CalcaulateWaveSize(start) ;
	DWORD curPos = 0;
	decoder->Seek(start,&curPos);
	int nRetVal = pAPECompress->Start(aFile, &wfeAudioFormat, size, 
		compLevel, NULL, CREATE_WAV_HEADER_ON_DECOMPRESSION);

	if (nRetVal != 0)
	{
		SAFE_DELETE(pAPECompress)
		printf("Error starting encoder.\n");
		return 2;
	}
	DWORD nAudioBytesLeft = size;

	while (nAudioBytesLeft > 0)
	{
		///////////////////////////////////////////////////////////////////////////////
		// NOTE: we're locking the buffer used internally by MAC and copying the data
		//		 directly into it... however, you could also use the AddData(...) command
		//       to avoid the added complexity of locking and unlocking 
		//       the buffer (but it may be a little slower )
		///////////////////////////////////////////////////////////////////////////////
	
		// lock the compression buffer
		int nBufferBytesAvailable = 0;
		unsigned char * pBuffer = pAPECompress->LockBuffer(&nBufferBytesAvailable);
	
		// fill the buffer with white noise
		int need = min(nBufferBytesAvailable, nAudioBytesLeft);
		DWORD dwRead;
		decoder->Read(pBuffer,need,&dwRead);
		if(dwRead != need)
		{
			break;
		}
		// unlock the buffer and let it get processed
		int nRetVal = pAPECompress->UnlockBuffer(need, TRUE);
		if (nRetVal != 0)
		{
			printf("Error Encoding Frame (error: %d)\n", nRetVal);
			break;
		}
		if(0 != zDProcCallback.zDProcCallbackfuc)
			zDProcCallback.zDProcCallbackfuc(this,1,size,size-nAudioBytesLeft,zDProcCallback.client);
		// update the audio bytes left
		nAudioBytesLeft -= need;
	}

	if (pAPECompress->Finish(NULL, 0, 0) != 0)
	{
		printf("Error finishing encoder.\n");
	}


	SAFE_DELETE(pAPECompress)
	printf("Done.\n");
	return 0;
}