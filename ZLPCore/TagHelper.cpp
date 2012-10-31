#include "TagHelper.h"
#include <exception>
using namespace std;

CTagHelper::CTagHelper(void)
{
	Init();
	tag = new Tags;
	m_bmpSize.SetSize(0,0);
}


CTagHelper::~CTagHelper(void)
{
	Destory();
	if(tag)
		tag->Link(L"");
	delete tag;
}

int CTagHelper::GetTextTag(const wchar_t *tagName,wchar_t *tag)
{
	assert(NULL != tagName );
	if(0 == wcscmp(tagName,L"TALB"))
	{
		if(NULL != tag && NULL != m_album)
		{
			wcscpy(tag,m_album);
		}
		return NULL != m_album?wcslen(m_album):0;
	}
	else if(0 == wcscmp(tagName,L"TPE1"))
	{
		if(NULL != tag && NULL != m_artist)
		{
			wcscpy(tag,m_artist);
		}
		return NULL != m_artist?wcslen(m_artist):0;
	}
	else if(0 == wcscmp(tagName,L"COMM"))
	{
		if(NULL != tag && NULL != m_comments)
		{
			wcscpy(tag,m_comments);
		}
		return NULL != m_comments?wcslen(m_comments):0;
	}
	else if(0 == wcscmp(tagName,L"USLT"))
	{
		if(NULL != tag && NULL != m_uslt)
		{
			wcscpy(tag,m_uslt);
		}
		return NULL != m_uslt?wcslen(m_uslt):0;
	}
	else if(0 == wcscmp(tagName,L"TIT2"))
	{
		if(NULL != tag && NULL != m_title)
		{
			wcscpy(tag,m_title);
		}
		return NULL != m_title?wcslen(m_title):0;
	}
	else if(0 == wcscmp(tagName,L"TRCK"))
	{
		if(NULL != tag && NULL != m_track)
		{
			wcscpy(tag,m_track);
		}
		return NULL != m_track?wcslen(m_track):0;
	}
	else if(0 == wcscmp(tagName,L"TYER"))
	{
		if(NULL != tag && NULL != m_year)
		{
			wcscpy(tag,m_year);
		}
		return NULL != m_year?wcslen(m_year):0;
	}
	return 0;
}

const wchar_t* CTagHelper::GetTextTag(const wchar_t *tagName)
{
	assert(NULL != tagName );
	if(0 == wcscmp(tagName,L"TALB"))
	{
		return m_album;
	}
	else if(0 == wcscmp(tagName,L"TPE1"))
	{
		return m_artist;
	}
	else if(0 == wcscmp(tagName,L"COMM"))
	{
		return m_comments;
	}
	else if(0 == wcscmp(tagName,L"USLT"))
	{
		return m_uslt;
	}
	else if(0 == wcscmp(tagName,L"TIT2"))
	{
		return m_title;
	}
	else if(0 == wcscmp(tagName,L"TRCK"))
	{
		return m_track;
	}
	else if(0 == wcscmp(tagName,L"TYER"))
	{
		return m_year;
	}
	return 0;
}

void CTagHelper::Init()
{
	this->m_album = NULL;
	this->m_artist = NULL;
	this->m_comments = NULL;
	this->m_fileName = NULL;
	this->m_genre = NULL;
	this->m_title = NULL;
	this->m_track = 0;
	this->m_year = 0;
	m_uslt = NULL;
	hbmp = NULL;
}

void CTagHelper::Destory()
{
	if(NULL != this->m_album)
	{
		free( this->m_album);
		this->m_album = 0;
	}
	if(NULL != this->m_artist)
	{
		free( this->m_artist);
		this->m_artist = 0;
	}
	if(NULL != this->m_comments)
	{
		free( this->m_comments);
		this->m_comments = 0;
	}
	if(NULL != this->m_fileName)
	{
		free(  this->m_fileName);
		this->m_fileName = 0;
	}
	if(NULL != this->m_genre)
	{
		free(  this->m_genre);
		this->m_genre = 0;
	}
	if(NULL != this->m_title)
	{
		free(  this->m_title);
		this->m_title = 0;
	}
	if(NULL != this->m_year)
	{
		free(  this->m_year);
		this->m_year = 0;
	}
	if(NULL != this->m_track)
	{
		free(  this->m_track);
		this->m_track = 0;
	}
	if(NULL != m_uslt)
	{
		free(  m_uslt);
		m_uslt = 0;
	}
	if(NULL != hbmp)
	{
		::DeleteObject(hbmp);
		hbmp = 0;
	}
}

int CTagHelper::Link(const wchar_t *file)
{
	Destory();
	int r = tag->Link(file);		
	unsigned int index = 0;
	ID3Frame *f = tag->FindFrame(L"TALB",&index);
	if(NULL != f)
	{
		CopyTagData(f->data,&m_album,wcslen(f->data));
	}
	f = tag->FindFrame(L"TPE1",&index);
	if(NULL != f)
	{
		CopyTagData(f->data,&m_artist,wcslen(f->data));
	}
	f = tag->FindFrame(L"COMM",&index);
	if(NULL != f)
	{
		CopyTagData(f->data,&m_comments,wcslen(f->data));
	}
	f = tag->FindFrame(L"TIT2",&index);
	if(NULL != f)
	{
		CopyTagData(f->data,&m_title,wcslen(f->data));
	}
	f = tag->FindFrame(L"TRCK",&index);
	if(NULL != f)
	{
		CopyTagData(f->data,&m_track,wcslen(f->data));
	}
	f = tag->FindFrame(L"TYER",&index);
	if(NULL != f)
	{
		CopyTagData(f->data,&m_year,wcslen(f->data));
	}

	f = tag->FindFrame("APIC",&index);

	if(NULL != f)
	{
		ID3Bitmap * id3Bitmap = tag->GetHBITMAP(index);
		if(id3Bitmap && id3Bitmap->hbm)
		{
			m_bmpSize.SetSize(id3Bitmap->width,id3Bitmap->height);
			hbmp = id3Bitmap->hbm;
		}
	}
	return r;
}


const HBITMAP CTagHelper::GetBitmap(int *w,int *h) 
{
	*w = m_bmpSize.width;
	*h = m_bmpSize.height;
	return hbmp;
}

 bool CTagHelper::HasImage()
 {
	 return (0 != hbmp);
 }



int CTagHelper::CopyTagData(wchar_t *src,wchar_t **des,int length)
{
	assert(NULL != des);
	*des = (wchar_t*)malloc(sizeof(wchar_t)*(length+1));
	wcscpy(*des,src);
	(*des)[length] = '\0';
	
	return 0;
}

