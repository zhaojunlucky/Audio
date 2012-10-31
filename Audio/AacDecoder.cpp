#include <math.h>

#include "AacDecoder.h"
#include "AacInfo.h"

AacDecoder::AacDecoder(void)
{
	infile = NULL;
	first_time = 1;
	buffercount = 0;
	buffer_index = 0; 
	bytes_in_buffer = 0;
	frames = 0;
	m_nSamplesPerFrame = 0;
	m_seek = FALSE;
	m_seekLeftBytes = 0;
	m_seek_table = NULL;
	seek_table_len = 0;
}


AacDecoder::~AacDecoder(void)
{
	Close();
}

HRESULT AacDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	LoadID3(strFileName);
	char ex[512] = "aac";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}

	CCharWCharHelper cwz(strFileName);
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	faadAACInfo info;
	
	strcpy_s(ex,512,cwz.GetMultiChar());
	ex[cwz.GetMultiByteLength()]='\0';
	if( -1 == get_AAC_format(ex,&info,&m_seek_table,&seek_table_len,0))
	{
		return M_AAC_FAIL_GET_INFO;//E_FAIL;
	}
	m_id3.duration_times = (float)info.length / 1000.0f;
	m_id3.bitrate = info.bitrate;
	fopen_s(&infile,cwz.GetMultiChar(),"rb");
	if(NULL == infile)
	{
		return M_FAIL_OPEN_FILE	;//E_FAIL;
	}

	/* declare variables for buffering */
    //DEC_BUFF_VARS
	INIT_BUFF(infile) // seek to 6144
	
	int tagsize = id3v2_tag(buffer);

	if (tagsize)
    {
        UPDATE_BUFF_SKIP(tagsize)
    }
	//long p = ftell(infile);
	hDecoder = NeAACDecOpen();

	config = NeAACDecGetCurrentConfiguration(hDecoder);
	config->defObjectType = 0;
	config->outputFormat = 1;
	unsigned long samplerate;
    unsigned char channels;
	NeAACDecSetConfiguration(hDecoder, config);
	if ((bytesconsumed = NeAACDecInit(hDecoder, buffer, bytes_in_buffer,
        &samplerate, &channels)) < 0)
    {
        /* If some error initializing occured, skip the file */
        return E_FAIL;
    }
    buffer_index += bytesconsumed;
	//p = ftell(infile);
	// decode one frame to get audio info
	/* update buffer */
    UPDATE_BUFF_READ
	m_pWaveDataAlloc = (BYTE*)NeAACDecDecode(hDecoder, &frameInfo, buffer, bytes_in_buffer);
	/* update buffer indices */
	UPDATE_BUFF_IDX(frameInfo)

	if (frameInfo.error > 0)
	{
		return M_AAC_FAIL_DECODE_DATA;//E_FAIL;
	}

	m_nWaveDataAllocSize = m_nWaveDataNum = frameInfo.channels*frameInfo.samples;
	m_pWaveDataBuffer = m_pWaveDataAlloc;

	switch(config->outputFormat)
	{
	case FAAD_FMT_24BIT:
		m_pwfx->wBitsPerSample = 24;
		break;
	case FAAD_FMT_32BIT:
		m_pwfx->wBitsPerSample = 32;
		break;
	case FAAD_FMT_16BIT:
	default:
		m_pwfx->wBitsPerSample = 16;
		break;
	}

	m_pwfx->nChannels = channels;
	m_pwfx->nSamplesPerSec = samplerate;
	m_pwfx->wFormatTag = 1;
	m_pwfx->cbSize = 0;
	m_pwfx->nBlockAlign = m_pwfx->nChannels * m_pwfx->wBitsPerSample / 8;
	m_pwfx->nAvgBytesPerSec = m_pwfx->nBlockAlign * m_pwfx->nSamplesPerSec;

	m_nSamplesPerFrame = frameInfo.samples / frameInfo.channels;
	switch(frameInfo.sbr)
	{
		default:
		case NO_SBR:
			m_nSamplesPerFrame = 1024;
		break;

		case SBR_UPSAMPLED:
			m_nSamplesPerFrame = 2048;
			//c_nDecoderDelay = SBR_DECODER_DELAY;
			//c_nFrameOverlaySet = FRAME_OVERLAY_SBR_NUM;
		break;

		case SBR_DOWNSAMPLED:
			m_nSamplesPerFrame = 512;
			//c_nDecoderDelay = SBR_DECODER_DELAY;
			//c_nFrameOverlaySet = FRAME_OVERLAY_SBR_NUM;
		break;

		case NO_SBR_UPSAMPLED:
			m_nSamplesPerFrame = 1024;
		break;

	}
	m_dwSize = (DWORD)(m_pwfx->nSamplesPerSec * m_pwfx->wBitsPerSample * m_pwfx->nChannels * m_id3.duration_times / 8.0f);
	//p = ftell(infile);
	return S_OK;
}

HRESULT AacDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
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
			if (buffer_index >= fileread)
			{
				return E_FAIL;
			}
			 /* update buffer */
			//long pos = ftell(infile);
			UPDATE_BUFF_READ
			//pos = ftell(infile);
			m_pWaveDataAlloc = (BYTE *)NeAACDecDecode(hDecoder, &frameInfo, buffer, bytes_in_buffer);

			/* update buffer indices */
			UPDATE_BUFF_IDX(frameInfo)

			if (frameInfo.error > 0)
			{
				printf("Error: %s\n",
				NeAACDecGetErrorMessage(frameInfo.error));
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
					m_nWaveDataAllocSize = (unsigned int)(frameInfo.channels*frameInfo.samples);
					if((unsigned int)m_seekLeftBytes < m_nWaveDataAllocSize)
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
		}
	}
	return S_OK;
}
HRESULT AacDecoder::ResetFile() 
{
	//m_nWaveDataAllocSize = m_nWaveDataNum = 0;
	fseek(infile,0,SEEK_SET);
	buffer_index = 0;
	unsigned long samplerate;
    unsigned char channels;
	int tagsize = id3v2_tag(buffer);

	if (tagsize)
    {
        UPDATE_BUFF_SKIP(tagsize)
    }
	bytes_in_buffer = bytesconsumed = fread(buffer, 1,FAAD_MIN_STREAMSIZE*MAX_CHANNELS, infile);
	m_seek = FALSE;
	m_seekLeftBytes = 0;

	NeAACDecSetConfiguration(hDecoder, config);
	if ((bytesconsumed = NeAACDecInit(hDecoder, buffer, bytes_in_buffer,
        &samplerate, &channels)) < 0)
    {
        /* If some error initializing occured, skip the file */
        return E_FAIL;
    }
    buffer_index += bytesconsumed;
	//p = ftell(infile);
	// decode one frame to get audio info
	/* update buffer */
    UPDATE_BUFF_READ
	m_pWaveDataAlloc = (BYTE*)NeAACDecDecode(hDecoder, &frameInfo, buffer, bytes_in_buffer);
	m_nWaveDataAllocSize = m_nWaveDataNum = frameInfo.channels*frameInfo.samples;
	m_pWaveDataBuffer = m_pWaveDataAlloc;
	/* update buffer indices */
	UPDATE_BUFF_IDX(frameInfo)
	return S_OK;
}
HRESULT AacDecoder::Close() 
{
	if(NULL != infile)
	{
		fclose(infile);
		infile = NULL;
	}
	if(NULL != m_seek_table)
	{
		free(m_seek_table);
		m_seek_table = NULL;
	}
	if(NULL != hDecoder)
	{
		NeAACDecClose(hDecoder);
		hDecoder = NULL;
	
		END_BUFF
	}
	m_pWaveDataAlloc = NULL;
	return S_OK;
}
HRESULT AacDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		//unsigned int samples = ((float)m_pwfx->nSamplesPerSec * (float)duration);
		//float frame = (float)samples / (float)m_nSamplesPerFrame;
		//float maxFrame = ceil(frame);
		//NeAACDecPostSeekReset(hDecoder,maxFrame);
		//UPDATE_BUFF_READ
		//UPDATE_BUFF_SKIP(651729)
		//if(maxFrame > frame)
		//{
		//	//m_seek = TRUE;
		//	int leftSamples = ((maxFrame - frame) *
		//		(float)m_nSamplesPerFrame);
		//	m_seekLeftBytes = leftSamples * m_pwfx->nChannels;
		//}
		m_nWaveDataAllocSize = m_nWaveDataNum = 0;
		float maxDuration = ceil(duration);
		if(maxDuration > duration)
		{
			m_seek = TRUE;
			m_seekLeftBytes = (int)((maxDuration - duration) * (float)m_pwfx->nAvgBytesPerSec);
		}
		long file_seek = m_seek_table[(int)maxDuration];//m_pFrame[(int)maxFrame];

		fseek(infile,file_seek,SEEK_SET);
		buffer_index = file_seek;

		bytes_in_buffer = fread(buffer, 1,FAAD_MIN_STREAMSIZE*MAX_CHANNELS, infile);
		bytesconsumed = 0;
	}
	return S_OK;
}

int AacDecoder::id3v2_tag(unsigned char *buffer)
{
    if (strncmp((char *)buffer, "ID3", 3) == 0) {
        unsigned long tagsize;

        /* high bit is not used */
        tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
            (buffer[8] <<  7) | (buffer[9] <<  0);

        tagsize += 10;

        return tagsize;
    } else {
        return 0;
    }
}

//int AacDecoder::read_ADTS_header(faadAACInfo *info, int *framesNum)
//{
// /* Get ADTS header data */
//    unsigned char buffer[ADTS_MAX_SIZE];
//    int frames, t_framelength = 0, frame_length, sr_idx = 0, ID;
//    int second = 0, pos;
//    float frames_per_sec = 0;
//    unsigned long bytes;
//    unsigned long *tmp_seek_table = NULL;
//
//    info->headertype = 2;
//
//    /* Read all frames to ensure correct time and bitrate */
//    for (frames = 0; /* */; frames++)
//    {
//		long poss = ftell(infile);
//        bytes = fread(buffer, 1, ADTS_MAX_SIZE, infile);
//
//        if (bytes != ADTS_MAX_SIZE)
//            break;
//
//        /* check syncword */
//        if (!((buffer[0] == 0xFF)&&((buffer[1] & 0xF6) == 0xF0)))
//            break;
//
//
//        if (!frames)
//        {
//            /* fixed ADTS header is the same for every frame, so we read it only once */
//            /* Syncword found, proceed to read in the fixed ADTS header */
//            ID = buffer[1] & 0x08;
//            info->object_type = (buffer[2]&0xC0)>>6;
//            sr_idx = (buffer[2]&0x3C)>>2;
//            info->channels = ((buffer[2]&0x01)<<2)|((buffer[3]&0xC0)>>6);
//
//            frames_per_sec = sample_rates[sr_idx] / 1024.f;
//        }
//
//        /* ...and the variable ADTS header */
//        if (ID == 0)
//        {
//            info->version = 4;
//        } else { /* MPEG-2 */
//            info->version = 2;
//        }
//        frame_length = ((((unsigned int)buffer[3] & 0x3)) << 11)
//            | (((unsigned int)buffer[4]) << 3) | (buffer[5] >> 5);
//
//        t_framelength += frame_length;
//
//        pos = ftell(infile) - ADTS_MAX_SIZE;
//
//        fseek(infile, frame_length - ADTS_MAX_SIZE, SEEK_CUR);
//
//		if(frames >= m_npFrameSize)
//		{
//			m_pFrame = reallocate_aac_ptr(m_pFrame,m_npFrameSize,FRAME_INCRE_SIZE + m_npFrameSize);
//			if(NULL == m_pFrame)
//			{
//				return E_FAIL;
//			}
//			m_npFrameSize += FRAME_INCRE_SIZE;
//		}
//		m_pFrame[frames] = poss;
//    }
//	*framesNum = frames;
//    if (frames > 0)
//    {
//        float sec_per_frame, bytes_per_frame;
//        info->sampling_rate = sample_rates[sr_idx];
//        sec_per_frame = (float)info->sampling_rate/1024.0;
//        bytes_per_frame = (float)t_framelength / (float)frames;
//        info->bitrate = 8 * (int)floor(bytes_per_frame * sec_per_frame);
//        info->length = (int)floor((float)frames/frames_per_sec)*1000;
//    } else {
//        info->sampling_rate = 4;
//        info->bitrate = 128000;
//        info->length = 0;
//        info->channels = 0;
//		return -1;
//    }
//    return 0;
//}
//int AacDecoder::get_AAC_format(faadAACInfo *info, int *framesNum)
//{
//	 unsigned long tagsize;
//   // FILE *file;
//    char buffer[10];
//    unsigned long file_len;
//    unsigned char adxx_id[5];
//    unsigned long tmp;
//	*framesNum = 0;
//    memset(info, 0, sizeof(faadAACInfo));
//
//    //file = fopen(filename, "rb");
//
//    //if(file == NULL)
//    //    return -1;
//
//    fseek(infile, 0, SEEK_END);
//    file_len = ftell(infile);
//    fseek(infile, 0, SEEK_SET);
//
//    /* Skip the tag, if it's there */
//    tmp = fread(buffer, 10, 1, infile);
//
//    if (StringComp(buffer, "ID3", 3) == 0)
//    {
//        /* high bit is not used */
//        tagsize = (buffer[6] << 21) | (buffer[7] << 14) |
//            (buffer[8] <<  7) | (buffer[9] <<  0);
//
//        fseek(infile, tagsize, SEEK_CUR);
//        tagsize += 10;
//    } else {
//        tagsize = 0;
//        fseek(infile, 0, SEEK_SET);
//    }
//
//    if (file_len)
//        file_len -= tagsize;
//
//    tmp = fread(adxx_id, 2, 1, infile);
//    adxx_id[5-1] = 0;
//    info->length = 0;
//
//    /* Determine the header type of the file, check the first two bytes */
//    if (StringComp((char *)adxx_id, "AD", 2) == 0)
//    {
//        /* We think its an ADIF header, but check the rest just to make sure */
//        tmp = fread(adxx_id + 2, 2, 1, infile);
//
//        if (StringComp((char *)adxx_id, "ADIF", 4) == 0)
//        {
//            //read_ADIF_header(file, info);
//
//            //info->length = (int)((float)file_len*8000.0/((float)info->bitrate));
//			return -1;
//        }
//    } else {
//        /* No ADIF, check for ADTS header */
//        if ((adxx_id[0] == 0xFF)&&((adxx_id[1] & 0xF6) == 0xF0))
//        {
//            /* ADTS  header located */
//            fseek(infile, tagsize, SEEK_SET);
//            read_ADTS_header( info, framesNum);
//        } else {
//            /* Unknown/headerless AAC file, assume format: */
//            info->version = 2;
//            info->bitrate = 128000;
//            info->sampling_rate = 44100;
//            info->channels = 2;
//            info->headertype = 0;
//            info->object_type = 1;
//			return -1;
//        }
//    }
//    return 0;
//}
