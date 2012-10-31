#pragma once
#include "audiodecoder.h"
#include "..\decoders\libtta\libtta.h"



#define PCM_BUFFER_LENGTH 5120
#define tta_read(__handle,__buffer,__size,__result) ReadFile(__handle,__buffer,__size,(LPDWORD)&(__result),NULL)
#define tta_seek(__handle,__offset) SetFilePointer(__handle,(LONG)__offset,(PLONG)&__offset+1,FILE_BEGIN)
#define tta_close(__handle) (CloseHandle(__handle)==TRUE?(0):(-1))
#define tta_open_read(__name) CreateFileW(__name,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL)

typedef struct {
	TTA_io_callback iocb;
	HANDLE handle;
} TTA_io_callback_wrapper;

using namespace tta;

class TtaDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) TtaDecoder(void);
	__declspec(dllexport) ~TtaDecoder(void);

public:
	__declspec(dllexport) virtual HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) ;
	__declspec(dllexport) virtual HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) ;
	__declspec(dllexport) virtual HRESULT ResetFile() ;
	__declspec(dllexport) virtual HRESULT Close() ;
	__declspec(dllexport) virtual HRESULT Seek(float duration ,DWORD *newPosition ) ;

private:
	tta_decoder *TTA;
	TTA_io_callback_wrapper io;
	HANDLE infile;
	int ret;
private:
	static TTAint32 CALLBACK TtaDecoder::read_callback(TTA_io_callback *io, TTAuint8 *buffer, TTAuint32 size);
	static TTAint64 CALLBACK TtaDecoder::seek_callback(TTA_io_callback *io, TTAint64 offset) ;
};

