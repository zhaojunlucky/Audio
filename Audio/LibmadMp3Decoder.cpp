#include "LibmadMp3Decoder.h"


LibmadMp3Decoder::LibmadMp3Decoder(void)
{
	mp3_file.fp = NULL;
}


LibmadMp3Decoder::~LibmadMp3Decoder(void)
{
	Close();
}

HRESULT LibmadMp3Decoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) 
{
	char ex[2048] = "mp3";
	if(!isFile_ex(strFileName,ex))
	{
		return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	}
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	mp3_file.fp = fopen(ex,"rb");
	if(!mp3_file.fp)
		return E_FAIL;
	return S_OK;
}
HRESULT LibmadMp3Decoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) 
{
	return S_OK;
}
HRESULT LibmadMp3Decoder::ResetFile() 
{
	return S_OK;
}
HRESULT LibmadMp3Decoder::Close() 
{
	if(mp3_file.fp)
	{
		fclose(mp3_file.fp);
		mp3_file.fp = NULL;
	}
	return S_OK;
}
HRESULT LibmadMp3Decoder::Seek(float duration ,DWORD *newPosition ) 
{
	return S_OK;
}

enum mad_flow LibmadMp3Decoder::input(void *data,struct mad_stream *stream)
{
	buffer *mp3fp;
	mad_flow      ret_code;
	int      unproc_data_size;    /*the unprocessed data's size*/
	int      copy_size;

	mp3fp = (buffer *)data;
	if(mp3fp->fpos>mp3fp->flen)
	{
		unproc_data_size = stream->bufend - stream->next_frame;
		memcpy(mp3fp->fbuf, mp3fp->fbuf+mp3fp->fbsize-unproc_data_size, unproc_data_size);
		copy_size = BUFSIZE - unproc_data_size;
		if(mp3fp->fpos + copy_size > mp3fp->flen)
		{
			copy_size = mp3fp->flen - mp3fp->fpos;
		}
		fread(mp3fp->fbuf+unproc_data_size, 1, copy_size, mp3fp->fp);
		mp3fp->fbsize = unproc_data_size + copy_size;
		mp3fp->fpos  += copy_size;

		/*Hand off the buffer to the mp3 input stream*/
		mad_stream_buffer(stream, mp3fp->fbuf, mp3fp->fbsize);
		ret_code = MAD_FLOW_CONTINUE;
	}
	else
	{
		ret_code = MAD_FLOW_STOP;
	}

	return ret_code;

}
signed int LibmadMp3Decoder::LibmadMp3Decoder::scale(mad_fixed_t sample)
{
	return 0;
}
enum mad_flow LibmadMp3Decoder::output(void *data,struct mad_header const *header,struct mad_pcm *pcm)
{
		return MAD_FLOW_CONTINUE;
}
enum mad_flow LibmadMp3Decoder::error(void *data,struct mad_stream *stream,struct mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}