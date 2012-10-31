#include "AudioDecoder.h"
#include "..\TagLibrary\Tags.h"

AudioDecoder::AudioDecoder(void)
{
	m_fileName = NULL;
	m_pwfx = ( WAVEFORMATEX* )new CHAR[ sizeof( WAVEFORMATEX ) ];
	m_nWaveDataAllocSize = m_nWaveDataNum = 0;
	m_dwSize = 0;
	m_pWaveDataAlloc = m_pWaveDataBuffer = NULL;
}


AudioDecoder::~AudioDecoder(void)
{
	SAFE_DELETE_ARRAY(m_fileName);
	SAFE_DELETE_ARRAY(m_pwfx);
	SAFE_DELETE_ARRAY(m_pWaveDataAlloc);
}

WAVEFORMATEX* AudioDecoder::GetFormat()
{
	return m_pwfx;
}

DWORD AudioDecoder::GetSize()
{
	return m_dwSize;
}

LPWSTR AudioDecoder::GetFileName()
{
	return m_fileName;
}

ID3Info *AudioDecoder::GetID3Info()
{
	return &m_id3;
}

void AudioDecoder::LoadID3(wchar_t *file)
{
	m_id3.EmptyTags();
	Tags *tag = new Tags;
	int r = tag->Link(file);		
	unsigned int index = 0;
	ID3Frame *f = tag->FindFrame(L"TALB",&index);
	if(NULL != f)
	{		
		m_id3.album.SetData(f->data,wcslen(f->data));
	}
	f = tag->FindFrame(L"TPE1",&index);
	if(NULL != f)
	{
		m_id3.artist.SetData(f->data,wcslen(f->data));
	}
	f = tag->FindFrame(L"COMM",&index);
	if(NULL != f)
	{
		m_id3.comment.SetData(f->data,wcslen(f->data));
	}
	f = tag->FindFrame(L"TIT2",&index);
	if(NULL != f)
	{
		m_id3.title.SetData(f->data,wcslen(f->data));
	}
	f = tag->FindFrame(L"TRCK",&index);
	if(NULL != f)
	{
		m_id3.track.SetData(f->data,wcslen(f->data));
	}
	f = tag->FindFrame(L"TYER",&index);
	if(NULL != f)
	{
		m_id3.year.SetData(f->data,wcslen(f->data));
	}


	f = tag->FindFrame("APIC",&index);

	if(NULL != f)
	{
		ID3Bitmap * bmp = tag->GetHBITMAP(index);
		if(bmp && bmp->hbm)
		{
			m_id3.id3Bitmap->width = bmp->width;
			m_id3.id3Bitmap->height = bmp->height;
			// copy hbitmap
			HDC sourcedc;
			HDC destdc;
			sourcedc = CreateCompatibleDC(NULL);
			destdc = CreateCompatibleDC(NULL);
			//the bitmap information.
			BITMAP bm = {0};
			//get the bitmap information.
			::GetObject(bmp->hbm, sizeof(bm), &bm);
			// create a bitmap to hold the result

			//HDC memDC = CreateCompatibleDC(0);
			m_id3.id3Bitmap->hbm = ::CreateCompatibleBitmap(GetDC(0), bm.bmWidth, bm.bmHeight);
			//SelectObject(memDC,m_id3.id3Bitmap->hbm);
			HBITMAP hbmoldsource = (HBITMAP)::SelectObject(sourcedc, bmp->hbm);
			HBITMAP hbmolddest = (HBITMAP)::SelectObject(destdc,  m_id3.id3Bitmap->hbm);
			BitBlt(destdc,0,0,bm.bmWidth, bm.bmHeight, sourcedc, 0, 0, SRCCOPY);

			//   restore   dcs
			::SelectObject(sourcedc,  hbmoldsource);
			::SelectObject(destdc,  hbmolddest);
			::DeleteObject(sourcedc);
			::DeleteObject(destdc);
			//::DeleteObject(memDC);
		}
	}
	delete tag;
}