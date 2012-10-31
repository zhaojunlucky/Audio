#include "FlacDecoder.h"
#include "..\TagLibrary\ImageProcess.h"

FlacDecoder::FlacDecoder(void)
{
	decoder = NULL;
}


FlacDecoder::~FlacDecoder(void)
{
	Close();
}

HRESULT FlacDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	//char ex[2048] = "flac";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}

	CCharWCharHelper cwz(strFileName);
	FILE *file = 0;
	fopen_s(&file,cwz.GetMultiChar(),"r");
	long len = 0; 
	if(file)
	{
		fseek(file,0,SEEK_END);
		len = ftell(file);
		fclose(file);
	}
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	if( NULL == (decoder = FLAC__stream_decoder_new()))
	{
		return M_FLAC_FAIL_CREATE_DECODER;//E_FAIL;// ??
	}
	(void)FLAC__stream_decoder_set_md5_checking(decoder, true);
	init_status = FLAC__stream_decoder_init_file(decoder, cwz.GetMultiChar(), write_callback, metadata_callback, error_callback, /*client_data=*/this);
	if(FLAC__STREAM_DECODER_INIT_STATUS_OK != init_status)
	{
		return M_FLAC_FAIL_INIT_FILE;//E_FAIL;
	}
	m_id3.EmptyTags();
	metaDataSize = 0;
	// process metadata

	FLAC__bool ok = FLAC__stream_decoder_process_until_end_of_metadata(decoder);

	if( !ok )
	{
		return M_FLAC_FAIL_DECODE_DATA;//E_FAIL;
	}

	metaDataSize = len - metaDataSize;
	if(metaDataSize >= 0 )		
	{
		m_id3.bitrate = 8.0 * metaDataSize / m_id3.duration_times / 1000.0;
	}
	return S_OK;
}
HRESULT FlacDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
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
			FLAC__bool ok = FLAC__stream_decoder_process_single(decoder);
			if(!ok)
			{
				return M_FLAC_FAIL_DECODE_DATA;//E_FAIL;
			}
			if(FLAC__STREAM_DECODER_END_OF_STREAM == FLAC__stream_decoder_get_state(decoder))
				break;
		}
	}
	return S_OK;
}
HRESULT FlacDecoder::ResetFile() 
{
	m_nWaveDataNum = 0;
	if(false == FLAC__stream_decoder_seek_absolute(decoder, 0))
	{
		return M_SEEK_FAIL;//E_FAIL;
	}

	if(FLAC__STREAM_DECODER_SEEK_ERROR == FLAC__stream_decoder_get_state(decoder))
	{
		if(false == FLAC__stream_decoder_flush(decoder))
		{
			return M_FLAC_FAIL_DECODER_FLUSH;//E_FAIL;
		}

		if(false == FLAC__stream_decoder_reset(decoder))
		{
			return M_FLAC_FAIL_DECODER_RESET;//E_FAIL;
		}
	}
	return S_OK;
}
HRESULT FlacDecoder::Close() 
{
	if( NULL != decoder )
	{
		FLAC__stream_decoder_delete(decoder);
	}
	decoder = NULL;
	return S_OK;
}
HRESULT FlacDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		//if(0 == samples)
		//{
		//	samples = 1;
		//}
		m_nWaveDataNum = 0;
		if(false == FLAC__stream_decoder_seek_absolute(decoder, samples))
		{
			return M_SEEK_FAIL;//E_FAIL;
		}

		if(FLAC__STREAM_DECODER_SEEK_ERROR == FLAC__stream_decoder_get_state(decoder))
		{
			if(false == FLAC__stream_decoder_flush(decoder))
			{
				return M_FLAC_FAIL_DECODER_FLUSH;//E_FAIL;
			}

			if(false == FLAC__stream_decoder_reset(decoder))
			{
				return M_FLAC_FAIL_DECODER_RESET;//E_FAIL;
			}
		}
	}
	
	return S_OK;
}

void FlacDecoder::error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{

}

void FlacDecoder::metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	FlacDecoder *flac = (FlacDecoder*) client_data;
	flac->metaDataSize += metadata->length;
	if(FLAC__METADATA_TYPE_STREAMINFO == metadata->type)
	{
		flac->m_dwSize = (DWORD)(metadata->data.stream_info.total_samples * metadata->data.stream_info.channels
			* (metadata->data.stream_info.bits_per_sample / 8 ));

		flac->m_id3.duration_times = (float)metadata->data.stream_info.total_samples / (float)metadata->data.stream_info.sample_rate;

		flac->m_pwfx->nChannels = metadata->data.stream_info.channels;
		flac->m_pwfx->nBlockAlign = metadata->data.stream_info.channels * (metadata->data.stream_info.bits_per_sample / 8);
		flac->m_pwfx->wFormatTag = 1;
		flac->m_pwfx->nSamplesPerSec = metadata->data.stream_info.sample_rate;
		flac->m_pwfx->wBitsPerSample = metadata->data.stream_info.bits_per_sample;
		flac->m_pwfx->nAvgBytesPerSec = flac->m_pwfx->nBlockAlign * metadata->data.stream_info.sample_rate;
		flac->m_pwfx->cbSize = 0;

	}
	else if(FLAC__METADATA_TYPE_VORBIS_COMMENT == metadata->type)
	{
		int entrynr = FLAC__metadata_object_vorbiscomment_find_entry_from(metadata,0,"TITLE");
		if(-1 != entrynr)
		{
			FLAC__StreamMetadata_VorbisComment_Entry  & e = metadata->data.vorbis_comment.comments[entrynr];
			CZString str;
			str.SetData(((char*)e.entry),e.length);
			int index = str.IndexOf(L'=');
			flac->m_id3.title.SetData(str.SubString(index+1));
		}
		else
			flac->m_id3.title.Empty();

		entrynr = FLAC__metadata_object_vorbiscomment_find_entry_from(metadata,0,"ARTIST");
		if(-1 != entrynr)
		{
			FLAC__StreamMetadata_VorbisComment_Entry  & e = metadata->data.vorbis_comment.comments[entrynr];
			CZString str;
			str.SetData((char*)e.entry,e.length);
			int index = str.IndexOf(L'=');
			flac->m_id3.artist.SetData(str.SubString(index+1));
		}
		else
			flac->m_id3.artist.Empty();


		entrynr = FLAC__metadata_object_vorbiscomment_find_entry_from(metadata,0,"ALBUM");
		if(-1 != entrynr)
		{
			FLAC__StreamMetadata_VorbisComment_Entry  & e = metadata->data.vorbis_comment.comments[entrynr];
			CZString str;
			str.SetData((char*)e.entry,e.length);
			int index = str.IndexOf(L'=');
			flac->m_id3.album.SetData(str.SubString(index+1));
		}
		else
			flac->m_id3.album.Empty();


		entrynr = FLAC__metadata_object_vorbiscomment_find_entry_from(metadata,0,"GENRE");
		if(-1 != entrynr)
		{
			FLAC__StreamMetadata_VorbisComment_Entry  & e = metadata->data.vorbis_comment.comments[entrynr];
			CZString str;
			str.SetData((char*)e.entry,e.length);
			int index = str.IndexOf(L'=');
			flac->m_id3.genre.SetData(str.SubString(index+1));
		}
		else
			flac->m_id3.genre.Empty();

		flac->m_id3.year.Empty();
		flac->m_id3.comment.Empty();
	}
	else if(FLAC__METADATA_TYPE_PICTURE == metadata->type )
	{
		const FLAC__StreamMetadata_Picture *pic = &metadata->data.picture;
		if(FLAC__STREAM_METADATA_PICTURE_TYPE_MEDIA == pic->type || FLAC__STREAM_METADATA_PICTURE_TYPE_LEAD_ARTIST == pic->type
			||FLAC__STREAM_METADATA_PICTURE_TYPE_ARTIST == pic->type)
		{
			if(0 ==  _stricmp(pic->mime_type,"image/png") || 0 ==  _stricmp(pic->mime_type,"image/jpeg")
				|| 0 ==  _stricmp(pic->mime_type,"image/bmp")|| 0 ==  _stricmp(pic->mime_type,"image/jpg"))
			{
				flac->m_id3.id3Bitmap->height = pic->height;
				flac->m_id3.id3Bitmap->width = pic->width;
				unsigned int w;
				unsigned int h;
				flac->m_id3.id3Bitmap->hbm = DecodePicture(pic->mime_type,(char*)pic->data,pic->data_length,&w,&h);
			}
		}
		
	}
}

FLAC__StreamDecoderWriteStatus FlacDecoder::write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{

	FlacDecoder *flac = (FlacDecoder*)client_data;
	assert(0 == flac->m_nWaveDataNum);
	if(flac->m_pwfx->nSamplesPerSec != frame->header.sample_rate)
	{
		return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;//
	}
	if(flac->m_pwfx->wBitsPerSample != frame->header.bits_per_sample)
	{
		return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;//
	}
	if(flac->m_pwfx->nChannels != frame->header.channels)
	{
		return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE; // skip this frame
	}
	unsigned int frameBytes = frame->header.blocksize * 4;// always use 16 bit

	if(frameBytes > flac->m_nWaveDataAllocSize)
	{
		if(flac->m_pWaveDataAlloc)
		{
			delete []flac->m_pWaveDataAlloc;
		}
		flac->m_pWaveDataAlloc = new BYTE[frameBytes];
		if(0 == flac->m_pWaveDataAlloc)
		{
			return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
		}
		flac->m_nWaveDataAllocSize = frameBytes;
	}

	//detect if stereo or mono
	unsigned int right = 0;
	switch(frame->header.channels)
	{
		case 1: //mono 
			right = 0;
			break;

		case 2: //stereo
			right = 1;
			break;
		default:
			return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	// set data
	/*switch(frame->header.bits_per_sample)
	{
	case 24:
		{
			unsigned int i;
			unsigned int j;
			BYTE *buf = (BYTE*)flac->m_pSamplesAlloc;
			for(i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
			{
				buf[j] = buffer[0][i] >> 8;
				buf[j + 1] = buffer[right][i] >> 8;
			}
		}
		break;

	case 16:
		{
			unsigned int i;
			unsigned int j = 0;
			BYTE *buf = (BYTE*)flac->m_pSamplesAlloc;
			//for(i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
			//{
			//	buf[j] = buffer[0][i];
			//	buf[j + 1] = buffer[right][i];
			//}
			for(i = 0; i < frame->header.blocksize, j < frameBytes; i++)
			{
				buf[j++] = buffer[0][i];
				buf[j++] = buffer[0][i] >> 8;
				buf[j++] = buffer[1][i];
				buf[j++] = buffer[1][i] >> 8;
			}
		}
		break;

	case 8:
		{
			unsigned int i;
			unsigned int j;
			BYTE *buf = (BYTE*)flac->m_pSamplesAlloc;
			for(i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
			{
				buf[j] = buffer[0][i] << 8;
				buf[j + 1] = buffer[right][i] << 8;
			}
		}
		break;

	default:
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}*/

		unsigned int i;
		unsigned int j = 0;
		BYTE *buf = (BYTE*)flac->m_pWaveDataAlloc;
		//for(i = 0, j = 0; i < frame->header.blocksize; i++, j += 2)
		//{
		//	buf[j] = buffer[0][i];
		//	buf[j + 1] = buffer[right][i];
		//}
		for(i = 0; i < frame->header.blocksize, j < frameBytes; i++)
		{
			buf[j++] = buffer[0][i];
			buf[j++] = buffer[0][i] >> 8;
			buf[j++] = buffer[1][i];
			buf[j++] = buffer[1][i] >> 8;
		}

	flac->m_pWaveDataBuffer = flac->m_pWaveDataAlloc;
	flac->m_nWaveDataNum = frameBytes;//frame->header.blocksize * 4;
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}
