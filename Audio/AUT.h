
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

//-----------------------------------------------------------------------------
// Typing macros 
//-----------------------------------------------------------------------------
#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

// error code 
#define SUCESS_CODE_START 0x21000000L
#define INFORMATION_CODE_START 0x61000000L
#define WARNING_CODE_START 0xA1000000L
#define ERROR_CODE_START 0xE1000000L

// 1-50
#define M_SUCESS									_HRESULT_TYPEDEF_((SUCESS_CODE_START + 0x1L))

#define M_FAIL										_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1L)
#define M_FILE_EXTENSION_NOT_EXPECTED				_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x2L)
#define M_INVALID_PARAMETERS						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x3L)
#define M_DURATION_OUT_OF_RANGE						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x4L)
#define M_SEEK_FAIL									_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x5L)
#define M_FLAC_FAIL_CREATE_DECODER					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x6L)
#define M_FLAC_FAIL_INIT_FILE						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x7L)
#define M_FLAC_FAIL_DECODE_META						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x8L)
#define M_FLAC_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x9L)
#define M_FLAC_FAIL_DECODER_RESET					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0xAL)
#define M_FLAC_FAIL_DECODER_FLUSH					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0xBL)
#define M_APE_FAIL_CREATE_DECODER					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0xCL)
#define M_APE_FAIL_GET_WAVE_INFO					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0xDL)
#define M_APE_FAIL_GET_INFO							_HRESULT_TYPEDEF_(ERROR_CODE_START + 0xEL)
#define M_APE_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0xFL)
#define M_AAC_FAIL_GET_INFO							_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x10L)
#define M_FAIL_OPEN_FILE							_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x11L)
#define M_AAC_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x12L)
#define M_MP3_FAIL_CREATE_DECODER					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x13L)
#define M_MP3_FAIL_GET_WAVE_INFO					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x14L)
#define M_MP3_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x15L)
#define M_MP4_FAIL_CREATE_CALLBACK					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x16L)
#define M_MP4_FAIL_CREATE_DECODER					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x17L)
#define M_MP4_FAIL_GET_TRACK						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x18L)
#define M_MP4_FAIL_GET_WAVE_INFO					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x19L)
#define M_MP4_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1AL)
#define M_OGG_FAIL_CREATE_DECODER					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1BL)
#define M_OGG_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1CL)
#define M_TTA_FAIL_CREATE_DECODER					_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1DL)
#define M_TTA_FAIL_GET_INFO							_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1EL)
#define M_TTA_FAIL_DECODE_DATA						_HRESULT_TYPEDEF_(ERROR_CODE_START + 0x1FL)

#pragma once
#include <Windows.h>
#include <atlbase.h>
#include <tchar.h>
#include <strsafe.h>

static BOOL isFile_ex(LPWSTR filePath,char *ex)
{
	USES_CONVERSION;
	char *path = W2A(filePath);
	int length = strlen(path);
	char file_ex[10];
	int i = length - 1;

	while ( i >= 0 && '.' != path[i] )
	{
		i--;
	}

	if ( i < 0 )
	{
		return FALSE;
	}
	i++;
	int j = 0;
	while ( i < length )
	{
		file_ex[j++] = path[i++];
	}
	file_ex[j] = '\0';
	
	if(0 == _stricmp(ex,file_ex))
	{		
		strcpy_s(ex,length + 1,path);
		return TRUE;
	}
	strcpy_s(ex,length + 1,path);
	return FALSE;
}

static int StringComp(char const *str1, char const *str2, int len)
{
	signed int c1 = 0, c2 = 0;

    while (len--) {
        c1 = *str1++;
        c2 = *str2++;

        if (c1 == 0 || c1 != c2)
            break;
    }

    return c1 - c2;
}

static long * reallocate_aac_ptr(long *src, unsigned int old_size, unsigned int new_size)
{
	if(new_size == 0)	// request to free src
	{
		if(src != NULL)
			free(src);

		return NULL;
	}


	// allocate new memory
	long *ptr = (long*) malloc(new_size * sizeof(long));
	if(ptr == NULL)
	{
		if(src)
			free(src);

		return NULL;
	}

	// copy old data into new memory
	unsigned int size = old_size < new_size ? old_size : new_size;
	if(src)
	{
		unsigned int i;
		for(i = 0; i < size; i++)
			ptr[i] = src[i];

		free(src);
	}

	return ptr;
}

static WCHAR *errMes[]=
{
	L"Unknown error!",
	L"File extension not correct!",
	L"Invalid Parameters!",
	L"Seek duration out of range!",
	L"Fail to seek!",
	L"Fail to create Flac decoder!",
	L"Flac decoder fail to open file!",
	L"Flac decoder fail to decode meta data!",
	L"Flac decoder fail to decode data!",
	L"Flac decoder fail to reset!",
	L"Flac decoder fail to flush",
	L"Fail to create APE decoder!",
	L"APE decoder fail to get wave header!",
	L"APE decoder fail to get infomation of the media file!",
	L"APE decoder fail to decode data!",
	L"AAC decoder fail to get wave data!",
	L"Fail to open file!",
	L"AAC decoder fail to decode data!",
	L"Fail to create Mp3 decoder!",
	L"Mp3 decoder fail to get wave header!",
	L"Mp3 decoder fail to decode data!",
	L"Fail to create Mp4 callbacks!",
	L"Fail to create Mp4 Decoder!",
	L"Mp4 Decoder fail to get track!",
	L"Mp4 Decoder fail to get wave header!",
	L"Mp4 Decoder fail to decode data!",
	L"Fail to create Ogg decoder!"
	L"Ogg decoder fail to decode data!",
	L"Fail to create Tta decoder!"
	L"Tta decoder fail to get wave header!",
	L"Tta decoder dail to decode data!"
};

static WCHAR * getErrMes(long errCode)
{
	long code = errCode - ERROR_CODE_START - 1;
	if(code >=0 && code <= 30)
	{
		return errMes[code];
	}
	return L"Unknown error code!";
}

