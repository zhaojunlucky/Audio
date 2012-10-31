#include "WaveDecoder.h"



WaveDecoder::WaveDecoder(void)
{
	//m_pwfx = NULL;
    m_hmmio = NULL;
    m_pResourceBuffer = NULL;
}


WaveDecoder::~WaveDecoder(void)
{
	Close();
}

HRESULT WaveDecoder::Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags )
{
	m_id3.EmptyTags();
	/*char ex[2048] = "wav";
	if(!isFile_ex(strFileName,ex))
	{
		return E_FAIL;
	}*/
	HRESULT hr;
	if(WAVEFILE_READ != dwFlags)
	{
		return M_INVALID_PARAMETERS;//E_FAIL;
	}

    m_dwFlags = dwFlags;
    //m_bIsReadingFromMemory = FALSE;
	SAFE_DELETE_ARRAY( m_pwfx );
	m_pwfx = NULL;
    if( m_dwFlags == WAVEFILE_READ )
    {
        if( strFileName == NULL )
            return E_INVALIDARG;
        
        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF | MMIO_READ );

        if( NULL == m_hmmio )
        {
            HRSRC hResInfo;
            HGLOBAL hResData;
            DWORD dwSize;
            VOID* pvRes;

            // Loading it as a file failed, so try it as a resource
            if( NULL == ( hResInfo = FindResource( NULL, strFileName, L"WAVE" ) ) )
            {
                if( NULL == ( hResInfo = FindResource( NULL, strFileName, L"WAV" ) ) )
                    //return DXTRACE_ERR( L"FindResource", E_FAIL );
					return E_FAIL;
            }

            if( NULL == ( hResData = LoadResource( GetModuleHandle( NULL ), hResInfo ) ) )
                //return DXTRACE_ERR( L"LoadResource", E_FAIL );
				return E_FAIL;

            if( 0 == ( dwSize = SizeofResource( GetModuleHandle( NULL ), hResInfo ) ) )
                //return DXTRACE_ERR( L"SizeofResource", E_FAIL );
				return E_FAIL;

            if( NULL == ( pvRes = LockResource( hResData ) ) )
                //return DXTRACE_ERR( L"LockResource", E_FAIL );
				return E_FAIL;

            m_pResourceBuffer = new CHAR[ dwSize ];
            if( m_pResourceBuffer == NULL )
                //return DXTRACE_ERR( L"new", E_OUTOFMEMORY );
				return  E_OUTOFMEMORY;
            memcpy( m_pResourceBuffer, pvRes, dwSize );

            MMIOINFO mmioInfo;
            ZeroMemory( &mmioInfo, sizeof( mmioInfo ) );
            mmioInfo.fccIOProc = FOURCC_MEM;
            mmioInfo.cchBuffer = dwSize;
            mmioInfo.pchBuffer = ( CHAR* )m_pResourceBuffer;

            m_hmmio = mmioOpen( NULL, &mmioInfo, MMIO_ALLOCBUF | MMIO_READ );
        }

        if( FAILED( hr = ReadMMIO() ) )
        {
            // ReadMMIO will fail if its an not a wave file
            mmioClose( m_hmmio, 0 );
           // return DXTRACE_ERR( L"ReadMMIO", hr );
			return hr;
        }

        if( FAILED( hr = ResetFile() ) )
            //return DXTRACE_ERR( L"ResetFile", hr );
			return hr;

        // After the reset, the size of the wav file is m_ck.cksize so store it now
        m_dwSize = m_ck.cksize;
		m_id3.duration_times = (float)m_dwSize / (float) m_pwfx->nAvgBytesPerSec;
		m_id3.bitrate = m_pwfx->nChannels * m_pwfx->nSamplesPerSec * m_pwfx->wBitsPerSample;
    }
    else
    {
        m_hmmio = mmioOpen( strFileName, NULL, MMIO_ALLOCBUF |
                            MMIO_READWRITE |
                            MMIO_CREATE );
        if( NULL == m_hmmio )
            //return DXTRACE_ERR( L"mmioOpen", E_FAIL );
			return E_FAIL;

        if( FAILED( hr = WriteMMIO( pwfx ) ) )
        {
            mmioClose( m_hmmio, 0 );
            //return DXTRACE_ERR( L"WriteMMIO", hr );
			return hr;
        }

        if( FAILED( hr = ResetFile() ) )
            //return DXTRACE_ERR( L"ResetFile", hr );
			return hr;
    }

    return hr;
}

HRESULT WaveDecoder::Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead )
{
	MMIOINFO mmioinfoIn; // current status of m_hmmio

    if( m_hmmio == NULL )
        return CO_E_NOTINITIALIZED;
    if( pBuffer == NULL || pdwSizeRead == NULL )
        return E_INVALIDARG;

    if( pdwSizeRead != NULL )
        *pdwSizeRead = 0;

    if( 0 != mmioGetInfo( m_hmmio, &mmioinfoIn, 0 ) )
        return E_FAIL;

    UINT cbDataIn = dwSizeToRead;
    if( cbDataIn > m_ck.cksize )
        cbDataIn = m_ck.cksize;

    m_ck.cksize -= cbDataIn;

    for( DWORD cT = 0; cT < cbDataIn; cT++ )
    {
        // Copy the bytes from the io to the buffer.
        if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
        {
            if( 0 != mmioAdvance( m_hmmio, &mmioinfoIn, MMIO_READ ) )
                return E_FAIL;

            if( mmioinfoIn.pchNext == mmioinfoIn.pchEndRead )
                return E_FAIL;
        }

        // Actual copy.
        *( ( BYTE* )pBuffer + cT ) = *( ( BYTE* )mmioinfoIn.pchNext );
        mmioinfoIn.pchNext++;
    }

    if( 0 != mmioSetInfo( m_hmmio, &mmioinfoIn, 0 ) )
        return E_FAIL;

    if( pdwSizeRead != NULL )
        *pdwSizeRead = cbDataIn;

    return S_OK;
}

HRESULT WaveDecoder::ResetFile() 
{
	if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

    if( m_dwFlags == WAVEFILE_READ )
    {
        // Seek to the data
        if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof( FOURCC ),
                            SEEK_SET ) )
            return E_FAIL;

        // Search the input file for the 'data' chunk.
        m_ck.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
        if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
            return E_FAIL;
    }
    else
    {
        // Create the 'data' chunk that holds the waveform samples.
        m_ck.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
        m_ck.cksize = 0;

        if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
            return E_FAIL;

        if( 0 != mmioGetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
            return E_FAIL;
    }
	return S_OK;
}

HRESULT WaveDecoder::Close() 
{
	if( m_dwFlags == WAVEFILE_READ )
    {
        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
        SAFE_DELETE_ARRAY( m_pResourceBuffer );
    }
    else
    {
        m_mmioinfoOut.dwFlags |= MMIO_DIRTY;

        if( m_hmmio == NULL )
            return CO_E_NOTINITIALIZED;

        if( 0 != mmioSetInfo( m_hmmio, &m_mmioinfoOut, 0 ) )
            return E_FAIL;

        // Ascend the output file out of the 'data' chunk -- this will cause
        // the chunk size of the 'data' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
            return E_FAIL;

        // Do this here instead...
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return E_FAIL;

        mmioSeek( m_hmmio, 0, SEEK_SET );

        if( 0 != ( INT )mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) )
            return E_FAIL;

        m_ck.ckid = mmioFOURCC( 'f', 'a', 'c', 't' );

        if( 0 == mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
        {
            DWORD dwSamples = 0;
            mmioWrite( m_hmmio, ( HPSTR )&dwSamples, sizeof( DWORD ) );
            mmioAscend( m_hmmio, &m_ck, 0 );
        }

        // Ascend the output file out of the 'RIFF' chunk -- this will cause
        // the chunk size of the 'RIFF' chunk to be written.
        if( 0 != mmioAscend( m_hmmio, &m_ckRiff, 0 ) )
            return E_FAIL;

        mmioClose( m_hmmio, 0 );
        m_hmmio = NULL;
    }

    return S_OK;
}

HRESULT WaveDecoder::Seek(float duration ,DWORD *newPosition) 
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
		if( -1 == mmioSeek( m_hmmio, m_ckRiff.dwDataOffset + sizeof( FOURCC ) + seekSize,
                    SEEK_SET ) )
            return E_FAIL;

        // Search the input file for the 'data' chunk.
        //m_ck.ckid = mmioFOURCC( 'd', 'a', 't', 'a' );
        //if( 0 != mmioDescend( m_hmmio, &m_ck, &m_ckRiff, MMIO_FINDCHUNK ) )
        //    return E_FAIL;
	}
	
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: CWaveFile::ReadMMIO()
// Desc: Support function for reading from a multimedia I/O stream.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_pwfx.
//-----------------------------------------------------------------------------
HRESULT WaveDecoder::ReadMMIO()
{
    MMCKINFO ckIn;           // chunk info. for general use.
    PCMWAVEFORMAT pcmWaveFormat;  // Temp PCM structure to load in.

    memset( &ckIn, 0, sizeof(ckIn) );

    m_pwfx = NULL;

    if( ( 0 != mmioDescend( m_hmmio, &m_ckRiff, NULL, 0 ) ) )
        //return DXTRACE_ERR( L"mmioDescend", E_FAIL );
		return E_FAIL;

    // Check to make sure this is a valid wave file
    if( ( m_ckRiff.ckid != FOURCC_RIFF ) ||
        ( m_ckRiff.fccType != mmioFOURCC( 'W', 'A', 'V', 'E' ) ) )
        //return DXTRACE_ERR( L"mmioFOURCC", E_FAIL );
		return E_FAIL;

    // Search the input file for for the 'fmt ' chunk.
    ckIn.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
    if( 0 != mmioDescend( m_hmmio, &ckIn, &m_ckRiff, MMIO_FINDCHUNK ) )
        //return DXTRACE_ERR( L"mmioDescend", E_FAIL );
		return E_FAIL;

    // Expect the 'fmt' chunk to be at least as large as <PCMWAVEFORMAT>;
    // if there are extra parameters at the end, we'll ignore them
    if( ckIn.cksize < ( LONG )sizeof( PCMWAVEFORMAT ) )
        //return DXTRACE_ERR( L"sizeof(PCMWAVEFORMAT)", E_FAIL );
		return E_FAIL;

    // Read the 'fmt ' chunk into <pcmWaveFormat>.
    if( mmioRead( m_hmmio, ( HPSTR )&pcmWaveFormat,
                  sizeof( pcmWaveFormat ) ) != sizeof( pcmWaveFormat ) )
        //return DXTRACE_ERR( L"mmioRead", E_FAIL );
		return E_FAIL;

    // Allocate the waveformatex, but if its not pcm format, read the next
    // word, and thats how many extra bytes to allocate.
    if( pcmWaveFormat.wf.wFormatTag == WAVE_FORMAT_PCM )
    {
        m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) ];
        if( NULL == m_pwfx )
            //return DXTRACE_ERR( L"m_pwfx", E_FAIL );
			return E_FAIL;

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
        m_pwfx->cbSize = 0;
    }
    else
    {
        // Read in length of extra bytes.
        WORD cbExtraBytes = 0L;
        if( mmioRead( m_hmmio, ( CHAR* )&cbExtraBytes, sizeof( WORD ) ) != sizeof( WORD ) )
            //return DXTRACE_ERR( L"mmioRead", E_FAIL );
			return E_FAIL;

        m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) + cbExtraBytes ];
        if( NULL == m_pwfx )
            //return DXTRACE_ERR( L"new", E_FAIL );
			return E_FAIL;

        // Copy the bytes from the pcm structure to the waveformatex structure
        memcpy( m_pwfx, &pcmWaveFormat, sizeof( pcmWaveFormat ) );
        m_pwfx->cbSize = cbExtraBytes;

        // Now, read those extra bytes into the structure, if cbExtraAlloc != 0.
        if( mmioRead( m_hmmio, ( CHAR* )( ( ( BYTE* )&( m_pwfx->cbSize ) ) + sizeof( WORD ) ),
                      cbExtraBytes ) != cbExtraBytes )
        {
            SAFE_DELETE( m_pwfx );
            //return DXTRACE_ERR( L"mmioRead", E_FAIL );
			return E_FAIL;
        }
    }

    // Ascend the input file out of the 'fmt ' chunk.
    if( 0 != mmioAscend( m_hmmio, &ckIn, 0 ) )
    {
        SAFE_DELETE( m_pwfx );
        //return DXTRACE_ERR( L"mmioAscend", E_FAIL );
		return E_FAIL;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: CWaveFile::WriteMMIO()
// Desc: Support function for reading from a multimedia I/O stream
//       pwfxDest is the WAVEFORMATEX for this new wave file.
//       m_hmmio must be valid before calling.  This function uses it to
//       update m_ckRiff, and m_ck.
//-----------------------------------------------------------------------------
HRESULT WaveDecoder::WriteMMIO( WAVEFORMATEX* pwfxDest )
{
    DWORD dwFactChunk; // Contains the actual fact chunk. Garbage until WaveCloseWriteFile.
    MMCKINFO ckOut1;

    memset( &ckOut1, 0, sizeof(ckOut1) );

    dwFactChunk = ( DWORD )-1;

    // Create the output file RIFF chunk of form type 'WAVE'.
    m_ckRiff.fccType = mmioFOURCC( 'W', 'A', 'V', 'E' );
    m_ckRiff.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &m_ckRiff, MMIO_CREATERIFF ) )
        //return DXTRACE_ERR( L"mmioCreateChunk", E_FAIL );
		return E_FAIL;

    // We are now descended into the 'RIFF' chunk we just created.
    // Now create the 'fmt ' chunk. Since we know the size of this chunk,
    // specify it in the MMCKINFO structure so MMIO doesn't have to seek
    // back and set the chunk size after ascending from the chunk.
    m_ck.ckid = mmioFOURCC( 'f', 'm', 't', ' ' );
    m_ck.cksize = sizeof( PCMWAVEFORMAT );

    if( 0 != mmioCreateChunk( m_hmmio, &m_ck, 0 ) )
        //return DXTRACE_ERR( L"mmioCreateChunk", E_FAIL );
		return E_FAIL;

    // Write the PCMWAVEFORMAT structure to the 'fmt ' chunk if its that type.
    if( pwfxDest->wFormatTag == WAVE_FORMAT_PCM )
    {
        if( mmioWrite( m_hmmio, ( HPSTR )pwfxDest,
                       sizeof( PCMWAVEFORMAT ) ) != sizeof( PCMWAVEFORMAT ) )
            //return DXTRACE_ERR( L"mmioWrite", E_FAIL );
			return E_FAIL;
    }
    else
    {
        // Write the variable length size.
        if( ( UINT )mmioWrite( m_hmmio, ( HPSTR )pwfxDest,
                               sizeof( *pwfxDest ) + pwfxDest->cbSize ) !=
            ( sizeof( *pwfxDest ) + pwfxDest->cbSize ) )
            //return DXTRACE_ERR( L"mmioWrite", E_FAIL );
			return E_FAIL;
    }

    // Ascend out of the 'fmt ' chunk, back into the 'RIFF' chunk.
    if( 0 != mmioAscend( m_hmmio, &m_ck, 0 ) )
        //return DXTRACE_ERR( L"mmioAscend", E_FAIL );
		return E_FAIL;

    // Now create the fact chunk, not required for PCM but nice to have.  This is filled
    // in when the close routine is called.
    ckOut1.ckid = mmioFOURCC( 'f', 'a', 'c', 't' );
    ckOut1.cksize = 0;

    if( 0 != mmioCreateChunk( m_hmmio, &ckOut1, 0 ) )
        //return DXTRACE_ERR( L"mmioCreateChunk", E_FAIL );
		return E_FAIL;

    if( mmioWrite( m_hmmio, ( HPSTR )&dwFactChunk, sizeof( dwFactChunk ) ) !=
        sizeof( dwFactChunk ) )
        //return DXTRACE_ERR( L"mmioWrite", E_FAIL );
		return E_FAIL;

    // Now ascend out of the fact chunk...
    if( 0 != mmioAscend( m_hmmio, &ckOut1, 0 ) )
        //return DXTRACE_ERR( L"mmioAscend", E_FAIL );
		return E_FAIL;

    return S_OK;
}
