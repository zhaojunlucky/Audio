#include <math.h>

#include "Mp4Decoder.h"


Mp4Decoder::Mp4Decoder(void)
{
	m_currentSample = 0;
	infile = NULL;
	mp4File = NULL;
	m_ntotalSamples = 0;
	mp4cb = NULL;
	buffer = NULL;
	buffer_size = 0;
	hDecoder = NULL;
	track = 0;
	m_pSb = NULL;
	m_seek = FALSE;
	m_seekLeftBytes = 0;
}


Mp4Decoder::~Mp4Decoder(void)
{
	Close();
}

HRESULT Mp4Decoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) 
{
	//char ex[2048] = "m4a";
	//char ex1[2048] = "mp4";
	//if(!isFile_ex(strFileName,ex) && !isFile_ex(strFileName,ex1))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}
	CCharWCharHelper cwz(strFileName);
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	/* initialise the callback structure */
    mp4cb = (mp4ff_callback_t*)malloc(sizeof(mp4ff_callback_t));
	if( NULL == mp4cb)
	{
		return M_MP4_FAIL_CREATE_CALLBACK;//E_FAIL;
	}
    fopen_s(&mp4File,cwz.GetMultiChar(), "rb");
    mp4cb->read = read_callback;
    mp4cb->seek = seek_callback;
    mp4cb->user_data = mp4File;

	infile = mp4ff_open_read(mp4cb);
    if (!infile)
    {
        /* unable to open file */
        return M_FAIL_OPEN_FILE;//E_FAIL;
    }

	if ((track = GetAACTrack(infile)) < 0)
    {
        return M_MP4_FAIL_GET_TRACK;//E_FAIL;
    }

	mp4ff_get_decoder_config(infile, track, &buffer, &buffer_size);

	hDecoder = NeAACDecOpen();

	unsigned long samplerate;
    unsigned char channels;

	if(NeAACDecInit2(hDecoder, buffer, buffer_size, &samplerate, &channels) < 0)
    {
        /* If some error initializing occured, skip the file */
        return M_MP4_FAIL_CREATE_DECODER;//E_FAIL;
    }
	m_ntotalSamples = mp4ff_num_samples(infile, track);

	long tmpSample = 0;
	m_pSb = new unsigned int[m_ntotalSamples];
	if(NULL == m_pSb)
	{
		return E_FAIL;
	}
	for(long i = 0;i < m_ntotalSamples; i++)
	{
		m_pSb[i] = mp4ff_get_sample_duration(infile,track,i);
		tmpSample += m_pSb[i];
	}
	m_id3.bitrate = mp4ff_get_avg_bitrate(infile,track) / 1000.0;
	m_pwfx->cbSize = 0;
	m_pwfx->wFormatTag = 1;
	m_pwfx->nSamplesPerSec = samplerate;
	m_pwfx->wBitsPerSample = SAMPLE_SIZE;
	m_pwfx->nChannels = channels;
	m_pwfx->nBlockAlign = m_pwfx->nChannels * m_pwfx->wBitsPerSample / 8;
	m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;

	m_dwSize = m_pwfx->nBlockAlign * tmpSample;
	m_id3.duration_times = (float)tmpSample / (float) samplerate;
	
	return S_OK;
}


HRESULT Mp4Decoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) 
{
	BYTE *buf = pBuffer;
	unsigned int samplesNeed = dwSizeToRead;
	*pdwSizeRead = 0;
	while(samplesNeed)
	{
		if(m_nWaveDataNum) // we have some data	
		{
			unsigned int i;
			if(m_nWaveDataNum >= samplesNeed)
			{
				for(i = 0; i < samplesNeed; i++)
				{
					buf[i] = m_pWaveDataBuffer[i];
				}

				*pdwSizeRead += samplesNeed;
				m_pWaveDataBuffer = &m_pWaveDataBuffer[samplesNeed];
				m_nWaveDataNum -= samplesNeed;
				samplesNeed = 0;
			}
			else
			{
				for(i = 0; i < m_nWaveDataNum; i++)
				{
					buf[i] = m_pWaveDataBuffer[i];
				}
				*pdwSizeRead += m_nWaveDataNum;
				samplesNeed -= m_nWaveDataNum;
				buf = &buf[m_nWaveDataNum];
				m_pWaveDataBuffer = m_pWaveDataAlloc;
				m_nWaveDataNum = 0;
			}
		}
		else
		{
			assert(0 == m_nWaveDataNum);
			if(m_currentSample >= m_ntotalSamples)
			{
				return E_FAIL;
			}

        int rc;

        /* get access unit from MP4 file */
        buffer = NULL;
        buffer_size = 0;

        rc = mp4ff_read_sample(infile, track, m_currentSample, &buffer, &buffer_size);
		m_currentSample++;
        if (rc == 0)
        {
            //error_handler("Reading from MP4 file failed.\n");
            //NeAACDecClose(hDecoder);
            //mp4ff_close(infile);
            //free(mp4cb);
            //fclose(mp4File);
			return M_MP4_FAIL_DECODE_DATA;//E_FAIL;
        }

        m_pWaveDataAlloc = (BYTE*)NeAACDecDecode(hDecoder, &frameInfo, buffer, buffer_size);

		if (frameInfo.error > 0)
		{
			//error_handler("Error: %s\n",
			//NeAACDecGetErrorMessage(frameInfo.error));
			//return E_FAIL;
			continue;
		}
		if ((frameInfo.error == 0) && (frameInfo.samples > 0))
		{
			if(!m_seek)
			{
				m_nWaveDataAllocSize = m_nWaveDataNum = frameInfo.channels*frameInfo.samples;
				m_pWaveDataBuffer = m_pWaveDataAlloc;
			}
			else
			{
				m_nWaveDataAllocSize = frameInfo.channels*frameInfo.samples;
				if(m_seekLeftBytes < m_nWaveDataAllocSize)
				{
					m_nWaveDataNum = m_nWaveDataAllocSize - m_seekLeftBytes;
					m_pWaveDataBuffer = &m_pWaveDataAlloc[m_seekLeftBytes ];
					m_seek = FALSE;
				}
				else
				{
					m_seekLeftBytes -= m_nWaveDataAllocSize;
				}
			}
		}
        if (buffer)
            free(buffer);

		}
	}
	return S_OK;
}
HRESULT Mp4Decoder::ResetFile() 
{
	m_nWaveDataNum = 0;
	m_currentSample = 0;
	m_seek = FALSE;
	m_seekLeftBytes = 0;
	return S_OK;
}
HRESULT Mp4Decoder::Close() 
{
	
	//if(buffer)
	//{
	//	free(buffer);
	//}
	if(NULL != hDecoder)
		NeAACDecClose(hDecoder);
	if(NULL != infile)
		mp4ff_close(infile);
	if(NULL != mp4cb)
		free(mp4cb);
	if(NULL != mp4File)
		fclose(mp4File);
	hDecoder = NULL;
	infile = NULL;
	mp4File = NULL;
	mp4cb = NULL;
	if(NULL != m_pSb)
	{
		delete []m_pSb;
	}
	m_pSb = 0;
	m_pWaveDataAlloc = NULL;
	return S_OK;
}
HRESULT Mp4Decoder::Seek(float duration ,DWORD *newPosition )
{
	*newPosition = 0;
	if(duration < 0 || duration >= m_id3.duration_times)
	{
		return M_DURATION_OUT_OF_RANGE;//E_FAIL;
	}

	if(0 == duration)
	{
		*newPosition = 0;
		return ResetFile();
	}
	else
	{
		DWORD seekSize = (DWORD)(duration * m_pwfx->nAvgBytesPerSec);

		if(seekSize > m_dwSize)
		{
			return E_FAIL;
		}
		*newPosition = seekSize;
		float samples = ((float)m_pwfx->nSamplesPerSec * (float)duration);
		//float tmp = (float)samples / (float)m_pwfx->nBlockAlign;
		unsigned int tmpNum = (unsigned int)ceil(samples);
		unsigned int tmpTotals = 0;
		long i;
		for(i = 0; i < m_ntotalSamples; i++)
		{
			tmpTotals += m_pSb[i];
			if(tmpTotals >= tmpNum)
			{
				break;
			}
		}
		m_currentSample = i;
		m_nWaveDataNum = 0;
		m_seekLeftBytes = (unsigned int)((float)m_pwfx->nBlockAlign * ((float)tmpNum - samples));
		m_seek = TRUE;
	}
	return S_OK;
}

uint32_t Mp4Decoder::read_callback(void *user_data, void *buffer, uint32_t length)
{
	return fread(buffer, 1, length, (FILE*)user_data);
}
uint32_t Mp4Decoder::seek_callback(void *user_data, uint64_t position)
{
    return (uint32_t)fseek((FILE*)user_data, (long)position, SEEK_SET);
}

int Mp4Decoder::GetAACTrack(mp4ff_t *infile)
{
    /* find AAC track */
    int i, rc;
    int numTracks = mp4ff_total_tracks(infile);

    for (i = 0; i < numTracks; i++)
    {
        unsigned char *buff = NULL;
        unsigned int b_size = 0;
        mp4AudioSpecificConfig mp4ASC;

        mp4ff_get_decoder_config(infile, i, &buff,&b_size);

        if (buff)
        {
            rc = NeAACDecAudioSpecificConfig(buff, b_size, &mp4ASC);
            free(buff);

            if (rc < 0)
                continue;
            return i;
        }
    }

    /* can't decode this */
    return -1;
}