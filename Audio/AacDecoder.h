#pragma once
#include "audiodecoder.h"
#include "..\decoders\faad\include\neaacdec.h"


//#define ADIF_MAX_SIZE 30 /* Should be enough */
//#define ADTS_MAX_SIZE 10 /* Should be enough */
//
//typedef struct {
//    int version;
//    int channels;
//    int sampling_rate;
//    int bitrate;
//    int length;
//    int object_type;
//    int headertype;
//} faadAACInfo;

//static int sample_rates[] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};

#define MAX_CHANNELS 8 /* make this higher to support files with
                          more channels */

/* FAAD file buffering routines */
/* declare buffering variables */
//#define DEC_BUFF_VARS \
//    int fileread, bytesconsumed, k; \
//    int buffercount = 0, buffer_index = 0; \
//    unsigned char *buffer; \
//    unsigned int bytes_in_buffer = 0;

/* initialise buffering */
#define INIT_BUFF(file) \
    fseek(file, 0, SEEK_END); \
    fileread = ftell(file); \
    fseek(file, 0, SEEK_SET); \
    buffer = (unsigned char*)malloc(FAAD_MIN_STREAMSIZE*MAX_CHANNELS); \
    memset(buffer, 0, FAAD_MIN_STREAMSIZE*MAX_CHANNELS); \
    bytes_in_buffer = fread(buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, file);

/* skip bytes in buffer */
#define UPDATE_BUFF_SKIP(bytes) \
    fseek(infile, bytes, SEEK_SET); \
    buffer_index += bytes; \
    buffercount = 0; \
    bytes_in_buffer = fread(buffer, 1, FAAD_MIN_STREAMSIZE*MAX_CHANNELS, infile);

/* update buffer */
#define UPDATE_BUFF_READ \
    if (bytesconsumed > 0) { \
        for (k = 0; k < (FAAD_MIN_STREAMSIZE*MAX_CHANNELS - bytesconsumed); k++) \
            buffer[k] = buffer[k + bytesconsumed]; \
        bytes_in_buffer += fread(buffer + (FAAD_MIN_STREAMSIZE*MAX_CHANNELS) - bytesconsumed, 1, bytesconsumed, infile); \
        bytesconsumed = 0; \
    }

/* update buffer indices after NeAACDecDecode */
#define UPDATE_BUFF_IDX(frame) \
    bytesconsumed += frame.bytesconsumed; \
    buffer_index += frame.bytesconsumed; \
    bytes_in_buffer -= frame.bytesconsumed;

/* true if decoding has to stop because of EOF */
#define IS_FILE_END buffer_index >= fileread

/* end buffering */
#define END_BUFF if (buffer) free(buffer);

#define FRAME_INCRE_SIZE 1024
class AacDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) AacDecoder(void);
	__declspec(dllexport) ~AacDecoder(void);
public:
	// overrid function
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
//public:
//	int read_ADTS_header(faadAACInfo *info, int *framesNum);
//	int get_AAC_format(faadAACInfo *info, int *framesNum);
private:
	int id3v2_tag(unsigned char *);
private:
	FILE *infile;
	int first_time;

	NeAACDecHandle hDecoder;
    NeAACDecFrameInfo frameInfo;
    NeAACDecConfigurationPtr config;

	// aac
	int fileread, bytesconsumed, k; 
    int buffercount , buffer_index ; 
    unsigned char *buffer; 
    unsigned int bytes_in_buffer ;
	int frames;
	unsigned int m_nSamplesPerFrame;
	BOOL m_seek;
	int m_seekLeftBytes;
	unsigned long *m_seek_table;
	int seek_table_len;
};

