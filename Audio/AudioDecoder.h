#pragma once

#include <Windows.h>
#include <assert.h>

#include "AUT.h"
#include "ID3Info.h"

class AudioDecoder
{
public:
	__declspec(dllexport) AudioDecoder(void);
	__declspec(dllexport) virtual ~AudioDecoder(void);
public:
	__declspec(dllexport) LPWSTR GetFileName();
	__declspec(dllexport) WAVEFORMATEX* GetFormat();
	__declspec(dllexport) DWORD GetSize();
	__declspec(dllexport) ID3Info *GetID3Info();
	// virtual functions
	__declspec(dllexport) virtual HRESULT Open( LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags ) = 0;
	__declspec(dllexport) virtual HRESULT Read( BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead ) = 0;
	__declspec(dllexport) virtual HRESULT ResetFile() = 0;
	__declspec(dllexport) virtual HRESULT Close() = 0;
	__declspec(dllexport) virtual HRESULT Seek(float duration ,DWORD *newPosition ) = 0;
	void LoadID3(wchar_t *);
	
protected:
	ID3Info m_id3;
	WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
	DWORD m_dwSize;      // The size of the wave file
	LPWSTR m_fileName;
	DWORD m_dwFlags;
protected:
	BYTE *m_pWaveDataAlloc;// the buffer store the decorded audio data
	unsigned int m_nWaveDataAllocSize; // the buffer size

	BYTE *m_pWaveDataBuffer;// current buffer location inside samples buffer 
	unsigned int m_nWaveDataNum;		// number of samples in samples buffer
};

