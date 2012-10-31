#include "WaveEncoder.h"
#include "..\Utility\FileHelper.h"

CWaveEncoder::CWaveEncoder(void)
{
}


CWaveEncoder::~CWaveEncoder(void)
{
}


int CWaveEncoder::Open(const wchar_t * aFile,AudioFormat af)
{
	start = 0;
	int r =  decoder->Open(aFile,af);

	end = decoder->GetWaveInfo()->durationTime;
	return r;
}

int CWaveEncoder::Save(const wchar_t * wFile)
{
	return WriteWaveFile(wFile);
}

int CWaveEncoder::WriteWaveFile(const wchar_t * wFile)
{
	char * mfile = 0;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,wFile,-1,NULL,0,NULL,FALSE);
	mfile = new char[dwNum + 1];
	WideCharToMultiByte (CP_OEMCP,NULL,wFile,-1,mfile,dwNum,NULL,FALSE);
	mfile[dwNum] = '\0';
	CFileHelper fileHelper;
	
	FILE *file = fopen(mfile,"wb+");
	delete mfile;
	if(!file)
	{
		return 1;
	}
	DWORD curPos = 0;
	decoder->Seek(start,&curPos);

	unsigned int size = decoder->CalcaulateWaveSize(end)- decoder->CalcaulateWaveSize(start) ;
	// RIFF WAVE Chunk
	fwrite("RIFF",1,4,file);	
	//fwrite(&size,4,1,file);
	write_little_endian_uint32(file,size+36);
	fwrite("WAVE",1,4,file);
	//end
	// Format Chunk
	fwrite("fmt ",1,4,file);
	//fwrite((int*)&size,4,1,file);
	write_little_endian_uint32(file,16);
	WAVEINFO waveInfo = *decoder->GetWaveInfo();
	//fwrite(&waveInfo.wFormatTag,2,1,file);
	write_little_endian_uint16(file,waveInfo.wFormatTag);
	//fwrite(&waveInfo.nChannels,2,1,file);
	write_little_endian_uint16(file,waveInfo.nChannels);
	//fwrite(&waveInfo.nSamplesPerSec,4,1,file);
	write_little_endian_uint32(file,waveInfo.nSamplesPerSec);
	//fwrite(&waveInfo.nAvgBytesPerSec,4,1,file);
	write_little_endian_uint32(file,waveInfo.nAvgBytesPerSec);
	//fwrite(&waveInfo.nBlockAlign,2,1,file);
	write_little_endian_uint16(file,waveInfo.nBlockAlign);
	//fwrite(&waveInfo.wBitsPerSample,2,1,file);
	write_little_endian_uint16(file,waveInfo.wBitsPerSample);
	//fwrite(&waveInfo.cbSize,2,1,file);
	//write_little_endian_uint16(file,waveInfo.cbSize);

	// data Chunk
	fwrite("data",1,4,file);
	//fwrite(&waveInfo.waveSize,4,1,file);
	write_little_endian_uint32(file,size);


	
	DWORD endPos = decoder->CalcaulateWaveSize(end);
	DWORD cbValid;
	DWORD has;
	DWORD dwRead;
	DWORD sSize = curPos;
	while(curPos < endPos)
	{
		dwRead = 0;
		decoder->Read(buffer,STREAMING_BUFFER_SIZE,&dwRead);
		cbValid = min(STREAMING_BUFFER_SIZE,dwRead);
		has = endPos - curPos;
		if(!has)
			cbValid += has;
		else
			cbValid = min(has,dwRead);

		curPos += cbValid;
		if(!cbValid)
		{
			cbValid = waveInfo.nBlockAlign;
			curPos = endPos;
			memset(buffer,0,STREAMING_BUFFER_SIZE);
		}
		//fwrite(buffer,1,cbValid,file);
		for(int i = 0;i < cbValid;i++)
			//write_little_endian_uint16(file,buffer[i]);
			fputc(buffer[i],file);

		if(0 != zDProcCallback.zDProcCallbackfuc)
			zDProcCallback.zDProcCallbackfuc(this,1,size,curPos-sSize,zDProcCallback.client);
	}
	fclose(file);
	return 0;
}

int CWaveEncoder::SetTimes(float s,float e)
{
	if(s >= 0 && s < e && e <= decoder->GetWaveInfo()->durationTime)
	{
		start = s;
		end = e;
	}
	return 0;
}