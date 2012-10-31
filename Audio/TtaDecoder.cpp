#include "TtaDecoder.h"

TtaDecoder::TtaDecoder(void)
{
	TTA = NULL;
	infile = NULL;
	ret = 1;
}


TtaDecoder::~TtaDecoder(void)
{
	Close();
}

HRESULT TtaDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	m_id3.EmptyTags();
	//char ex[2048] = "tta";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}

	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	infile = tta_open_read(strFileName);
	if(NULL == infile)
	{
		return M_FAIL_OPEN_FILE;//E_FAIL;
	}
	TTA_info info;

	io.iocb.read = &read_callback;
	io.iocb.seek = &seek_callback;
	io.iocb.write = NULL;
	io.handle = infile;

	TTA = new tta_decoder((TTA_io_callback *) &io);

	if(NULL == TTA)
	{
		return M_TTA_FAIL_CREATE_DECODER;//E_FAIL;
	}

	try 
	{
		TTA->init_get_info(&info, 0);
		m_id3.bitrate = TTA->get_rate();
	} 
	catch (tta_exception ex) 
	{
		return M_TTA_FAIL_GET_INFO;//E_FAIL;
	}

	m_pwfx->cbSize = 0;
	m_pwfx->wFormatTag = 1;
	m_pwfx->nChannels = info.nch;
	m_pwfx->wBitsPerSample = info.bps;
	m_pwfx->nSamplesPerSec = info.sps;
	m_pwfx->nBlockAlign = info.nch * ((info.bps+7) / 8);
	m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;
	m_dwSize = m_pwfx->nBlockAlign * info.samples;
	m_id3.duration_times = (float)info.samples / (float)info.sps;

	m_nWaveDataAllocSize = PCM_BUFFER_LENGTH * m_pwfx->nBlockAlign ;
	m_pWaveDataAlloc = new BYTE[m_nWaveDataAllocSize];
	if( NULL == m_pWaveDataAlloc)
	{
		return E_FAIL;
	}
	return S_OK;
}
HRESULT TtaDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) 
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
			// decode data
			if(ret <= 0)
			{
				return E_FAIL;//
			}
			try
			{
				ret = TTA->process_stream((TTAuint8*)m_pWaveDataAlloc, m_nWaveDataAllocSize);
			}
			catch (tta_exception ex)
			{
				//ex.code();
			}
			m_pWaveDataBuffer = m_pWaveDataAlloc;
			if(ret <= 0)
			{
				m_nWaveDataNum = 0;				
			}
			else
			{
				m_nWaveDataNum = ret * m_pwfx->nBlockAlign;

			}
		}
	}
	return S_OK;
}
HRESULT TtaDecoder::ResetFile() 
{
	ret = 1;
	unsigned int times = 0;
	m_nWaveDataNum = 0;
	TTA->set_position(0,&times);
	return S_OK;
}
HRESULT TtaDecoder::Close() 
{
	if(infile)
	{
		tta_close(infile);
	}
	infile = NULL;
	if(TTA)
	{
		delete TTA;
	}
	TTA = NULL;
	return S_OK;
}
HRESULT TtaDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		unsigned int times = 0;
		m_nWaveDataNum = 0;
		TTA->set_position((unsigned int)duration,&times);
		*newPosition += (DWORD)((times - duration) * m_pwfx->nAvgBytesPerSec);
	}
	return S_OK;
}

TTAint32 CALLBACK TtaDecoder::read_callback(TTA_io_callback *io, TTAuint8 *buffer, TTAuint32 size) 
{
	TTA_io_callback_wrapper *iocb = (TTA_io_callback_wrapper *)io; 
	TTAint32 result;
	if (tta_read(iocb->handle, buffer, size, result))
		return result;
	return 0;
} // read_callback

TTAint64 CALLBACK TtaDecoder::seek_callback(TTA_io_callback *io, TTAint64 offset) 
{
	TTA_io_callback_wrapper *iocb = (TTA_io_callback_wrapper *)io; 
	return tta_seek(iocb->handle, offset);
} // seek_callback