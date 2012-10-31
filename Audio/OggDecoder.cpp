#include "OggDecoder.h"


OggDecoder::OggDecoder(void)
{
	infile = NULL;
	current_section = 0;
	eof = 0;
	m_pWaveDataAlloc = new BYTE[OGG_BUFFER_SIZE];
	m_pWaveDataBuffer = m_pWaveDataAlloc;
}


OggDecoder::~OggDecoder(void)
{
	Close();
}

HRESULT OggDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	m_id3.EmptyTags();
	//char ex[2048] = "ogg";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}

	CCharWCharHelper cwz(strFileName);
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	} 
	fopen_s(&infile,cwz.GetMultiChar(), "rb"); // open the ogg file
	if(!infile)
	{
		return M_FAIL_OPEN_FILE;//E_FAIL;
	}
	if (ov_open_callbacks(infile, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
	{
		return M_OGG_FAIL_CREATE_DECODER;//E_FAIL;
	}
	vorbis_info *vi=ov_info(&vf,-1);
	m_pwfx->cbSize = 0;
	m_pwfx->wFormatTag = 1;
	m_pwfx->nChannels = vi->channels;
	m_pwfx->nSamplesPerSec = vi->rate;
	m_pwfx->wBitsPerSample = 16;
	m_pwfx->nBlockAlign = m_pwfx->nChannels * ( m_pwfx->wBitsPerSample / 8);
	m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;
	m_dwSize = (long)ov_pcm_total(&vf,-1) * m_pwfx->nBlockAlign;
	m_id3.duration_times = (float)m_dwSize / (float) m_pwfx->nAvgBytesPerSec;
	m_id3.bitrate = vi->bitrate_nominal;
	return S_OK;
}
HRESULT OggDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	BYTE *buf = pBuffer;
	unsigned int samplesNeed = dwSizeToRead;
	*pdwSizeRead = 0;

	while(samplesNeed)
	{
		if(m_nWaveDataNum)
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
			if(eof)
			{
				return E_FAIL;
			}
			long ret=ov_read(&vf,(char*)m_pWaveDataAlloc,OGG_BUFFER_SIZE,0,2,1,&current_section);
			//word Specifies word size. Possible arguments are 1 for 8-bit samples,
			//or 2 or 16-bit samples. Typical value is 2.
			if(0 == ret)
			{
				eof = 1;
			}
			if( ret < 0)
			{
				continue;
			}
			m_nWaveDataNum = ret;
			m_pWaveDataBuffer = m_pWaveDataAlloc;
		}
	}
	return S_OK;
}
HRESULT OggDecoder::ResetFile() 
{
	m_nWaveDataNum = 0;
	current_section = 0;
	eof = 0;
	return ( 0 == ov_time_seek(&vf,0.0));
}
HRESULT OggDecoder::Close() 
{
	ov_clear(&vf);
	if(infile)
	{
		fclose(infile);
	}
	infile = NULL;
	return S_OK;
}
HRESULT OggDecoder::Seek(float duration ,DWORD *newPosition )
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
		m_nWaveDataNum = 0;
		eof = 0;
		return (0 == ov_time_seek(&vf,(float)duration));
	}
	return S_OK;
}