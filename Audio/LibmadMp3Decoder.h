#pragma once
#include "audiodecoder.h"
#include "..\decoders\libmad\mad.h"

#define BUFSIZE 65536
typedef struct buffer {
  FILE  *fp;                    /*file pointer*/
  unsigned int  flen;           /*file length*/
  unsigned int  fpos;           /*current position*/
  unsigned char fbuf[BUFSIZE];  /*buffer*/
  unsigned int  fbsize;         /*indeed size of buffer*/
}buffer;

class LibmadMp3Decoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) LibmadMp3Decoder(void);
	__declspec(dllexport) ~LibmadMp3Decoder(void);
public:
	__declspec(dllexport) virtual HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) ;
	__declspec(dllexport) virtual HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) ;
	__declspec(dllexport) virtual HRESULT ResetFile() ;
	__declspec(dllexport) virtual HRESULT Close() ;
	__declspec(dllexport) virtual HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	static enum mad_flow input(void *data,struct mad_stream *stream);
	static inline signed int scale(mad_fixed_t sample);
	static enum mad_flow output(void *data,struct mad_header const *header,struct mad_pcm *pcm);
	static enum mad_flow error(void *data,struct mad_stream *stream,struct mad_frame *frame);
	static enum mad_flow header(void *data, struct mad_header const *h);
private:
	struct mad_decoder decoder;
	buffer mp3_file;
};

