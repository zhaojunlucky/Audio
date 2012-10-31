#include "Mp3Decoder.h"

#include <string.h>
using namespace std;

Mp3Decoder::Mp3Decoder(void)
{
}


Mp3Decoder::~Mp3Decoder(void)
{
	Close();
}

HRESULT Mp3Decoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	// first scan id3

	LoadID3(strFileName);
	//char ex[2048] = "mp3";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}
	CCharWCharHelper cwz(strFileName);
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	if(mpg123_init() != MPG123_OK)
		return M_MP3_FAIL_CREATE_DECODER;//E_FAIL;
	int ret = -1;
	_handle = mpg123_new(NULL, &ret);
	if(_handle == NULL || ret != MPG123_OK)
		return M_MP3_FAIL_CREATE_DECODER;//E_FAIL;

	if(mpg123_open(_handle, cwz.GetMultiChar()) != MPG123_OK)
		return M_FAIL_OPEN_FILE;//E_FAIL;
	struct mpg123_frameinfo i;
	
	mpg123_scan(_handle);
	mpg123_info(_handle, &i);
	switch(i.vbr)
	{
	case MPG123_CBR: m_id3.bitrate = i.bitrate; break;
	case MPG123_VBR: m_id3.bitrate = 0; break;
	case MPG123_ABR: m_id3.bitrate = i.bitrate; break;
	default: break;
	}


	// set the wave header
	long rate = 0;
	int channel = 0;
	int encoding = 0;

	if(mpg123_getformat(_handle, &rate, &channel, &encoding) != MPG123_OK)
		return M_MP3_FAIL_GET_WAVE_INFO;//-1;
	if((encoding & MPG123_ENC_16) == MPG123_ENC_16)
		encoding = 16;
	else if((encoding & MPG123_ENC_32) == MPG123_ENC_32)
		encoding = 32;
	else
		encoding = 8;
	
	m_pwfx->wFormatTag = WAVE_FORMAT_PCM;
	m_pwfx->nChannels = channel;
	m_pwfx->nSamplesPerSec = rate;
	m_pwfx->wBitsPerSample = encoding;
	m_pwfx->nBlockAlign = (encoding / 8) * channel;
	m_pwfx->nAvgBytesPerSec = rate * (encoding / 8) * channel;
	m_pwfx->cbSize = 0;

	m_dwSize = mpg123_length(_handle) * (m_pwfx->nBlockAlign);
	m_id3.duration_times = (float)m_dwSize / (float) m_pwfx->nAvgBytesPerSec;

	
	return S_OK;
}
HRESULT Mp3Decoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	size_t outsize = 0;
	int ret = mpg123_read(_handle, pBuffer, dwSizeToRead, &outsize);
	*pdwSizeRead = 0;
	if(ret != MPG123_OK)
		return M_MP3_FAIL_DECODE_DATA;//E_FAIL;
	*pdwSizeRead = outsize;
	return S_OK;
}
HRESULT Mp3Decoder::ResetFile() 
{
	if(mpg123_seek(_handle,0, SEEK_SET) >= 0)
	{
		return S_OK;
	}
	return E_FAIL;
}
HRESULT Mp3Decoder::Close() 
{
	if(_handle != NULL)
	{
		mpg123_close(_handle);
		mpg123_delete(_handle);
		mpg123_exit();
		_handle = NULL;
	}
	return S_OK;
}
HRESULT Mp3Decoder::Seek(float duration ,DWORD *newPosition )
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
		unsigned int samples = (unsigned int)((float)m_pwfx->nSamplesPerSec * (float)duration);
		if(mpg123_seek(_handle,samples, SEEK_SET) >= 0)
		{
			return S_OK;
		}
	}
	return M_SEEK_FAIL;//E_FAIL;
}