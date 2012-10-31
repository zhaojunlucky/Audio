#include "TakDecoder.h"


TakDecoder::TakDecoder(void)
{
	ReadNum = 1;
}


TakDecoder::~TakDecoder(void)
{
	Close() ;
}

HRESULT TakDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) 
{
	m_id3.EmptyTags();
	//char ex[2048] = "tak";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}

	CCharWCharHelper cwz(strFileName);

	Options.Cpu   = tak_Cpu_Any;
	Options.Flags = 0;//tak_ssd_opt_SequentialRead;
	//TtakStreamIoInterface     sioi = { _CanRead, _CanWrite, _CanSeek, _Read, NULL, NULL, NULL, _Seek, _GetLength };
	Decoder = tak_SSD_Create_FromFile (cwz.GetMultiChar(), &Options, NULL, NULL);
	if(!Decoder)
		return E_FAIL;
	if (tak_SSD_Valid (Decoder) == tak_True)
	{
		Ttak_str_StreamInfo           StreamInfo;
		if (tak_SSD_GetStreamInfo (Decoder, &StreamInfo) != tak_res_Ok) 
			return E_FAIL;
		
		m_pwfx->cbSize = sizeof(WAVEFORMATEX);
		m_pwfx->nChannels = StreamInfo.Audio.ChannelNum;
		m_pwfx->wBitsPerSample = StreamInfo.Audio.SampleBits;
		m_pwfx->nSamplesPerSec = StreamInfo.Audio.SampleRate;
		m_pwfx->wFormatTag = 1;
		m_pwfx->nBlockAlign = StreamInfo.Audio.BlockSize;//StreamInfo.Audio.ChannelNum * ((StreamInfo.Audio.SampleBits+7) / 8);
		m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;
		m_dwSize = (DWORD)(StreamInfo.Sizes.SampleNum * m_pwfx->nBlockAlign);
		m_id3.duration_times = (float)StreamInfo.Sizes.SampleNum / (float)m_pwfx->nSamplesPerSec;
		m_nWaveDataAllocSize = StreamInfo.Sizes.FrameSizeInSamples * StreamInfo.Audio.BlockSize;
		SamplesPerBuf = StreamInfo.Sizes.FrameSizeInSamples;
		m_pWaveDataAlloc = new BYTE[m_nWaveDataAllocSize];
		if( NULL == m_pWaveDataAlloc)
		{
			return E_FAIL;
		}
		m_pWaveDataBuffer = m_pWaveDataAlloc;
		m_nWaveDataNum = 0;
		m_id3.bitrate = tak_SSD_GetCurFrameBitRate(Decoder);// error
	}
	else
		return E_FAIL;
	return S_OK;
}
HRESULT TakDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) 
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
			if(ReadNum <= 0)
				return E_FAIL;
			TtakResult OpResult = tak_SSD_ReadAudio (Decoder, (void*)m_pWaveDataAlloc, SamplesPerBuf, &ReadNum);
			if (   (OpResult != tak_res_Ok)
				&& (OpResult != tak_res_ssd_FrameDamaged)) 
				return E_FAIL;
			m_pWaveDataBuffer = m_pWaveDataAlloc;
			m_nWaveDataNum = ReadNum * m_pwfx->nBlockAlign;
		}
	}
	return S_OK;
}
HRESULT TakDecoder::ResetFile() 
{
	if ( tak_True != tak_SSD_Valid( Decoder))
		return E_FAIL;
	m_nWaveDataNum = 0;
	ReadNum = 1;
	int ret = tak_SSD_Seek(Decoder,0) ;
	if ( tak_True != tak_SSD_Valid( Decoder))
		return E_FAIL;
	return S_OK;
}
HRESULT TakDecoder::Close() 
{
	tak_SSD_Destroy (Decoder);
	Decoder = 0;
	return S_OK;
}
HRESULT TakDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		if ( tak_True != tak_SSD_Valid( Decoder))
			return E_FAIL;
		m_nWaveDataNum = 0;

		*newPosition = seekSize;
		ReadNum = (int)((float)m_pwfx->nSamplesPerSec * (float)duration);
		int r = tak_SSD_Seek(Decoder,ReadNum) ;
		if ( tak_True != tak_SSD_Valid( Decoder))
			return E_FAIL;
	}
	return S_OK;
}
