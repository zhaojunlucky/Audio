#include "MP3Encoder.h"


CMP3Encoder::CMP3Encoder(void)
{
	quality = 2;
	mpegMode = STEREO;
	bitrate = 320;
}


CMP3Encoder::~CMP3Encoder(void)
{
}


int CMP3Encoder::Open(const wchar_t *aFile,AudioFormat af )
{
	start = 0;
	int r =  decoder->Open(aFile,af);

	end = decoder->GetWaveInfo()->durationTime;
	return r;
}
int CMP3Encoder::SetTimes(float s,float e)
{
	if(s >= 0 && s < e && e <= decoder->GetWaveInfo()->durationTime)
	{
		start = s;
		end = e;
	}
	return 0;
}

int CMP3Encoder::Save(const wchar_t *mFile)
{
	char * mfile = 0;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP,NULL,mFile,-1,NULL,0,NULL,FALSE);
	mfile = new char[dwNum + 1];
	WideCharToMultiByte (CP_OEMCP,NULL,mFile,-1,mfile,dwNum,NULL,FALSE);
	mfile[dwNum] = '\0';
	FILE *file = fopen(mfile,"wb+");
	delete mfile;
	if(!file)
		return 1;

	lame_global_flags *gfp;
    gfp = lame_init ();

	if(!gfp)
		return 2;

	id3tag_init(gfp);
	
	id3tag_set_title( gfp, title.GetData());
	id3tag_set_artist( gfp, artist.GetData());
	id3tag_set_album( gfp,album.GetData());
	id3tag_set_year( gfp, year.GetData());
	id3tag_set_comment( gfp,comment.GetData());
            

	id3tag_set_track( gfp,track.GetData());

	id3tag_set_genre( gfp,genre.GetData());

	id3tag_v2_only(gfp);
	WAVEINFO waveInfo = *decoder->GetWaveInfo();
	lame_set_num_channels(gfp, waveInfo.nChannels);
    lame_set_in_samplerate(gfp, waveInfo.nSamplesPerSec);
    lame_set_brate(gfp, bitrate);
    lame_set_mode(gfp, mpegMode);
    lame_set_quality(gfp, quality);  

	lame_init_params (gfp);

	DWORD size = decoder->CalcaulateWaveSize(end) - decoder->CalcaulateWaveSize(start);

	DWORD dwRead = 0;
	DWORD curPos = 0;
	DWORD cbValid;
	DWORD has;
	decoder->Seek(start,&curPos);
	DWORD sSize = curPos;
	DWORD rSize = INBUFSIZE*2;
	int mp3_bytes;
	while(curPos < size)
	{
		dwRead = 0;
		decoder->Read((BYTE*)buffer,rSize,&dwRead);
		cbValid = min(rSize,dwRead);
		has = size - curPos;
		if(!has)
			cbValid += has;
		else
			cbValid = min(has,dwRead);

		curPos += cbValid;
		if(!cbValid)
		{
			cbValid = waveInfo.nBlockAlign;
			curPos = size;
			memset(buffer,0,rSize);
		}
		//fwrite(buffer,1,cbValid,file);
		mp3_bytes = lame_encode_buffer_interleaved(gfp, buffer,dwRead/4, mp3Buffer, MP3BUFSIZE);
        if (mp3_bytes < 0) {
            break;
        } else if(mp3_bytes > 0) {
            fwrite(mp3Buffer, 1, mp3_bytes, file);
        }
		if(0 != zDProcCallback.zDProcCallbackfuc)
			zDProcCallback.zDProcCallbackfuc(this,1,size,curPos-sSize,zDProcCallback.client);
	}
	mp3_bytes = lame_encode_flush(gfp, mp3Buffer, MP3BUFSIZE);
    if (mp3_bytes > 0) {
        printf("writing %d mp3 bytes\n", mp3_bytes);
        fwrite(mp3Buffer, 1, mp3_bytes, file);
    }

	fclose(file);
	lame_close(gfp);
	return 0;
}

int CMP3Encoder::SetQuality(int q)
{
	if(q >=0 && q<=9)
		quality = q;
	return 0;
}
int CMP3Encoder::SetMPEG_Mode(int m)
{
	if(m >=0 && m <= 4 && 2 != m)
		mpegMode = (MPEG_mode)m;
	return 0;
}
int CMP3Encoder::SetBitrate(int b)
{
	if(b <= 64)
		bitrate = 64;
	else if(b <= 128)
		bitrate = 128;
	else if(b <= 192)
		bitrate = 192;
	else if(b <= 256)
		bitrate = 256;
	else
		bitrate = 320;
	return 0;
}