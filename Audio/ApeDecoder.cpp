#include "ApeDecoder.h"


ApeDecoder::ApeDecoder(void)
{
	m_total_blocks = 0;
	m_current_blocks = 0;
	m_pAPEDecompress = NULL;
	m_block_algin = 0;
}


ApeDecoder::~ApeDecoder(void)
{
	Close();
}

HRESULT ApeDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	//char ex[2048] = "ape";
	//if(!isFile_ex(strFileName,ex))
	//{
	//	return M_FILE_EXTENSION_NOT_EXPECTED;//E_FAIL;
	//}
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}
	int nRetVal = 0;
	m_pAPEDecompress = CreateIAPEDecompress(strFileName,&nRetVal);
	if(NULL == m_pAPEDecompress)
	{
		return M_APE_FAIL_CREATE_DECODER;//E_FAIL;
	}
	m_block_algin = m_pAPEDecompress->GetInfo(APE_INFO_BLOCK_ALIGN);
	m_nWaveDataAllocSize = APE_BLOCKS_NUM * m_block_algin;
	m_pWaveDataAlloc = (BYTE*) malloc(m_nWaveDataAllocSize);
	m_pWaveDataBuffer = m_pWaveDataAlloc;
	m_total_blocks = m_pAPEDecompress->GetInfo(APE_DECOMPRESS_TOTAL_BLOCKS);
	m_id3.bitrate = m_pAPEDecompress->GetInfo(APE_DECOMPRESS_AVERAGE_BITRATE);
	// get and set wave header
	nRetVal = m_pAPEDecompress->GetInfo(APE_INFO_WAVEFORMATEX,(int) m_pwfx,0);
	if(0 != nRetVal)
	{
		return M_APE_FAIL_GET_WAVE_INFO;//E_FAIL;
	}
	m_dwSize = m_pAPEDecompress->GetInfo(APE_INFO_WAV_DATA_BYTES);
	if(m_dwSize <= 0)
	{
		return M_APE_FAIL_GET_INFO;//E_FAIL;
	}
	m_id3.duration_times = (float)m_dwSize / (float)m_pwfx->nAvgBytesPerSec; 

	m_id3.EmptyTags();
	CAPETag * pAPETag = (CAPETag *) m_pAPEDecompress->GetInfo(APE_INFO_TAG);
	BOOL bHasID3Tag = pAPETag->GetHasID3Tag();
	BOOL bHasAPETag = pAPETag->GetHasAPETag();

	if (bHasID3Tag || bHasAPETag)
	{
		// iterate through all the tag fields
		
		CAPETagField * pTagField = pAPETag->GetTagField(APE_TAG_FIELD_TITLE);
		if(pTagField)
		{
			/*printf("%X\n",pTagField->GetFieldValue()[0]);
			printf("%X\n",pTagField->GetFieldValue()[1]);
			printf("%X\n",pTagField->GetFieldValue()[2]);*/
			if(pTagField->GetIsUTF8Text())
			{
				CCharWCharHelper cwh;
				cwh.ZMultiByteToWideChar(pTagField->GetFieldValue(),ZUTF8);
				m_id3.title.SetData(cwh.GetWideChar(),cwh.GetWideCharLength());
			}
			else
				m_id3.title.SetData(pTagField->GetFieldValue(),strlen(pTagField->GetFieldValue()));
		}


		pTagField = pAPETag->GetTagField(APE_TAG_FIELD_ARTIST);
		if(pTagField)
		{
			if(pTagField->GetIsUTF8Text())
			{
				CCharWCharHelper cwh;
				cwh.ZMultiByteToWideChar(pTagField->GetFieldValue(),ZUTF8);
				m_id3.artist.SetData(cwh.GetWideChar(),cwh.GetWideCharLength());
			}
			else
				m_id3.artist.SetData(pTagField->GetFieldValue(),strlen(pTagField->GetFieldValue()));
		}


		pTagField = pAPETag->GetTagField(APE_TAG_FIELD_ALBUM);
		if(pTagField)
		{
			if(pTagField->GetIsUTF8Text())
			{
				CCharWCharHelper cwh;
				cwh.ZMultiByteToWideChar(pTagField->GetFieldValue(),ZUTF8);
				m_id3.album.SetData(cwh.GetWideChar(),cwh.GetWideCharLength());
			}
			else
				m_id3.album.SetData(pTagField->GetFieldValue(),strlen(pTagField->GetFieldValue()));
		}


		pTagField = pAPETag->GetTagField(APE_TAG_FIELD_YEAR);
		if(pTagField)
		{
			if(pTagField->GetIsUTF8Text())
			{
				CCharWCharHelper cwh;
				cwh.ZMultiByteToWideChar(pTagField->GetFieldValue(),ZUTF8);
				m_id3.year.SetData(cwh.GetWideChar(),cwh.GetWideCharLength());
			}
			else
				m_id3.year.SetData(pTagField->GetFieldValue(),strlen(pTagField->GetFieldValue()));
		}


		pTagField = pAPETag->GetTagField(APE_TAG_FIELD_COMMENT);
		if(pTagField)
		{
			if(pTagField->GetIsUTF8Text())
			{
				CCharWCharHelper cwh;
				cwh.ZMultiByteToWideChar(pTagField->GetFieldValue(),ZUTF8);
				m_id3.comment.SetData(cwh.GetWideChar(),cwh.GetWideCharLength());
			}
			else
				m_id3.comment.SetData(pTagField->GetFieldValue(),strlen(pTagField->GetFieldValue()));
		}


		pTagField = pAPETag->GetTagField(APE_TAG_FIELD_GENRE);
		if(pTagField)
		{
			if(pTagField->GetIsUTF8Text())
			{
				CCharWCharHelper cwh;
				cwh.ZMultiByteToWideChar(pTagField->GetFieldValue(),ZUTF8);
				m_id3.genre.SetData(cwh.GetWideChar(),cwh.GetWideCharLength());
			}
			else
				m_id3.genre.SetData(pTagField->GetFieldValue(),strlen(pTagField->GetFieldValue()));
		}

		
	}
	return S_OK;
}
HRESULT ApeDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	BYTE *buf = pBuffer;
	unsigned int samplesNeed = dwSizeToRead;
	*pdwSizeRead = 0;
	int nRetVal = 0;
	
	while(samplesNeed)
	{
		if(m_nWaveDataNum)// we have some data
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
		else // decode data
		{
			assert(0 == m_nWaveDataNum);
			if(m_current_blocks < m_total_blocks)
			{
				int nBlocksRetrieved;// 1024 * blockalign
				nRetVal = m_pAPEDecompress->GetData((char*)m_pWaveDataAlloc, APE_BLOCKS_NUM, &nBlocksRetrieved);
				m_current_blocks += nBlocksRetrieved;
				if (ERROR_SUCCESS != nRetVal)
				{
					return M_APE_FAIL_DECODE_DATA;//E_FAIL;
				}
				m_pWaveDataBuffer = m_pWaveDataAlloc;
				m_nWaveDataNum = nBlocksRetrieved * m_block_algin;
			}
			else
			{
				return E_FAIL;
			}
		}
	}
	//printf("read from ape\n");
	return S_OK;
}
HRESULT ApeDecoder::ResetFile() 
{
	m_current_blocks = 0;
	m_nWaveDataNum = 0;
	m_pAPEDecompress->Seek(0);
	//printf("reset from ape\n");
	return S_OK;
}
HRESULT ApeDecoder::Close() 
{
	SAFE_DELETE_ARRAY(m_pAPEDecompress);
	m_total_blocks = 0;
	m_current_blocks = 0;
	m_pAPEDecompress = NULL;
	m_block_algin = 0;
	return S_OK;
}
HRESULT ApeDecoder::Seek(float duration ,DWORD *newPosition ) 
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
		*newPosition = seekSize;
		unsigned int mcBlock = seekSize / m_block_algin; 
		if(mcBlock > m_total_blocks)
		{
			return E_FAIL;
		}
		int num = m_pAPEDecompress->Seek(mcBlock);
		m_current_blocks = mcBlock;
		//return (0 == num);
		
	}
	return S_OK;;
}