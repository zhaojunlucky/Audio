#include "WvDecoder.h"
#include <math.h>

WvDecoder::WvDecoder(void)
{
	wpc = NULL;
	//input = NULL;
	//output = NULL;
}


WvDecoder::~WvDecoder(void)
{
	Close();
}

HRESULT WvDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) 
{
	m_id3.EmptyTags();
	//char ex[2048] = "wv";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}

	CCharWCharHelper cwz(strFileName);
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	wpc = WavpackOpenFileInput(cwz.GetMultiChar(),error,OPEN_TAGS | OPEN_WVC | OPEN_NORMALIZE,0);
	if(!wpc)
		return E_FAIL;
	m_pwfx->cbSize = 0;
	numchannels = m_pwfx->nChannels = WavpackGetNumChannels(wpc);
	m_pwfx->wBitsPerSample = WavpackGetBitsPerSample(wpc);
	m_pwfx->nSamplesPerSec = WavpackGetSampleRate(wpc);
	m_pwfx->wFormatTag = 1;
	m_pwfx->nBlockAlign = m_pwfx->nChannels * ((m_pwfx->wBitsPerSample+7) / 8);
	m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;
	totalSamples = WavpackGetNumSamples(wpc);
	m_id3.duration_times = (float)((double)totalSamples/(double)m_pwfx->nSamplesPerSec);
	m_dwSize = totalSamples * m_pwfx->nBlockAlign;
	bytespersample = WavpackGetBytesPerSample(wpc);
	m_nWaveDataAllocSize = BUFFER_SIZE * m_pwfx->nChannels * 4;
	m_pWaveDataAlloc = (BYTE*)malloc(m_nWaveDataAllocSize);
	m_nWaveDataNum = 0;
	m_pWaveDataBuffer = m_pWaveDataAlloc;
	m_id3.bitrate = WavpackGetAverageBitrate(wpc, WavpackGetNumChannels(wpc)/1000);

	return S_OK;
}
HRESULT WvDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) 
{
	unsigned int samplesNeed = dwSizeToRead;
	//unsigned int sampleHave = 0;
	*pdwSizeRead = 0;
	BYTE *buf = pBuffer;
	while(samplesNeed)
	{
		if(m_nWaveDataNum) // we have some data
		{
			unsigned int i ;
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
			//decode data
			assert(0 == m_nWaveDataNum);
			m_pWaveDataBuffer = m_pWaveDataAlloc;
			uint32_t numsamples = WavpackUnpackSamples(wpc,(int32_t *)m_pWaveDataAlloc,BUFFER_SIZE);
			if(numsamples)
			{
				//int numchannels=WavpackGetNumChannels(wpc);
                //int bytespersample=WavpackGetBytesPerSample(wpc);
				int32_t *expandedptr=(int32_t *)m_pWaveDataAlloc;
                uint8_t *compactptr=m_pWaveDataAlloc;

                switch(bytespersample)
                {
                    case 1:
                            for(unsigned long i=0;i<numsamples*numchannels;i++)
                            {
                                    int32_t value=*expandedptr++;
                                    *compactptr++=(value+0x80)&0xff;
                            }
                    break;

                    case 2:
                            for(unsigned  long i=0;i<numsamples*numchannels;i++)
                            {
                                    int32_t value=*expandedptr++;
                                    *compactptr++=value&0xff;
                                    *compactptr++=(value>>8)&0xff;
                            }
                    break;

                    case 3:
                            for(unsigned  long i=0;i<numsamples*numchannels;i++)
                            {
                                    int32_t value=*expandedptr++;
                                    *compactptr++=value&0xff;
                                    *compactptr++=(value>>8)&0xff;
                                    *compactptr++=(value>>16)&0xff;
                            }
                    break;

                    case 4:
                            for(unsigned  long i=0;i<numsamples*numchannels;i++)
                            {
                                    int32_t value=*expandedptr++;
                                    *compactptr++=value&0xff;
                                    *compactptr++=(value>>8)&0xff;
                                    *compactptr++=(value>>16)&0xff;
                                    *compactptr++=(value>>24)&0xff;
                            }
                    break;
                }
				m_nWaveDataNum = numchannels*numsamples*bytespersample;
			}
			else
				return E_FAIL;
		}
	}
	return S_OK;
}
HRESULT WvDecoder::ResetFile() 
{
	if(WavpackSeekSample(wpc,0))
		return S_OK;
	return E_FAIL;
}
HRESULT WvDecoder::Close() 
{
	WavpackCloseFile(wpc);
	if(m_pWaveDataAlloc)
		free(m_pWaveDataAlloc);
	m_pWaveDataAlloc = 0;
	return S_OK;
}
HRESULT WvDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		DWORD seekSize = (DWORD)(duration * (float)m_pwfx->nAvgBytesPerSec);

		if(seekSize > m_dwSize)
		{
			return E_FAIL;
		}
		*newPosition = seekSize;
		unsigned int samples = (unsigned int)((float)m_pwfx->nSamplesPerSec * (float)duration);
		if(WavpackSeekSample(wpc,samples))
			return S_OK;
	}
	return E_FAIL;
}

