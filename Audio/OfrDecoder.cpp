#include "OfrDecoder.h"


OfrDecoder::OfrDecoder(void)
{
	decoder = NULL;
}


OfrDecoder::~OfrDecoder(void)
{
	Close();
}

HRESULT OfrDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) 
{
	m_id3.EmptyTags();
	char ex[512] = "ofr";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}
	CCharWCharHelper cwz(strFileName);
	strcpy_s(ex,512,cwz.GetMultiChar());
	ex[cwz.GetMultiByteLength()]='\0';
	decoder = new OptimFROGDecoder;
	if(!decoder)
		return E_FAIL;
	condition_t ret = decoder->open(ex);
	if(!ret)
		return E_FAIL;
	m_id3.bitrate = decoder->bitrate;
	m_pwfx->cbSize = sizeof(WAVEFORMATEX);
	m_pwfx->nChannels = decoder->channels;
	m_pwfx->wBitsPerSample = decoder->bitspersample;
	m_pwfx->nSamplesPerSec = decoder->samplerate;
	m_pwfx->nBlockAlign = decoder->channels * ((decoder->bitspersample+7) / 8);
	m_pwfx->wFormatTag = 1;
	m_pwfx->nAvgBytesPerSec =  m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;
	m_id3.duration_times = (float)decoder->length_ms / 1000.0f;
	m_dwSize = (DWORD)((float)m_pwfx->nAvgBytesPerSec * m_id3.duration_times);
	m_nWaveDataAllocSize = 65536;
	m_pWaveDataAlloc = new BYTE[m_nWaveDataAllocSize];

	return S_OK;
}
HRESULT OfrDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) 
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
			m_nWaveDataNum = decoder->readBytes((void*)m_pWaveDataAlloc,m_nWaveDataAllocSize);
			if(m_nWaveDataNum <= 0)
			{
				m_nWaveDataNum = 0;
				return E_FAIL;
			}
		}
	}
	return S_OK;
}
HRESULT OfrDecoder::ResetFile() 
{
	if(!decoder->seekTimeDouble(0))
		return E_FAIL;
	return S_OK;
}
HRESULT OfrDecoder::Close() 
{
	if(decoder)
		decoder->close();
	SAFE_DELETE(decoder);
	return S_OK;
}
HRESULT OfrDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		m_nWaveDataNum = 0;

		
		if(!decoder->seekTimeDouble(duration))
			return E_FAIL;
		*newPosition = seekSize;
	}
	return S_OK;
}
