#pragma once

#define APE_BLOCKS_NUM 1024
#include "audiodecoder.h"
#include "..\decoders\MAC_SDK\Shared\All.h"
#include "..\decoders\MAC_SDK\Shared\APETag.h"
#include "..\decoders\MAC_SDK\Shared\MACLib.h"


class ApeDecoder :
	public AudioDecoder
{
public:
	__declspec(dllexport) ApeDecoder(void);
	__declspec(dllexport) ~ApeDecoder(void);
public:
	// overrid function
	__declspec(dllexport) HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags );
	__declspec(dllexport) HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead );
	__declspec(dllexport) HRESULT ResetFile() ;
	__declspec(dllexport) HRESULT Close() ;
	__declspec(dllexport) HRESULT Seek(float duration ,DWORD *newPosition ) ;
private:
	IAPEDecompress * m_pAPEDecompress;
	unsigned int m_total_blocks;
	unsigned int m_current_blocks; 
	int m_block_algin;
};

